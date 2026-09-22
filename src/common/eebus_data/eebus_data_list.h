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
 * @brief EEEBUS Data List
 */

#ifndef SRC_COMMON_EEBUS_DATA_EEBUS_DATA_LIST_H_
#define SRC_COMMON_EEBUS_DATA_EEBUS_DATA_LIST_H_

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/struct_util.h"

/*
 * Upper bound for list elements accepted by JSON parsing and list growth APIs.
 * The default cap limits memory usage and parsing work for unusually large
 * inputs, which helps reduce denial-of-service risk. Products/builds with
 * different resource constraints can override this value at compile time.
 */
#ifndef EEBUS_DATA_LIST_MAX_ELEMENTS_NUM
#define EEBUS_DATA_LIST_MAX_ELEMENTS_NUM 10000
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Data List Interface
 */
extern const EebusDataInterface eebus_data_list_methods;

/**
 * @brief EEBUS Data List Simple Interface
 */
extern const EebusDataInterface eebus_data_list_simple_methods;

/**
 * @brief EEBUS Data List type check
 */
#define EEBUS_DATA_IS_LIST(cfg) (EEBUS_DATA_INTERFACE(cfg) == &eebus_data_list_methods)

/**
 * @brief EEBUS Data List configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name
 * @param le_cfg Entry point of list element Data configuration
 */
#define EEBUS_DATA_LIST(ed_name, struct_name, struct_field, le_cfg)          \
  {                                                                          \
      .interface_  = &eebus_data_list_methods,                               \
      .name        = ed_name,                                                \
      .offset      = STRUCT_MEMBER_OFFSET(struct_name, struct_field),        \
      .size_offset = STRUCT_MEMBER_OFFSET(struct_name, struct_field##_size), \
      .metadata    = le_cfg,                                                 \
  }

/**
 * @brief EEBUS Data List Simple configuration
 * @param ed_name Expected Data record name
 * @param struct_name Structure name associated with Data record
 * @param struct_field Structure field name
 * @param le_cfg Entry point of list element Data configuration
 */
#define EEBUS_DATA_LIST_SIMPLE(ed_name, struct_name, struct_field, le_cfg)   \
  {                                                                          \
      .interface_  = &eebus_data_list_simple_methods,                        \
      .name        = ed_name,                                                \
      .offset      = STRUCT_MEMBER_OFFSET(struct_name, struct_field),        \
      .size_offset = STRUCT_MEMBER_OFFSET(struct_name, struct_field##_size), \
      .metadata    = le_cfg,                                                 \
  }

/**
 * @brief An element of EEBUS Data List configuration
 * @param ed_type EEBUS Data Configuration type name (tag, sequence, list, etc.)
 * @param ed_size Size of assigned data
 * @param ce_cfg EEBUS Data Element child Data configuration entry point
 */
#define EEBUS_DATA_LIST_ELEMENT(ed_type, ed_size, ed_cfg) \
  {                                                       \
      .interface_ = &eebus_data_##ed_type##_methods,      \
      .name       = "",                                   \
      .offset     = 0,                                    \
      .size       = ed_size,                              \
      .metadata   = ed_cfg,                               \
  }

EebusError EebusDataListDataAppend(void*** ar, size_t* ar_size, const void* el);
EebusError EebusDataListDataRemove(void*** ar, size_t* ar_size, const void* el);
EebusError EebusDataListDataAppendList(void*** ar, size_t* ar_size, const void** elements, size_t elements_size);

typedef struct EebusDataListMatchIterator EebusDataListMatchIterator;

struct EebusDataListMatchIterator {
  const EebusDataCfg* ar_element_cfg;
  const void* const* ar;
  size_t size;
  const void* data_to_match_base_addr;
  const void* const* first;
  const void* const* last;
  const void* const* current;
};

void EebusDataListMatchIteratorConstruct(
    EebusDataListMatchIterator* self,
    const EebusDataCfg* cfg,
    const void* base_addr,
    const void* data_to_match_base_addr
);

void EebusDataListMatchIteratorNext(EebusDataListMatchIterator* self);
const void* EebusDataListMatchIteratorGet(const EebusDataListMatchIterator* self);
bool EebusDataListMatchIteratorIsDone(const EebusDataListMatchIterator* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_EEBUS_DATA_EEBUS_DATA_LIST_H_
