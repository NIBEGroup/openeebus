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
 * @brief Smart Energy Management PS implementation
 */

#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_data/eebus_data_sequence.h"
#include "src/common/eebus_data/eebus_data_util.h"
#include "src/spine/model/smart_energy_management_ps_cfg.h"
#include "src/spine/model/smart_energy_management_ps_types.h"

static EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr);

static bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
);

static EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);

static void DeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);

const EebusDataInterface eebus_data_smart_energy_management_ps_methods = {
    .create_empty          = EebusDataBaseCreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = EebusDataSequenceFromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = EebusDataSequenceToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = CopyMatching,
    .compare               = EebusDataSequenceCompare,
    .is_null               = EebusDataSequenceIsNull,
    .is_empty              = EebusDataSequenceIsEmpty,
    .has_identifiers       = EebusDataSequenceHasIdentifiers,
    .selectors_match       = SelectorsMatch,
    .identifiers_match     = EebusDataSequenceIdentifiersMatch,
    .read_elements         = EebusDataSequenceReadElements,
    .write                 = EebusDataSequenceWrite,
    .write_elements        = EebusDataSequenceWriteElements,
    .write_partial         = WritePartial,
    .delete_elements       = EebusDataSequenceDeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = EebusDataSequenceDelete,
};

EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr) {
  const EebusDataCfg* schedule_cfg = EebusDataSequenceGetFieldCfg(cfg, "nodeScheduleInformation");
  if (schedule_cfg == NULL) {
    return kEebusErrorInit;
  }

  const EebusDataCfg* alternatives_cfg = EebusDataSequenceGetFieldCfg(cfg, "alternatives");
  if (alternatives_cfg == NULL) {
    return kEebusErrorInit;
  }

  const void** const buf = (const void**)((const uint8_t*)base_addr + cfg->offset);
  if (*buf == NULL) {
    // Nothing to copied
    return kEebusErrorInputArgument;
  }

  void** dst_buf = (void**)((uint8_t*)dst_base_addr + cfg->offset);
  if (EEBUS_DATA_IS_NULL(cfg, dst_base_addr)) {
    *dst_buf = EEBUS_DATA_CREATE_EMPTY(cfg, dst_base_addr);
    if (*dst_buf == NULL) {
      return kEebusErrorMemoryAllocate;
    }
  }

  if (EEBUS_DATA_COPY_MATCHING(schedule_cfg, *buf, *dst_buf, data_to_match_base_addr) != kEebusErrorOk) {
    return kEebusErrorOk;
  }

  return EEBUS_DATA_COPY_MATCHING(alternatives_cfg, *buf, *dst_buf, data_to_match_base_addr);
}

bool SelectorsMatch(
    const EebusDataCfg* cfg,
    const void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr
) {
  UNUSED(cfg);
  UNUSED(base_addr);
  UNUSED(selectors_cfg);
  UNUSED(selectors_base_addr);

  EEBUS_ASSERT_ALWAYS();
  return false;
}

EebusError SmartEnergyManagementPsAlternativesWritePartial(
    const EebusDataCfg* cfg,
    SmartEnergyManagementPsDataType* dst,
    const SmartEnergyManagementPsDataType* src,
    const EebusDataCfg* selectors_cfg,
    const SmartEnergyManagementPsDataSelectorsType* selectors_base_addr
) {
  if (!EEBUS_DATA_IS_NULL(selectors_cfg, selectors_base_addr)) {
    return kEebusErrorNotImplemented;
  } else {
    return EEBUS_DATA_WRITE_PARTIAL(cfg, (void*)dst, (const void*)src, NULL, NULL, NULL);
  }
}

EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
  const EebusDataCfg* schedule_cfg = EebusDataSequenceGetFieldCfg(cfg, "nodeScheduleInformation");
  if (schedule_cfg == NULL) {
    return kEebusErrorInit;
  }

  const EebusDataCfg* alternatives_cfg = EebusDataSequenceGetFieldCfg(cfg, "alternatives");
  if (alternatives_cfg == NULL) {
    return kEebusErrorInit;
  }

  const void** const src_buf = (const void**)((const uint8_t*)src_base_addr + cfg->offset);
  if (*src_buf == NULL) {
    // Nothing to written
    return kEebusErrorInputArgument;
  }

  void** buf = (void**)((uint8_t*)base_addr + cfg->offset);
  if (EEBUS_DATA_IS_NULL(cfg, base_addr)) {
    *buf = EEBUS_DATA_CREATE_EMPTY(cfg, base_addr);
    if (*buf == NULL) {
      return kEebusErrorMemoryAllocate;
    }
  }

  // Handle the PowerSequenceNodeScheduleInformationDataType first
  if (EEBUS_DATA_WRITE_PARTIAL(schedule_cfg, *buf, *src_buf, selectors_cfg, selectors_base_addr, selectors_matcher)
      != kEebusErrorOk) {
    return kEebusErrorOk;
  }

  return SmartEnergyManagementPsAlternativesWritePartial(
      alternatives_cfg,
      *buf,
      *src_buf,
      selectors_cfg,
      selectors_base_addr
  );
}

void DeletePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
  UNUSED(cfg);
  UNUSED(base_addr);
  UNUSED(selectors_cfg);
  UNUSED(selectors_base_addr);
  UNUSED(selectors_matcher);
  UNUSED(elements_cfg);
  UNUSED(elements_base_addr);

  EEBUS_ASSERT_ALWAYS();
  return;
}
