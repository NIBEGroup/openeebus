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

#include "src/common/entity_address_list.h"

#include <stddef.h>

static void EntryDelete(void* entry) { EntityAddressDelete((EntityAddressType*)entry); }

void EntityAddressListInit(EntityAddressList* self) {
  VectorConstructWithDeallocator(&self->entries, EntryDelete);
}

void EntityAddressListRelease(EntityAddressList* self) {
  VectorFreeElements(&self->entries);
  VectorDestruct(&self->entries);
}

EebusError EntityAddressListAdd(EntityAddressList* self, const EntityAddressType* addr) {
  if (addr == NULL) {
    return kEebusErrorInputArgument;
  }
  for (size_t i = 0; i < VectorGetSize(&self->entries); ++i) {
    if (EntityAddressCompare((EntityAddressType*)VectorGetElement(&self->entries, i), addr)) {
      return kEebusErrorOk;
    }
  }
  EntityAddressType* copy = EntityAddressCopy(addr);
  if (copy == NULL) {
    return kEebusErrorMemoryAllocate;
  }
  VectorPushBack(&self->entries, copy);
  return kEebusErrorOk;
}

void EntityAddressListRemove(EntityAddressList* self, const EntityAddressType* addr) {
  if (addr == NULL) {
    return;
  }
  for (size_t i = 0; i < VectorGetSize(&self->entries); ++i) {
    EntityAddressType* entry = (EntityAddressType*)VectorGetElement(&self->entries, i);
    if (EntityAddressCompare(entry, addr)) {
      VectorRemove(&self->entries, entry);
      EntityAddressDelete(entry);
      return;
    }
  }
}

size_t EntityAddressListGetSize(const EntityAddressList* self) {
  return VectorGetSize(&self->entries);
}

const EntityAddressType* EntityAddressListGet(const EntityAddressList* self, size_t idx) {
  return (const EntityAddressType*)VectorGetElement(&self->entries, idx);
}
