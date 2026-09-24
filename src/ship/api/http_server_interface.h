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
 * @brief Http Server interface declarations
 */

#ifndef SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_
#define SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief On server connection established callback type definition
 */
typedef int (*WebsocketServerCallbackType)(const char* ski, WebsocketCreatorObject* wsc, void* ctx);

/**
 * @brief Http Server Interface
 * (Http Server "virtual functions table" declaration)
 */
typedef struct HttpServerInterface HttpServerInterface;

/**
 * @brief Http Server Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct HttpServerObject HttpServerObject;

/**
 * @brief HttpServer Interface Structure
 */
struct HttpServerInterface {
  void (*destruct)(HttpServerObject* self);
  EebusError (*start)(HttpServerObject* self);
  void (*stop)(HttpServerObject* self);

  /**
   * @brief SHA-256 fingerprint of the certificate of the connecting peer
   *
   * Valid only while a WebsocketServerCallbackType call is on the stack, which
   * is where a node decides whether to admit the peer. SHIP Pairing Service
   * verifies this in place of an SKI (SHIP Pairing Service TS 1.0.0,
   * section 10.2), and the callback receives only an SKI.
   *
   * Appended to the end of the table and may be NULL. Its signature carries the
   * fingerprint out rather than into the callback, so that a backend
   * implemented outside this repository neither has to change nor stops
   * compiling.
   *
   * @return Fingerprint as uppercase hexadecimal digits, owned by the server,
   *         or NULL if it is not known
   */
  const char* (*get_peer_fingerprint)(const HttpServerObject* self);
};

/**
 * @brief Http Server Object Structure
 */
struct HttpServerObject {
  const HttpServerInterface* interface_;
};

/**
 * @brief Http Server pointer typecast
 */
#define HTTP_SERVER_OBJECT(obj) ((HttpServerObject*)(obj))

/**
 * @brief Gets the fingerprint of the connecting peer's certificate
 *
 * A function rather than a macro, because the method is optional and has to be
 * checked for before it is called.
 *
 * @return Fingerprint, or NULL if the backend does not report one
 */
static inline const char* HttpServerGetPeerFingerprint(const HttpServerObject* obj) {
  if ((obj == NULL) || (obj->interface_->get_peer_fingerprint == NULL)) {
    return NULL;
  }

  return obj->interface_->get_peer_fingerprint(obj);
}

/**
 * @brief Http Server Interface class pointer typecast
 */
#define HTTP_SERVER_INTERFACE(obj) (HTTP_SERVER_OBJECT(obj)->interface_)

/**
 * @brief Http Server Destruct caller definition
 */
#define HTTP_SERVER_DESTRUCT(obj) (HTTP_SERVER_INTERFACE(obj)->destruct(obj))

/**
 * @brief Http Server Start caller definition
 */
#define HTTP_SERVER_START(obj) (HTTP_SERVER_INTERFACE(obj)->start(obj))

/**
 * @brief Http Server Stop caller definition
 */
#define HTTP_SERVER_STOP(obj) (HTTP_SERVER_INTERFACE(obj)->stop(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_HTTP_SERVER_INTERFACE_H_
