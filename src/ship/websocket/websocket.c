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
 * @brief Websocket implementation
 * @note Messages with size > 4K requre specific buffering mechanism.
 * Changing the lws_protocols configuration does not help for some reason.
 * Some explanation and work around are available at:
 * https://github.com/warmcat/libwebsockets/issues/1103
 * Additional useful resources:
 * # https://libwebsockets.org/git/libwebsockets/tree/READMEs/README.lws_sul.md
 */

#include "src/ship/websocket/websocket_internal.h"

#include <libwebsockets.h>

#include "src/common/debug.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_queue/eebus_queue.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/common/string_util.h"
#include "src/common/struct_util.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_interface.h"
#include "src/ship/tls_certificate/tls_certificate.h"
#include "src/ship/websocket/websocket_debug.h"

static const size_t kWriteQueueSize  = 50;
static const size_t kMaxInputMsgSize = EEBUS_WEBSOCKET_MAX_INPUT_MSG_SIZE;

typedef struct WriteMessage WriteMessage;

struct WriteMessage {
  uint8_t* data;
  size_t data_size;
};

static void WebsocketWrQueueMsgRelease(void* msg);

EebusError WebsocketConstruct(Websocket* self, WebsocketCallback cb, void* ctx) {
  self->callback = cb;
  self->context  = ctx;

  self->is_closed   = false;
  self->close_error = 0;

  self->wr_queue = NULL;
  self->wr_mutex = NULL;

  memset(&self->sul_stagger, 0, sizeof(self->sul_stagger));

  self->lws_ctx = NULL;

  self->wsi = NULL;

  self->buf_tmp      = NULL;
  self->buf_tmp_size = 0;

  self->wr_queue = EebusQueueCreate(kWriteQueueSize, sizeof(WriteMessage), WebsocketWrQueueMsgRelease);
  if (self->wr_queue == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), initialising write queue failed\n", __func__);
    return kEebusErrorMemory;
  }

  self->wr_mutex = EebusMutexCreate();
  if (self->wr_mutex == NULL) {
    WEBSOCKET_DEBUG_PRINTF("%s(), creating write mutex failed\n", __func__);
    return kEebusErrorInit;
  }

  return kEebusErrorOk;
}

void WebsocketWrQueueMsgRelease(void* msg) {
  WriteMessage* const wr_msg = (WriteMessage*)msg;
  if (wr_msg != NULL) {
    EEBUS_FREE(wr_msg->data);
    wr_msg->data      = NULL;
    wr_msg->data_size = 0;
  }
}

void WebsocketDestruct(WebsocketObject* self) {
  Websocket* const ws = WEBSOCKET(self);

  ws->wsi       = NULL;
  ws->is_closed = true;

  if (ws->lws_ctx != NULL) {
    lws_context_destroy(ws->lws_ctx);
    ws->lws_ctx = NULL;
  }

  EebusMutexDelete(ws->wr_mutex);
  ws->wr_mutex = NULL;

  EebusQueueDelete(ws->wr_queue);
  ws->wr_queue = NULL;

  if (ws->buf_tmp != NULL) {
    EEBUS_FREE(ws->buf_tmp);
    ws->buf_tmp = NULL;
  }
}

void WebsocketUserCallback(const Websocket* self, WebsocketCallbackType type, const void* in, size_t size) {
  if (self->callback != NULL) {
    self->callback(type, in, size, self->context);
  }
}

int32_t WebsocketTryWrite(Websocket* self, const uint8_t* msg, size_t msg_size) {
  if (self->is_closed) {
    return 0;
  }

  const size_t data_size = msg_size + LWS_PRE;
  WriteMessage wr_msg    = {.data = (uint8_t*)EEBUS_MALLOC(data_size), .data_size = data_size};
  if (wr_msg.data == NULL) {
    return 0;
  }

  memset(wr_msg.data, 0, LWS_PRE);
  memcpy(&wr_msg.data[LWS_PRE], msg, msg_size);

  const EebusError ret = EEBUS_QUEUE_SEND(self->wr_queue, &wr_msg, 0);
  if (ret != kEebusErrorOk) {
    WEBSOCKET_DEBUG_PRINTF("%s(), error sending message to queue\n", __func__);
    EEBUS_FREE(wr_msg.data);
    wr_msg.data = NULL;
    return 0;
  }

  return (int32_t)msg_size;
}

int32_t WebsocketWrite(WebsocketObject* self, const uint8_t* msg, size_t msg_size) {
  Websocket* const ws = WEBSOCKET(self);

  EEBUS_MUTEX_LOCK(ws->wr_mutex);
  const int32_t ret = WebsocketTryWrite(ws, msg, msg_size);
  EEBUS_MUTEX_UNLOCK(ws->wr_mutex);

  return ret;
}

void WebsocketClose(WebsocketObject* self, int32_t close_code, const char* reason) {
  UNUSED(reason);
  Websocket* const ws = WEBSOCKET(self);

  EEBUS_MUTEX_LOCK(ws->wr_mutex);
  ws->close_error = close_code;
  ws->wsi         = NULL;
  ws->is_closed   = true;
  EEBUS_MUTEX_UNLOCK(ws->wr_mutex);
}

bool WebsocketIsClosed(const WebsocketObject* self) {
  return WEBSOCKET(self)->is_closed;
}

int32_t WebsocketGetCloseError(const WebsocketObject* self) {
  return WEBSOCKET(self)->close_error;
}

void WebsocketScheduleWrite(WebsocketObject* self) {
  Websocket* const ws = WEBSOCKET(self);

  if (!EEBUS_QUEUE_IS_EMPTY(ws->wr_queue)) {
    lws_callback_on_writable(ws->wsi);
  }
}

