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
 * @brief Websocket implementation declarations
 */

#ifndef SRC_SHIP_WEBSOCKET_WEBSOCKET_INTERNAL_H_
#define SRC_SHIP_WEBSOCKET_WEBSOCKET_INTERNAL_H_

#include <libwebsockets.h>

#include "src/common/api/eebus_queue_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_mutex/eebus_mutex.h"
#include "src/ship/api/tls_certificate_interface.h"
#include "src/ship/api/websocket_interface.h"

/**
 * @brief Upper bound in bytes for the total reassembled WebSocket input message size.
 *
 * Fragments are accumulated until the final one arrives; if the running total
 * exceeds this cap the connection is closed and the partial data is discarded.
 * The default limits memory consumption from unexpectedly large or malformed
 * messages. Products/builds with different constraints can override this value
 * at compile time.
 */
#ifndef EEBUS_WEBSOCKET_MAX_INPUT_MSG_SIZE
#define EEBUS_WEBSOCKET_MAX_INPUT_MSG_SIZE (64 * 1024)
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define SHIP_WEBSOCKET_SUB_PROTOCOL "ship"

#ifdef __freertos__
static const lws_usec_t kWebsocketStaggerDelay = 200 * LWS_US_PER_MS;
#else
static const lws_usec_t kWebsocketStaggerDelay = 50 * LWS_US_PER_MS;
#endif  // __freertos__

typedef struct Websocket Websocket;

struct Websocket {
  /** Implements the Websocket Interface */
  WebsocketObject obj;

  struct lws* wsi;
  struct lws_context* lws_ctx;

  WebsocketCallback callback;
  void* context;

  bool is_closed;
  int32_t close_error;

  EebusQueueObject* wr_queue;
  EebusMutexObject* wr_mutex;

  uint8_t* buf_tmp;
  size_t buf_tmp_size;
  lws_sorted_usec_list_t sul_stagger;
};

#define WEBSOCKET(obj) ((Websocket*)(obj))

EebusError WebsocketConstruct(Websocket* self, WebsocketCallback cb, void* ctx);

void WebsocketDestruct(WebsocketObject* self);
int32_t WebsocketWrite(WebsocketObject* self, const uint8_t* msg, size_t msg_size);
void WebsocketClose(WebsocketObject* self, int32_t close_code, const char* reason);
bool WebsocketIsClosed(const WebsocketObject* self);
int32_t WebsocketGetCloseError(const WebsocketObject* self);
void WebsocketScheduleWrite(WebsocketObject* self);
void WebsocketStaggerCallback(lws_sorted_usec_list_t* sul);
void WebsocketUserCallback(const Websocket* self, WebsocketCallbackType type, const void* in, size_t size);

int WebsocketOnWritable(WebsocketObject* self);
int WebsocketOnReceive(WebsocketObject* self, void* in, size_t len);
int WebsocketOnClose(WebsocketObject* self);

const char* WebsocketGetSkiWithWsi(struct lws* wsi);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_WEBSOCKET_WEBSOCKET_INTERNAL_H_
