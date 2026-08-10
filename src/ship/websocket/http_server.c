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
 * @brief Http Server implementation
 * Additional useful resources:
 * # https://libwebsockets.org/git/libwebsockets/tree/READMEs/README.lws_sul.md
 * # https://github.com/warmcat/libwebsockets/issues/2414
 */

#include <libwebsockets.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/string_util.h"
#include "src/ship/api/http_server_interface.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_creator_interface.h"
#include "src/ship/websocket/websocket_creator.h"
#include "src/ship/websocket/websocket_debug.h"
#include "src/ship/websocket/websocket_internal.h"
#include "src/ship/websocket/websocket_server_creator.h"

/** Set HTTP_SERVER_DEBUG 1 to enable debug prints */
#ifndef HTTP_SERVER_DEBUG
#define HTTP_SERVER_DEBUG 0
#endif

/** Http server debug printf(), enabled whith HTTP_SERVER_DEBUG = 1 */
#if HTTP_SERVER_DEBUG
#define HTTP_SERVER_DEBUG_PRINTF(fmt, ...) DebugPrintf(fmt, ##__VA_ARGS__)
#else
#define HTTP_SERVER_DEBUG_PRINTF(fmt, ...)
#endif  // HTTP_SERVER_DEBUG

/**
 * @brief Workaround for mbedtls verify client cert post-handshake
 * until https://github.com/warmcat/libwebsockets/pull/3460 is merged
 * and included into release
 */
#ifndef LWS_SERVER_OPTION_MBEDTLS_VERIFY_CLIENT_CERT_POST_HANDSHAKE
#define LWS_SERVER_OPTION_MBEDTLS_VERIFY_CLIENT_CERT_POST_HANDSHAKE 0
#endif  // LWS_SERVER_OPTION_MBEDTLS_VERIFY_CLIENT_CERT_POST_HANDSHAKE

typedef struct {
  struct lws*      wsi;
  WebsocketObject* ws;
} WsiWsEntry;

#define HTTP_SERVER_MAX_CONNECTIONS 10

typedef struct HttpServer HttpServer;

struct HttpServer {
  /** Implements the Http Server Interface */
  HttpServerObject obj;

  bool cancel;
  EebusThreadObject* thread;
  EebusMutexObject* mutex;

  struct lws_context* lws_ctx;
  WebsocketServerCallbackType conn_establish_cb;
  void* conn_establish_ctx;
  WsiWsEntry wsi_ws_table[HTTP_SERVER_MAX_CONNECTIONS];
  size_t     wsi_ws_count;

  int port;
  const TlsCertificateObject* tls_cert;
  struct lws_protocols protocols[2];
  struct lws_context_creation_info info;
  lws_sorted_usec_list_t sul_stagger;
};

#define HTTP_SERVER(obj) ((HttpServer*)(obj))

static void Destruct(HttpServerObject* self);
static EebusError Start(HttpServerObject* self);
static void Stop(HttpServerObject* self);

static const HttpServerInterface http_server_methods = {
    .destruct = Destruct,
    .start    = Start,
    .stop     = Stop,
};

static void HttpServerConstruct(
    HttpServer* self,
    int port,
    const TlsCertificateObject* tls_cert,
    WebsocketServerCallbackType conn_establish_cb,
    void* conn_establish_ctx
);

static void HttpServerStaggerCallback(lws_sorted_usec_list_t* sul);
static struct lws_context* HttpServerContextCreate(HttpServer* self);
static void* HttpServerConnectionLoop(void* self);
static EebusError HttpServerTryStart(HttpServer* self);
static int HttpServerOnClientConnect(HttpServer* self, struct lws* wsi);
static int HttpServerOnReceive(HttpServer* self, struct lws* wsi, void* in, size_t len);
static int HttpServerOnWriteable(HttpServer* self, struct lws* wsi);
static int HttpServerOnConnectionClose(HttpServer* self, struct lws* wsi);
static int
HttpServerServiceCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

void HttpServerConstruct(
    HttpServer* self,
    int port,
    const TlsCertificateObject* tls_cert,
    WebsocketServerCallbackType conn_establish_cb,
    void* conn_establish_ctx
) {
  // Override "virtual functions table"
  HTTP_SERVER_INTERFACE(self) = &http_server_methods;

  memset(&self->info, 0, sizeof(self->info));
  memset(&self->sul_stagger, 0, sizeof(self->sul_stagger));

  self->protocols[0]
      = (struct lws_protocols){SHIP_WEBSOCKET_SUB_PROTOCOL, HttpServerServiceCallback, 0, 4096, 0, self, 0};
  self->protocols[1] = (struct lws_protocols)LWS_PROTOCOL_LIST_TERM;

  self->tls_cert = tls_cert;
  self->thread   = NULL;
  self->cancel   = false;

  self->conn_establish_cb  = conn_establish_cb;
  self->conn_establish_ctx = conn_establish_ctx;

  self->port         = port;
  self->wsi_ws_count = 0;

  self->lws_ctx = NULL;

  self->mutex = EebusMutexCreateRecursive();
}

