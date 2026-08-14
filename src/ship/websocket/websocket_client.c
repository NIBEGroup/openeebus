/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief Websocket Uri implementation
 */

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/string_util.h"
#include "src/ship/websocket/websocket.h"
#include "src/ship/websocket/websocket_debug.h"
#include "src/ship/websocket/websocket_internal.h"

typedef struct WebsocketClient WebsocketClient;

struct WebsocketClient {
  /** Implements the Websocket Interface */
  Websocket obj;

  bool cancel;
  EebusThreadObject* thread;

  char* uri;
  const char* address;
  const char* path;
  int port;
  const TlsCertificateObject* tls_cert;
  const char* remote_ski;
  struct lws_protocols protocols[2];
  struct lws_client_connect_info* lws_connect_info;
};

#define WEBSOCKET_CLIENT(obj) ((WebsocketClient*)(obj))

static void Destruct(WebsocketObject* self);

static const WebsocketInterface websocket_client_methods = {
    .destruct        = Destruct,
    .write           = WebsocketWrite,
    .close           = WebsocketClose,
    .is_closed       = WebsocketIsClosed,
    .get_close_error = WebsocketGetCloseError,
    .schedule_write  = WebsocketScheduleWrite,
};

static EebusError WebsocketClientConstruct(
    WebsocketClient* self,
    const char* uri,
    const TlsCertificateObject* tls_cert,
    const char* remote_ski,
    WebsocketCallback cb,
    void* ctx
);
static void* WebsocketClientLoop(void* parameters);
static struct lws_client_connect_info* WebsocketClientConnectInfoCreate(WebsocketClient* self);
static struct lws_context* WebsocketClientLwsContextCreate(WebsocketClient* self);
static EebusError WebsocketClientParse(WebsocketClient* self);
static EebusError WebsocketClientTryStart(WebsocketClient* self);
static int WebsocketClientOnClientEstablished(WebsocketClient* self);
static int WebsocketClientOnClientConnectionError(WebsocketClient* self, const char* in, size_t len);
static int WebsocketClientOnWsiDestroy(WebsocketClient* self);
static int
WebsocketClientServiceCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

EebusError WebsocketClientConstruct(
    WebsocketClient* self,
    const char* uri,
    const TlsCertificateObject* tls_cert,
    const char* remote_ski,
    WebsocketCallback cb,
    void* ctx
) {
  const EebusError ret = WebsocketConstruct(WEBSOCKET(self), cb, ctx);
  // Override "virtual functions table"
  WEBSOCKET_INTERFACE(self) = &websocket_client_methods;

  if (ret != kEebusErrorOk) {
    return ret;
  }

  self->cancel = false;
  self->thread = NULL;

  self->uri        = StringCopy(uri);
  self->address    = NULL;
  self->path       = NULL;
  self->port       = 0;
  self->tls_cert   = tls_cert;
  self->remote_ski = StringCopy(remote_ski);

  self->protocols[0]
      = (struct lws_protocols){SHIP_WEBSOCKET_SUB_PROTOCOL, WebsocketClientServiceCallback, 0, 16 * 1024, 0, self, 0};
  self->protocols[1] = (struct lws_protocols)LWS_PROTOCOL_LIST_TERM;

  self->lws_connect_info = NULL;

  return kEebusErrorOk;
}

void* WebsocketClientLoop(void* parameters) {
  Websocket* const ws         = (Websocket*)parameters;
  WebsocketClient* const self = WEBSOCKET_CLIENT(ws);

  ws->wsi = lws_client_connect_via_info(self->lws_connect_info);

  if (ws->wsi == NULL) {
    static const char* const kWebsocketConnectErr = "websocket connect failed";
    WEBSOCKET_DEBUG_PRINTF("%s(), %s!\n", __func__, kWebsocketConnectErr);
    WebsocketUserCallback(ws, kWebsocketCallbackTypeError, (void*)kWebsocketConnectErr, strlen(kWebsocketConnectErr));
    return NULL;
  }

  lws_sul_schedule(ws->lws_ctx, 0, &ws->sul_stagger, WebsocketStaggerCallback, kWebsocketStaggerDelay);

  do {
    lws_service(ws->lws_ctx, 500);
  } while (!self->cancel && ws->wsi != NULL);

  return NULL;
}

struct lws_client_connect_info* WebsocketClientConnectInfoCreate(WebsocketClient* self) {
  Websocket* const ws = WEBSOCKET(self);
  struct lws_client_connect_info* info
      = (struct lws_client_connect_info*)EEBUS_MALLOC(sizeof(struct lws_client_connect_info));

  if (info == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), cannot allocate memory for lws_client_connect_info\n", __func__);
    return NULL;
  }

  memset(info, 0, sizeof(*info));

  static const int kSslConnectionCfg
      = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

  info->address                   = self->address;
  info->port                      = self->port;
  info->path                      = self->path;
  info->context                   = ws->lws_ctx;
  info->ssl_connection            = kSslConnectionCfg;
  info->host                      = info->address;
  info->origin                    = info->address;
  info->ietf_version_or_minus_one = -1;
  info->protocol                  = self->protocols[0].name;
  info->pwsi                      = &ws->wsi;

  return info;
}

