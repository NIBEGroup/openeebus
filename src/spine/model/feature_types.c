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
 * @brief SPINE Datagram Feature payload functions implementation
 */

#include "src/spine/model/feature_types.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "src/common/bool_ptr.h"
#include "src/common/eebus_data/eebus_data.h"
#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/common/string_util.h"
#include "src/spine/model/entity_types.h"
#include "src/spine/model/model.h"

FeatureAddressType* FeatureAddressCreate(const EntityAddressType* entity_addr, uint32_t feature_id) {
  FeatureAddressType* const addr = (FeatureAddressType*)EEBUS_MALLOC(sizeof(FeatureAddressType));
  if (addr == NULL) {
    return NULL;
  }

  addr->device = StringCopy(entity_addr->device);

  const uint32_t** entity_ids = (const uint32_t**)EEBUS_MALLOC(sizeof(uint32_t*) * entity_addr->entity_size);
  addr->entity                = entity_ids;
  for (size_t i = 0; i < entity_addr->entity_size; ++i) {
    entity_ids[i] = Uint32Create(*entity_addr->entity[i]);
  }

  addr->entity_size = entity_addr->entity_size;
  addr->feature     = Uint32Create(feature_id);

  return addr;
}

bool FeatureAddressIsValid(const FeatureAddressType* addr) {
  if (addr == NULL) {
    return false;
  }

  return (addr->feature != NULL) && (addr->entity != NULL) && (addr->entity_size != 0);
}

FeatureAddressType* FeatureAddressCopy(const FeatureAddressType* self) {
  EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FeatureAddressType, ModelGetFeatureAddressCfg());
  return (FeatureAddressType*)ModelDataCopy(&cfg, self);
}

bool FeatureAddressCompare(const FeatureAddressType* addr_a, const FeatureAddressType* addr_b) {
  EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FeatureAddressType, ModelGetFeatureAddressCfg());
  return EEBUS_DATA_COMPARE(&cfg, &addr_a, &cfg, &addr_b);
}

bool FeatureAddressMatchDevice(const FeatureAddressType* addr, const char* device) {
  if ((addr == NULL) || (device == NULL)) {
    return false;
  }

  return (addr->device != NULL) && (strcmp(addr->device, device) == 0);
}

void FeatureAddressPrint(const char* fmt, const FeatureAddressType* addr) {
  if (addr == NULL) {
    printf(fmt, "NULL");
    return;
  }

  const char* feature_addr_string = EEBUS_DATA_PRINT_UNFORMATTED(ModelGetFeatureAddressCfg(), &addr);
  if (feature_addr_string != NULL) {
    printf(fmt, feature_addr_string);
  } else {
    printf(fmt, "<error converting to string>");
  }

  StringDelete((char*)feature_addr_string);
}

void FeatureAddressDelete(FeatureAddressType* self) {
  if (self == NULL) {
    return;
  }
  EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FeatureAddressType, ModelGetFeatureAddressCfg());
  EEBUS_DATA_DELETE(&cfg, &self);
}

void FeatureAddressElementsDelete(FeatureAddressElementsType* self) {
  EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(FeatureAddressElementsType, ModelGetFeatureAddressElementsCfg());
  EEBUS_DATA_DELETE(&cfg, &self);
}