HttpServerObject* HttpServerCreate(
    int port,
    const TlsCertificateObject* tls_cert,
    WebsocketServerCallbackType conn_establish_cb,
    void* conn_establish_ctx
) {
  HttpServer* const http_server = (HttpServer*)EEBUS_MALLOC(sizeof(HttpServer));

  HttpServerConstruct(http_server, port, tls_cert, conn_establish_cb, conn_establish_ctx);

  return HTTP_SERVER_OBJECT(http_server);
}

void Destruct(HttpServerObject* self) {
  HttpServer* const srv = HTTP_SERVER(self);

  if (srv->lws_ctx != NULL) {
    lws_context_destroy(srv->lws_ctx);
    srv->lws_ctx = NULL;
  }

  if (srv->mutex != NULL) {
    EebusMutexDelete(srv->mutex);
    srv->mutex = NULL;
  }
}

void HttpServerStaggerCallback(lws_sorted_usec_list_t* sul) {
  HttpServer* const srv = lws_container_of(sul, HttpServer, sul_stagger);

  EEBUS_MUTEX_LOCK(srv->mutex);
  for (size_t i = 0; i < srv->wsi_ws_count; ++i) {
    if (!WEBSOCKET_IS_CLOSED(srv->wsi_ws_table[i].ws)) {
      WEBSOCKET_SCHEDULE_WRITE(srv->wsi_ws_table[i].ws);
    }
  }
  EEBUS_MUTEX_UNLOCK(srv->mutex);

  lws_sul_schedule(srv->lws_ctx, 0, &srv->sul_stagger, HttpServerStaggerCallback, kWebsocketStaggerDelay);
}

struct lws_context* HttpServerContextCreate(HttpServer* self) {
  const struct lws_context_creation_info lws_ctx_creation_info = (struct lws_context_creation_info){
      .port      = self->port,
      .protocols = self->protocols,
      .gid       = (gid_t)-1,
      .uid       = (uid_t)-1,

      .options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_SSL_ECDH
                 | LWS_SERVER_OPTION_PEER_CERT_NOT_REQUIRED | LWS_SERVER_OPTION_H2_JUST_FIX_WINDOW_UPDATE_OVERFLOW
                 | LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT
                 | LWS_SERVER_OPTION_MBEDTLS_VERIFY_CLIENT_CERT_POST_HANDSHAKE,

      .ecdh_curve = "prime256v1",
      .ssl_cipher_list
      = "ECDHE-ECDSA-AES128-GCM-SHA256:"
        "ECDHE-ECDSA-AES128-CCM8:"
        "ECDHE-ECDSA-AES128-SHA256",

      .server_ssl_cert_mem            = TLS_CERTIFICATE_GET_CERTIFICATE(self->tls_cert),
      .server_ssl_cert_mem_len        = (unsigned int)TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(self->tls_cert),
      .server_ssl_private_key_mem     = TLS_CERTIFICATE_GET_PRIVATE_KEY(self->tls_cert),
      .server_ssl_private_key_mem_len = (unsigned int)TLS_CERTIFICATE_GET_PRIVATE_KEY_SIZE(self->tls_cert),

      .user = self,
  };

  if (WEBSOCKET_DEBUG == 2) {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_DEBUG;
    lws_set_log_level(logs, NULL);
  }

  return lws_create_context(&lws_ctx_creation_info);
}

void* HttpServerConnectionLoop(void* self) {
  HttpServer* const srv = (HttpServer*)self;

  int err = 0;
  lws_sul_schedule(srv->lws_ctx, 0, &srv->sul_stagger, HttpServerStaggerCallback, kWebsocketStaggerDelay);

  do {
    err = lws_service(srv->lws_ctx, 100);
  } while ((err >= 0) && (!srv->cancel));

  return NULL;
}

EebusError HttpServerTryStart(HttpServer* self) {
  self->lws_ctx = HttpServerContextCreate(self);
  if (self->lws_ctx == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), creating libwebsocket context failed\n", __func__);
    return kEebusErrorInit;
  }

  self->thread = EebusThreadCreate(HttpServerConnectionLoop, self, 16 * 1024);
  if (self->thread == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), start webserver failed\n", __func__);
    return kEebusErrorThread;
  }

  return kEebusErrorOk;
}

EebusError Start(HttpServerObject* self) {
  HttpServer* const srv = HTTP_SERVER(self);

  EebusError ret = HttpServerTryStart(srv);

  return ret;
}

void Stop(HttpServerObject* self) {
  HttpServer* const srv = HTTP_SERVER(self);
  HTTP_SERVER_DEBUG_PRINTF("Stopping HTTP server\n");
  srv->cancel = true;

  if (srv->thread != NULL) {
    EEBUS_THREAD_JOIN(srv->thread);
    EebusThreadDelete(srv->thread);
    srv->thread = NULL;
  }

  HTTP_SERVER_DEBUG_PRINTF("HTTP server stopped\n");
}