struct lws_context* WebsocketClientLwsContextCreate(WebsocketClient* self) {
  // Since we know this lws context is only ever going to be used with
  // a few client wsis / fds / sockets at a time, let lws know it doesn't
  // have to use the default allocations for fd tables up to ulimit -n.
  // It will just allocate for 2 internal and 4 that we might use.
  static const uint32_t kFdLmitPerThread = 2 + 4;

  const struct lws_context_creation_info lws_ctx_creation_info = (struct lws_context_creation_info){
      .port      = CONTEXT_PORT_NO_LISTEN,
      .protocols = self->protocols,
      .gid       = (gid_t)-1,
      .uid       = (uid_t)-1,

      .fd_limit_per_thread = kFdLmitPerThread,
      .options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_H2_JUST_FIX_WINDOW_UPDATE_OVERFLOW,

      .client_ssl_cipher_list
      = "ECDHE-ECDSA-AES128-GCM-SHA256:"
        "ECDHE-ECDSA-AES128-CCM8:"
        "ECDHE-ECDSA-AES128-SHA256",

      .client_ssl_cert_mem     = TLS_CERTIFICATE_GET_CERTIFICATE(self->tls_cert),
      .client_ssl_cert_mem_len = (unsigned int)TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(self->tls_cert),
      .client_ssl_key_mem      = TLS_CERTIFICATE_GET_PRIVATE_KEY(self->tls_cert),
      .client_ssl_key_mem_len  = (unsigned int)TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(self->tls_cert),

      .user = self,
  };

  if (WEBSOCKET_DEBUG == 2) {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_DEBUG;
    lws_set_log_level(logs, NULL);
  }

  return lws_create_context(&lws_ctx_creation_info);
}

// ParseUri() is a drop-in replacement for lws_parse_uri() that is compatible
// with both lws < 5.0 (delegates directly) and lws >= 5.0 (uses the new
// lws_parse_uri_create() API). In the lws >= 5.0 path, the parsed tokens are
// written back into the mutable buffer uri so the returned pointers remain valid
// after the lws_parse_uri_t is destroyed — identical lifetime semantics to the
// old API.
static int ParseUri(char* uri, const char** prot, const char** ads, int* port, const char** path) {
#if LWS_LIBRARY_VERSION_NUMBER >= 5000000
  lws_parse_uri_t* parsed = lws_parse_uri_create(uri);
  if (parsed == NULL) {
    return 1;
  }

  // Write scheme\0host\0path\0 sequentially into uri (always shorter than the
  // original URI) so that the returned pointers into uri outlive parsed.
  char* p = uri;

  *prot    = p;
  size_t n = strlen(parsed->scheme);
  memcpy(p, parsed->scheme, n);
  p += n;
  *p++ = '\0';

  *ads = p;
  n    = strlen(parsed->host);
  memcpy(p, parsed->host, n);
  p += n;
  *p++ = '\0';

  *port = parsed->port;

  *path = p;
  n     = strlen(parsed->path);
  memcpy(p, parsed->path, n);
  p[n] = '\0';

  lws_parse_uri_destroy(&parsed);
  return 0;
#else
  return lws_parse_uri(uri, prot, ads, port, path);
#endif
}

EebusError WebsocketClientParse(WebsocketClient* self) {
  const char* path     = NULL;
  const char* protocol = NULL;

  if (ParseUri(self->uri, &protocol, &self->address, &self->port, &path)) {
    WEBSOCKET_DEBUG_PRINTF("%s(), error parsing uri\n", __func__);
    return kEebusErrorParse;
  }

  if ((protocol == NULL) || (strcmp(protocol, "wss"))) {
    WEBSOCKET_DEBUG_PRINTF("%s(), Unsopported protocol specified", __func__);
    WEBSOCKET_DEBUG_PRINTF("\"%s\"\n", (protocol != NULL) ? protocol : "");
    return kEebusErrorInputArgument;
  }

  self->path = StringFmtSprintf("%s%s", (path[0] != '/') ? "/" : "", path);
  if (self->path == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), error allocating memory for path\n", __func__);
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EebusError WebsocketClientTryStart(WebsocketClient* self) {
  Websocket* const ws = WEBSOCKET(self);

  if (WebsocketClientParse(self) != kEebusErrorOk) {
    WEBSOCKET_DEBUG_PRINTF("%s(), error parsing uri\n", __func__);
    return kEebusErrorParse;
  }

  ws->lws_ctx = WebsocketClientLwsContextCreate(self);
  if (ws->lws_ctx == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), creating libwebsocket context failed\n", __func__);
    return kEebusErrorInit;
  }

  self->lws_connect_info = WebsocketClientConnectInfoCreate(self);
  if (self->lws_connect_info == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), creating libwebsocket connect info failed\n", __func__);
    return kEebusErrorMemory;
  }

  self->thread = EebusThreadCreate(WebsocketClientLoop, self, 10240);
  if (self->thread == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), start websocket client thread failed\n", __func__);
    return kEebusErrorThread;
  }

  return kEebusErrorOk;
}