void WebsocketStaggerCallback(lws_sorted_usec_list_t* sul) {
  Websocket* const ws = lws_container_of(sul, Websocket, sul_stagger);

  if (ws != NULL) {
    if (!WEBSOCKET_IS_CLOSED(WEBSOCKET_OBJECT(ws))) {
      WEBSOCKET_SCHEDULE_WRITE(WEBSOCKET_OBJECT(ws));
    }

    lws_sul_schedule(ws->lws_ctx, 0, &ws->sul_stagger, WebsocketStaggerCallback, kWebsocketStaggerDelay);
  }
}

// LWS event handlers

int WebsocketOnWritable(WebsocketObject* self) {
  Websocket* const ws = (Websocket*)WEBSOCKET(self);

  // Snapshot wsi once: WebsocketClose() can set ws->wsi = NULL from another thread.
  // If wsi is non-NULL here the LWS wsi object is still alive — lws_context_destroy
  // runs only after the LWS service thread exits.
  struct lws* const wsi = ws->wsi;
  if (wsi == NULL) {
    return 0;
  }

  WriteMessage wr_msg = {0};

  const EebusError ret = EEBUS_QUEUE_RECEIVE(ws->wr_queue, &wr_msg, 0);
  if (ret != kEebusErrorOk) {
    WEBSOCKET_DEBUG_PRINTF("%s(), error receiving the message from queue\n", __func__);
    return 0;
  }

  const size_t sz = wr_msg.data_size - LWS_PRE;

  WEBSOCKET_DEBUG_HEXDUMP(&wr_msg.data[LWS_PRE], sz);
  const int n = lws_write(wsi, &wr_msg.data[LWS_PRE], sz, LWS_WRITE_BINARY);

  EEBUS_FREE(wr_msg.data);
  if ((n < 0) || ((size_t)n != sz)) {
    WEBSOCKET_DEBUG_PRINTF("sending message failed: %d < %d\n", n, sz);
    return -1;
  }

  lws_callback_on_writable(wsi);
  return 0;
}

void BufTmpAppend(Websocket* self, const uint8_t* data, size_t data_size) {
  if ((data == NULL) || (data_size == 0)) {
    return;
  }

  if (data_size > SIZE_MAX - self->buf_tmp_size) {
    return;  // size_t overflow guard
  }

  uint8_t* const new_buf = EEBUS_MALLOC(self->buf_tmp_size + data_size);
  if (new_buf == NULL) {
    return;
  }

  if (self->buf_tmp != NULL) {
    memcpy(new_buf, self->buf_tmp, self->buf_tmp_size);
  }

  memcpy(new_buf + self->buf_tmp_size, data, data_size);
  EEBUS_FREE(self->buf_tmp);
  self->buf_tmp = new_buf;
  self->buf_tmp_size += data_size;
}

void BufTmpRelease(Websocket* self) {
  EEBUS_FREE(self->buf_tmp);
  self->buf_tmp      = NULL;
  self->buf_tmp_size = 0;
}

int WebsocketOnReceive(WebsocketObject* self, void* in, size_t len) {
  Websocket* const ws = (Websocket*)WEBSOCKET(self);
  if (ws->wsi == NULL) {
    return -1;
  }

  if ((len > kMaxInputMsgSize) || (ws->buf_tmp_size > kMaxInputMsgSize - len)) {
    WEBSOCKET_DEBUG_PRINTF("%s(), input message exceeds maximum size (%zu), closing\n", __func__, kMaxInputMsgSize);
    BufTmpRelease(ws);
    return -1;
  }

  WEBSOCKET_DEBUG_HEXDUMP(in, len);

  if (lws_is_final_fragment(ws->wsi) && !lws_remaining_packet_payload(ws->wsi)) {
    if (ws->buf_tmp != NULL) {
      BufTmpAppend(ws, (const uint8_t*)in, len);
      WebsocketUserCallback(ws, kWebsocketCallbackTypeRead, ws->buf_tmp, ws->buf_tmp_size);
      BufTmpRelease(ws);
    } else {
      WebsocketUserCallback(ws, kWebsocketCallbackTypeRead, in, len);
    }
  } else {
    BufTmpAppend(ws, (const uint8_t*)in, len);
  }
  return 0;
}

int WebsocketOnClose(WebsocketObject* self) {
  Websocket* const ws = (Websocket*)WEBSOCKET(self);
  WEBSOCKET_DEBUG_PRINTF("%s(), websocket closed\n", __func__);
  WebsocketUserCallback(ws, kWebsocketCallbackTypeClose, NULL, 0);
  return 0;
}

const char* WebsocketGetSkiWithWsi(struct lws* wsi) {
  if (wsi == NULL) {
    return NULL;
  }

  static const size_t kMaxCertSize = 2048;

  char* const buf = (char*)EEBUS_MALLOC(kMaxCertSize);

  union lws_tls_cert_info_results* results = (union lws_tls_cert_info_results*)buf;
  if (buf == NULL) {
    return NULL;
  }

  const char* ski = NULL;
  const size_t n  = kMaxCertSize - sizeof(*results) + sizeof(results->ns.name);
  int err         = lws_tls_peer_cert_info(wsi, LWS_TLS_CERT_INFO_DER_RAW, results, n);
  if ((err == 0) && (results->ns.len != 0)) {
    ski = TlsCertificateCalcPublicKeySki((const uint8_t*)results->ns.name, results->ns.len);
    if (ski == NULL) {
      WEBSOCKET_DEBUG_PRINTF("%s(), TlsCertificateCalcPublicKeySki() failed\n", __func__);
    }
  } else {
    WEBSOCKET_DEBUG_PRINTF("%s(), lws_tls_peer_cert_info() failed: %d\n", __func__, err);
  }

  EEBUS_FREE(buf);
  return ski;
}
