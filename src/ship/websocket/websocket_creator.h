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
 * @brief Websocket Creator implementation declarations
 */

#ifndef SRC_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_H_
#define SRC_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_H_

#include <stddef.h>
#include "src/common/eebus_malloc.h"

#include "src/ship/api/websocket_creator_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

WebsocketCreatorObject* WebsocketCreatorCreate(void);

static inline void WebsocketCreatorDelete(WebsocketCreatorObject* websocket_creator) {
    if (websocket_creator != NULL) {
        WEBSOCKET_CREATOR_DESTRUCT(websocket_creator);
        EEBUS_FREE(websocket_creator);
    }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_WEBSOCKET_WEBSOCKET_CREATOR_H_