WebsocketObject* WebsocketClientOpen(
    const char* uri,
    const TlsCertificateObject* tls_cert,
    const char* remote_ski,
    WebsocketCallback cb,
    void* ctx
) {
  WebsocketClient* const ws = (WebsocketClient*)EEBUS_MALLOC(sizeof(WebsocketClient));
  if (ws == NULL) {
    return NULL;
  }

  EebusError ret = WebsocketClientConstruct(ws, uri, tls_cert, remote_ski, cb, ctx);
  if (ret != kEebusErrorOk) {
    WebsocketDelete(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  if (WebsocketClientTryStart(ws) != kEebusErrorOk) {
    WebsocketDelete(WEBSOCKET_OBJECT(ws));
    return NULL;
  }

  return WEBSOCKET_OBJECT(ws);
}

void Destruct(WebsocketObject* self) {
  WebsocketClient* const ws = WEBSOCKET_CLIENT(self);

  ws->cancel = true;

  if (ws->thread != NULL) {
    EEBUS_THREAD_JOIN(ws->thread);
    EebusThreadDelete(ws->thread);
    ws->thread = NULL;
  }

  if (ws->uri != NULL) {
    StringDelete(ws->uri);
    ws->uri = NULL;
  }

  if (ws->remote_ski != NULL) {
    StringDelete((char*)ws->remote_ski);
    ws->remote_ski = NULL;
  }

  if (ws->lws_connect_info != NULL) {
    if (ws->lws_connect_info->path != NULL) {
      StringDelete((char*)ws->lws_connect_info->path);
      ws->lws_connect_info->path = NULL;
    }

    EEBUS_FREE(ws->lws_connect_info);
    ws->lws_connect_info = NULL;
  }

  WebsocketDestruct(self);
}

// LWS Handlers

int WebsocketClientOnClientEstablished(WebsocketClient* self) {
  Websocket* const ws = WEBSOCKET(self);

  if (self->remote_ski == NULL) {
    // Initialisation without trusted SKI is not accepted
    WEBSOCKET_DEBUG_PRINTF("%s(), remote_ski is NULL\n", __func__);
    return -1;
  }

  // Snapshot ws->wsi once. WebsocketClose() can set ws->wsi = NULL from another
  // thread (SIMOPEN superseded connection cleanup path). Reading it once and using only the
  // snapshot eliminates the race window between the two uses below: if it is already
  // NULL we bail out early; if it becomes NULL after the snapshot the LWS wsi object
  // itself is still alive (lws_context_destroy runs only after this thread exits).
  struct lws* const wsi = ws->wsi;
  if (wsi == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), ws->wsi is NULL\n", __func__);
    return -1;
  }

  const char* ski = WebsocketGetSkiWithWsi(wsi);
  if (ski == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), WebsocketGetSkiWithWsi() failed\n", __func__);
    return -1;
  }

  int ret = -1;
  if (strcmp(ski, self->remote_ski) == 0) {
    lws_sul_schedule(ws->lws_ctx, 0, &ws->sul_stagger, WebsocketStaggerCallback, kWebsocketStaggerDelay);
    lws_callback_on_writable(wsi);
    ret = 0;
  } else {
    WEBSOCKET_DEBUG_PRINTF("%s(), server certificate SKI does not match the trusted SKI\n", __func__);
    ret = -1;
  }

  StringDelete((char*)ski);
  return ret;
}

int WebsocketClientOnClientConnectionError(WebsocketClient* self, const char* in, size_t len) {
  Websocket* const ws = WEBSOCKET(self);
  WEBSOCKET_DEBUG_PRINTF("client connection error: %s\n", (in != NULL) ? in : "(null)");
  WebsocketUserCallback(ws, kWebsocketCallbackTypeError, in, len);
  return 0;
}

int WebsocketClientOnWsiDestroy(WebsocketClient* self) {
  Websocket* const ws = WEBSOCKET(self);
  WEBSOCKET_DEBUG_PRINTF("Destroying the wsi\n");
  WebsocketUserCallback(ws, kWebsocketCallbackTypeClose, "", 0);
  return 0;
}

int WebsocketClientServiceCallback(
    struct lws* wsi,
    enum lws_callback_reasons reason,
    void* user,
    void* in,
    size_t len
) {
  UNUSED(user);
  WebsocketClient* const ws = lws_context_user(lws_get_context(wsi));

  WEBSOCKET_DEBUG_PRINTF("%s(), reason = %s\n", __func__, WebsocketLwsReasonToString(reason));

  switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED: {
      return WebsocketClientOnClientEstablished(ws);
    }

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
      return WebsocketClientOnClientConnectionError(ws, (const char*)in, len);
    }

    case LWS_CALLBACK_CLIENT_WRITEABLE: {
      return WebsocketOnWritable(WEBSOCKET_OBJECT(ws));
    }

    case LWS_CALLBACK_CLIENT_RECEIVE: {
      return WebsocketOnReceive(WEBSOCKET_OBJECT(ws), in, len);
    }

    case LWS_CALLBACK_CLIENT_CLOSED: {
      return WebsocketClientOnWsiDestroy(ws);
    }

    default: {
      return 0;
    }
  }
}