void HttpServerUnbindWsi(HttpServerObject* self, struct lws* wsi) {
  HttpServer* const srv = HTTP_SERVER(self);
  if (wsi == NULL) {
    return;
  }

  EEBUS_MUTEX_LOCK(srv->mutex);
  for (size_t i = 0; i < srv->wsi_ws_count; ++i) {
    if (srv->wsi_ws_table[i].wsi == wsi) {
      /* Swap-with-last-and-pop to remove without shifting */
      srv->wsi_ws_table[i] = srv->wsi_ws_table[--srv->wsi_ws_count];
      break;
    }
  }
  lws_set_wsi_user(wsi, NULL);
  EEBUS_MUTEX_UNLOCK(srv->mutex);
}

// LWS Handlers
int HttpServerOnClientConnect(HttpServer* self, struct lws* wsi) {
  const char* ski = WebsocketGetSkiWithWsi(wsi);
  if (ski == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), WebsocketGetSkiWithWsi() failed\n", __func__);
    return -1;
  }

  if (self->conn_establish_cb == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), conn_establish_cb() is NULL\n", __func__);
    StringDelete((char*)ski);
    return -1;
  }

  WebsocketCreatorObject* websocket_creator = WebsocketServerCreatorCreate(HTTP_SERVER_OBJECT(self), wsi);

  const int ret = self->conn_establish_cb(ski, websocket_creator, self->conn_establish_ctx);
  WebsocketCreatorDelete(websocket_creator);
  StringDelete((char*)ski);
  if (ret != 0) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), conn_establish_cb() failed: %d\n", __func__, ret);
    return -1;
  }

  WebsocketObject* const ws = (WebsocketObject*)lws_wsi_user(wsi);
  if (ws == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), websocket object is NULL\n", __func__);
    return -1;
  }

  EEBUS_MUTEX_LOCK(self->mutex);
  if (self->wsi_ws_count < HTTP_SERVER_MAX_CONNECTIONS) {
    self->wsi_ws_table[self->wsi_ws_count].wsi = wsi;
    self->wsi_ws_table[self->wsi_ws_count].ws  = ws;
    self->wsi_ws_count++;
  }
  EEBUS_MUTEX_UNLOCK(self->mutex);

  lws_sul_schedule(self->lws_ctx, 0, &self->sul_stagger, HttpServerStaggerCallback, kWebsocketStaggerDelay);

  return 0;
}

int HttpServerOnReceive(HttpServer* self, struct lws* wsi, void* in, size_t len) {
  UNUSED(self);

  WebsocketObject* ws = (WebsocketObject*)lws_wsi_user(wsi);
  if (ws == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), websocket object is NULL\n", __func__);
    return -1;
  }

  const int ret = WebsocketOnReceive(ws, in, len);
  if (ret != 0) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), WebsocketOnReceive() failed: %d\n", __func__, ret);
  }

  return ret;
}

int HttpServerOnWriteable(HttpServer* self, struct lws* wsi) {
  UNUSED(self);
  WebsocketObject* ws = (WebsocketObject*)lws_wsi_user(wsi);
  if (ws == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), websocket object is NULL\n", __func__);
    return -1;
  }

  const int ret = WebsocketOnWritable(ws);
  if (ret != 0) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), WebsocketOnWritable() failed: %d\n", __func__, ret);
  }

  return ret;
}

int HttpServerOnConnectionClose(HttpServer* self, struct lws* wsi) {
  WebsocketObject* ws = (WebsocketObject*)lws_wsi_user(wsi);
  if (ws == NULL) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), websocket object is NULL\n", __func__);
    return -1;
  }

  WEBSOCKET_CLOSE(ws, 0, "");
  WebsocketOnClose(ws);
  lws_cancel_service(self->lws_ctx);
  return 0;
}

int HttpServerServiceCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
  UNUSED(user);

  HTTP_SERVER_DEBUG_PRINTF("%s(), reason = %s\n", __func__, WebsocketLwsReasonToString(reason));
  HttpServer* const srv = lws_context_user(lws_get_context(wsi));
  int ret               = 0;

  switch (reason) {
    case LWS_CALLBACK_ESTABLISHED: ret = HttpServerOnClientConnect(srv, wsi); break;

    case LWS_CALLBACK_RECEIVE: ret = HttpServerOnReceive(srv, wsi, in, len); break;

    case LWS_CALLBACK_SERVER_WRITEABLE: ret = HttpServerOnWriteable(srv, wsi); break;

    case LWS_CALLBACK_CLOSED: ret = HttpServerOnConnectionClose(srv, wsi); break;

    case LWS_CALLBACK_EVENT_WAIT_CANCELLED: ret = 0; break;

    default: break;
  }

  if (ret != 0) {
    HTTP_SERVER_DEBUG_PRINTF("%s(), ret = %d\n", __func__, ret);
  }

  return ret;
}
