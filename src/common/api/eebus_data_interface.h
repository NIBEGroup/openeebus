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
 * @brief EEBUS Data interface declarations
 */

#ifndef SRC_COMMON_API_EEBUS_DATA_INTERFACE_H_
#define SRC_COMMON_API_EEBUS_DATA_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#include "src/common/eebus_errors.h"
#include "src/common/json.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum EebusDataFlag {
    kEebusDataFlagIsIdentifier    = 1,
    kEebusDataFlagIsSubIdentifier = 2,
    kEebusDataFlagIsReadOnly      = 3,
};

typedef bool (*SelectorsMatcher)(const void* selectors, const void* data);

/**
 * @brief EEBUS Data Interface
 * (EEBUS Data "virtual functions table" declaration)
 */
typedef struct EebusDataInterface EebusDataInterface;

/**
 * @brief EEBUS Data Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusDataCfg EebusDataCfg;

/**
 * @brief EebusData Interface Structure
 */
struct EebusDataInterface {
    void* (*create_empty)(const EebusDataCfg* cfg, void* base_addr);
    void* (*parse)(const EebusDataCfg* cfg, const char* s);
    char* (*print_unformatted)(const EebusDataCfg* cfg, const void* base_addr);
    EebusError (*from_json_object_item)(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item);
    EebusError (*from_json_object)(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj, bool is_root);
    EebusError (*to_json_object_item)(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item);
    EebusError (*to_json_object)(const EebusDataCfg* cfg, const void* base_addr, JsonObject* json_obj, bool is_root);
    EebusError (*copy)(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr);
    EebusError (*copy_matching)(
        const EebusDataCfg* cfg,
        const void* base_addr,
        void* dst_base_addr,
        const void* data_to_match_base_addr
    );
    bool (*compare)(
        const EebusDataCfg* a_cfg,
        const void* a_base_addr,
        const EebusDataCfg* b_cfg,
        const void* b_base_addr
    );
    bool (*is_null)(const EebusDataCfg* cfg, const void* base_addr);
    bool (*is_empty)(const EebusDataCfg* cfg, const void* base_addr);
    bool (*has_identifiers)(const EebusDataCfg* cfg, const void* base_addr);
    bool (*selectors_match)(
        const EebusDataCfg* cfg,
        const void* base_addr,
        const EebusDataCfg* selectors_cfg,
        const void* selectors_base_addr
    );
    bool (*identifiers_match)(const EebusDataCfg* cfg, const void* base_addr, const void* src_base_addr);
    EebusError (*read_elements)(
        const EebusDataCfg* cfg,
        const void* base_addr,
        void* dst_base_addr,
        const EebusDataCfg* elements_cfg,
        const void* elements_base_addr
    );
    EebusError (*write)(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
    EebusError (*write_elements)(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
    EebusError (*write_partial)(
        const EebusDataCfg* cfg,
        void* base_addr,
        const void* src_base_addr,
        const EebusDataCfg* selectors_cfg,
        const void* selectors_base_addr,
        SelectorsMatcher selectors_matcher
    );
    void (*delete_elements)(
        const EebusDataCfg* cfg,
        void* base_addr,
        const EebusDataCfg* elements_cfg,
        const void* elements_base_addr
    );
    void (*delete_partial)(
        const EebusDataCfg* cfg,
        void* base_addr,
        const EebusDataCfg* selectors_cfg,
        const void* selectors_base_addr,
        SelectorsMatcher selectors_matcher,
        const EebusDataCfg* elements_cfg,
        const void* elements_base_addr
    );
    void (*delete_)(const EebusDataCfg* cfg, void* base_addr);
};

/**
 * @brief EEBUS Data configuration structure
 */
struct EebusDataCfg {
    /**
     * @brief EebusData virtual function table
     */
    const EebusDataInterface* interface_;
    /**
     * @brief Json data name
     */
    const char* name;
    /**
     * @brief Offset of referred structure field
     */
    size_t offset;
    /**
     * @brief Size of referred structure field
     */
    union {
        size_t size;
        size_t size_offset;
        int32_t type_id_offset;
    };
    /**
     * @brief Specific data flags e.g. identifier or readonly (see @EebusDataFlag)
     */
    uint32_t flags;
    /**
     * @brief Specific data that can vary depending on Data Configuration type
     * (see EEBUS_JSON_ definitions group). In many cases, metadata is used
     * as Data Configuration tree branch entry point
     */
    const void* metadata;
};

/**
 * @brief EEBUS Data Interface class pointer typecast
 */
#define EEBUS_DATA_INTERFACE(cfg) ((cfg)->interface_)

/**
 * @brief EEBUS Data type equivalent check
 */
#define EEBUS_DATA_TYPE_EQ(a_cfg, b_cfg) (EEBUS_DATA_INTERFACE(a_cfg) == EEBUS_DATA_INTERFACE(b_cfg))

/**
 * @brief EEBUS Data Create Empty caller definition
 */
#define EEBUS_DATA_CREATE_EMPTY(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->create_empty(cfg, base_addr))

/**
 * @brief EEBUS Data Parse caller definition
 */
#define EEBUS_DATA_PARSE(cfg, s) (EEBUS_DATA_INTERFACE(cfg)->parse(cfg, s))

/**
 * @brief EEBUS Data Print Unformatted caller definition
 */
#define EEBUS_DATA_PRINT_UNFORMATTED(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->print_unformatted(cfg, base_addr))

/**
 * @brief EEBUS Data From Json Object Item caller definition
 */
#define EEBUS_DATA_FROM_JSON_OBJECT_ITEM(cfg, base_addr, json_item) \
    (EEBUS_DATA_INTERFACE(cfg)->from_json_object_item(cfg, base_addr, json_item))

/**
 * @brief EEBUS Data From Json Object caller definition
 */
#define EEBUS_DATA_FROM_JSON_OBJECT(cfg, base_addr, json_obj, is_root) \
    (EEBUS_DATA_INTERFACE(cfg)->from_json_object(cfg, base_addr, json_obj, is_root))

/**
 * @brief EEBUS Data To Json Object Item caller definition
 */
#define EEBUS_DATA_TO_JSON_OBJECT_ITEM(cfg, base_addr, json_item) \
    (EEBUS_DATA_INTERFACE(cfg)->to_json_object_item(cfg, base_addr, json_item))

/**
 * @brief EEBUS Data To Json Object caller definition
 */
#define EEBUS_DATA_TO_JSON_OBJECT(cfg, base_addr, json_obj, is_root) \
    (EEBUS_DATA_INTERFACE(cfg)->to_json_object(cfg, base_addr, json_obj, is_root))

/**
 * @brief EEBUS Data Copy caller definition
 */
#define EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr) (EEBUS_DATA_INTERFACE(cfg)->copy(cfg, base_addr, dst_base_addr))

/**
 * @brief EEBUS Data Copy Matching caller definition
 */
#define EEBUS_DATA_COPY_MATCHING(cfg, base_addr, dst_base_addr, data_to_match_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->copy_matching(cfg, base_addr, dst_base_addr, data_to_match_base_addr))

/**
 * @brief EEBUS Data Compare caller definition
 */
#define EEBUS_DATA_COMPARE(a_cfg, a_base_addr, b_cfg, b_base_addr) \
    (EEBUS_DATA_INTERFACE(a_cfg)->compare(a_cfg, a_base_addr, b_cfg, b_base_addr))

/**
 * @brief EEBUS Data Is Null caller definition
 */
#define EEBUS_DATA_IS_NULL(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->is_null(cfg, base_addr))

/**
 * @brief EEBUS Data Is Empty caller definition
 */
#define EEBUS_DATA_IS_EMPTY(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->is_empty(cfg, base_addr))

/**
 * @brief EEBUS Data Has Identifiers caller definition
 */
#define EEBUS_DATA_HAS_IDENTIFIERS(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->has_identifiers(cfg, base_addr))

/**
 * @brief EEBUS Data Selectors Match caller definition
 */
#define EEBUS_DATA_SELECTORS_MATCH(cfg, base_addr, selectors_cfg, selectors_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->selectors_match(cfg, base_addr, selectors_cfg, selectors_base_addr))

/**
 * @brief EEBUS Data Identifiers Match caller definition
 */
#define EEBUS_DATA_IDENTIFIERS_MATCH(cfg, base_addr, src_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->identifiers_match(cfg, base_addr, src_base_addr))

/**
 * @brief EEBUS Data Read Elements caller definition
 */
#define EEBUS_DATA_READ_ELEMENTS(cfg, base_addr, dst_base_addr, elements_cfg, elements_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->read_elements(cfg, base_addr, dst_base_addr, elements_cfg, elements_base_addr))

/**
 * @brief EEBUS Data Write caller definition
 */
#define EEBUS_DATA_WRITE(cfg, base_addr, src_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->write(cfg, base_addr, src_base_addr))

/**
 * @brief EEBUS Data Write Elements caller definition
 */
#define EEBUS_DATA_WRITE_ELEMENTS(cfg, base_addr, src_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->write_elements(cfg, base_addr, src_base_addr))

/**
 * @brief EEBUS Data Write Partial caller definition
 */
#define EEBUS_DATA_WRITE_PARTIAL(cfg, base_addr, src_base_addr, selectors_cfg, selectors_base_addr, selectors_matcher) \
    (EEBUS_DATA_INTERFACE(cfg)                                                                                         \
         ->write_partial(cfg, base_addr, src_base_addr, selectors_cfg, selectors_base_addr, selectors_matcher))

/**
 * @brief EEBUS Data Delete Elements caller definition
 */
#define EEBUS_DATA_DELETE_ELEMENTS(cfg, base_addr, elements_cfg, elements_base_addr) \
    (EEBUS_DATA_INTERFACE(cfg)->delete_elements(cfg, base_addr, elements_cfg, elements_base_addr))

/**
 * @brief EEBUS Data Delete Partial caller definition
 */
#define EEBUS_DATA_DELETE_PARTIAL(              \
    cfg,                                        \
    base_addr,                                  \
    selectors_cfg,                              \
    selectors_base_addr,                        \
    selectors_matcher,                          \
    elements_cfg,                               \
    elements_base_addr                          \
)                                               \
    (EEBUS_DATA_INTERFACE(cfg)->delete_partial( \
        cfg,                                    \
        base_addr,                              \
        selectors_cfg,                          \
        selectors_base_addr,                    \
        selectors_matcher,                      \
        elements_cfg,                           \
        elements_base_addr                      \
    ))

/**
 * @brief EEBUS Data Delete caller definition
 */
#define EEBUS_DATA_DELETE(cfg, base_addr) (EEBUS_DATA_INTERFACE(cfg)->delete_(cfg, base_addr))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_API_EEBUS_DATA_INTERFACE_H_
