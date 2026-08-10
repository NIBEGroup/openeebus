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
 * @brief Ordered list of entity addresses with deep-copy ownership
 */

#ifndef SRC_COMMON_ENTITY_ADDRESS_LIST_H_
#define SRC_COMMON_ENTITY_ADDRESS_LIST_H_

#include <stddef.h>

#include "src/common/eebus_errors.h"
#include "src/common/vector.h"
#include "src/spine/model/entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EntityAddressList EntityAddressList;

struct EntityAddressList {
  Vector entries;
};

void                     EntityAddressListInit(EntityAddressList* self);
void                     EntityAddressListRelease(EntityAddressList* self);
EebusError               EntityAddressListAdd(EntityAddressList* self, const EntityAddressType* addr);
void                     EntityAddressListRemove(EntityAddressList* self, const EntityAddressType* addr);
size_t                   EntityAddressListGetSize(const EntityAddressList* self);
const EntityAddressType* EntityAddressListGet(const EntityAddressList* self, size_t idx);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_ENTITY_ADDRESS_LIST_H_
