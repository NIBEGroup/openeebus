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
 * @brief EEEBUS Data List and EEBUS Data List Simple implementation.
 * The only difference between List and List Simple is that
 * List Simple does not have identifiers and WritePartial method acts like
 * WriteElements one
 */

#include <stdbool.h>
#include <string.h>

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_assert.h"
#include "src/common/eebus_data/eebus_data_base.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/eebus_data/eebus_data_simple.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"

static const size_t kMaxElementsNum = EEBUS_DATA_LIST_MAX_ELEMENTS_NUM;

static void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr);
static EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_item);
static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_item);
static EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr);
static bool
Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr);
static bool IsNull(const EebusDataCfg* cfg, const void* base_addr);
static bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr);
static bool HasIdentifiers(const EebusDataCfg* cfg, const void* base_addr);
static EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
);
static EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr);
static EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);
static void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
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
static void Delete(const EebusDataCfg* cfg, void* base_addr);

const EebusDataInterface eebus_data_list_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = CopyMatching,
    .compare               = Compare,
    .is_null               = IsNull,
    .is_empty              = IsEmpty,
    .has_identifiers       = HasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = WritePartial,
    .delete_elements       = DeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = Delete,
};

static EebusError WritePartialSimple(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
);

const EebusDataInterface eebus_data_list_simple_methods = {
    .create_empty          = CreateEmpty,
    .parse                 = EebusDataBaseParse,
    .print_unformatted     = EebusDataBasePrintUnformatted,
    .from_json_object_item = FromJsonObjectItem,
    .from_json_object      = EebusDataBaseFromJsonObject,
    .to_json_object_item   = ToJsonObjectItem,
    .to_json_object        = EebusDataBaseToJsonObject,
    .copy                  = EebusDataBaseCopy,
    .copy_matching         = CopyMatching,
    .compare               = Compare,
    .is_null               = IsNull,
    .is_empty              = IsEmpty,
    .has_identifiers       = HasIdentifiers,
    .selectors_match       = EebusDataBaseSelectorsMatch,
    .identifiers_match     = EebusDataBaseIdentifiersMatch,
    .read_elements         = ReadElements,
    .write                 = Write,
    .write_elements        = EebusDataBaseWriteElements,
    .write_partial         = WritePartialSimple,
    .delete_elements       = DeleteElements,
    .delete_partial        = DeletePartial,
    .delete_               = Delete,
};

void* CreateEmpty(const EebusDataCfg* cfg, void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    EEBUS_ASSERT_ALWAYS();
    return NULL;
}

void** CreateListBuffer(size_t n) {
    if ((n > SIZE_MAX / sizeof(void*)) || (n > kMaxElementsNum)) {
        return NULL;
    }

    const size_t buf_size = n * sizeof(void*);
    void** const buf      = (void**)EEBUS_MALLOC(buf_size);
    if (buf != NULL) {
        memset(buf, 0, buf_size);
    }

    return buf;
}

EebusError FromJsonObjectItem(const EebusDataCfg* cfg, void* base_addr, const JsonObject* json_obj) {
    if (!JsonIsArray(json_obj)) {
        return kEebusErrorParse;
    }

    const size_t n = JsonGetArraySize(json_obj);

    if (n == 0) {
        // Ok - empty array
        return kEebusErrorOk;
    }

    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    *ar = CreateListBuffer(n);
    if (*ar == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    *ar_size = n;

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    for (size_t i = 0; i < JsonGetArraySize(json_obj); ++i) {
        const JsonObject* const json_el = JsonGetArrayItem(json_obj, i);

        const EebusError ret = EEBUS_DATA_FROM_JSON_OBJECT_ITEM(ar_element_cfg, (void*)&(*ar)[i], json_el);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

static EebusError ToJsonObjectItem(const EebusDataCfg* cfg, const void* base_addr, JsonObject** json_obj) {
    const void*** const ar      = (const void***)((const uint8_t*)base_addr + cfg->offset);
    const size_t* const ar_size = (const size_t*)((const uint8_t*)base_addr + cfg->size_offset);
    if (*ar == NULL) {
        *json_obj = NULL;
        return kEebusErrorOk;
    }

    *json_obj = JsonCreateArray();
    if (*json_obj == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    const EebusDataCfg* const element_cfg = (const EebusDataCfg*)cfg->metadata;

    JsonObject* json_el = NULL;
    for (size_t i = 0; i < *ar_size; ++i) {
        const EebusError ret = EEBUS_DATA_TO_JSON_OBJECT_ITEM(element_cfg, (void*)&(*ar)[i], &json_el);
        if (ret != kEebusErrorOk) {
            JsonDelete(*json_obj);
            *json_obj = NULL;
            return ret;
        }

        if (!JsonAddItemToArray(*json_obj, json_el)) {
            JsonDelete(json_el);
            JsonDelete(*json_obj);
            *json_obj = NULL;
            return kEebusErrorMemoryAllocate;
        }
    }

    return kEebusErrorOk;
}

EebusError
CopyMatching(const EebusDataCfg* cfg, const void* base_addr, void* dst_base_addr, const void* data_to_match_base_addr) {
    const void*** const ar = (const void***)((const uint8_t*)base_addr + cfg->offset);
    if (*ar == NULL) {
        EEBUS_DATA_DELETE(cfg, dst_base_addr);
        return kEebusErrorOk;
    }

    const size_t* const ar_size = (const size_t*)((const uint8_t*)base_addr + cfg->size_offset);

    void*** const dst_ar      = (void***)((uint8_t*)dst_base_addr + cfg->offset);
    size_t* const dst_ar_size = (size_t*)((uint8_t*)dst_base_addr + cfg->size_offset);

    // 1. Clear the dst data if any
    if (*dst_ar != NULL) {
        EEBUS_DATA_DELETE(cfg, dst_base_addr);
        *dst_ar      = NULL;
        *dst_ar_size = 0;
    }

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    // 2. Get the number of elements
    size_t new_dst_size = 0;
    for (size_t i = 0; i < *ar_size; ++i) {
        // Data to match fields are processed same as selectors
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], ar_element_cfg, data_to_match_base_addr)) {
            ++new_dst_size;
        }
    }

    if (new_dst_size == 0) {
        // No data to be copied
        return kEebusErrorOk;
    }

    // 3. Allocate the new buffer
    *dst_ar = CreateListBuffer(new_dst_size);
    if (*dst_ar == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    // 4. Copy the matching list items
    EebusError ret = kEebusErrorOk;
    for (size_t i = 0; i < *ar_size; ++i) {
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], ar_element_cfg, data_to_match_base_addr)) {
            ret = EEBUS_DATA_WRITE(ar_element_cfg, &(*dst_ar)[*dst_ar_size], (void*)&(*ar)[i]);
            if (ret != kEebusErrorOk) {
                return ret;
            }

            ++(*dst_ar_size);
        }
    }

    return kEebusErrorOk;
}

bool Compare(const EebusDataCfg* a_cfg, const void* a_base_addr, const EebusDataCfg* b_cfg, const void* b_base_addr) {
    const void*** const a_ar      = (const void***)((const uint8_t*)a_base_addr + a_cfg->offset);
    const size_t* const a_ar_size = (const size_t*)((const uint8_t*)a_base_addr + a_cfg->size_offset);

    const void*** const b_ar      = (const void***)((const uint8_t*)b_base_addr + b_cfg->offset);
    const size_t* const b_ar_size = (const size_t*)((const uint8_t*)b_base_addr + b_cfg->size_offset);

    if ((*a_ar == NULL) || (*b_ar == NULL)) {
        return *a_ar == *b_ar;
    }

    if (*a_ar_size != *b_ar_size) {
        return false;
    }

    const EebusDataCfg* const a_ar_element_cfg = (EebusDataCfg*)a_cfg->metadata;
    const EebusDataCfg* const b_ar_element_cfg = (EebusDataCfg*)b_cfg->metadata;

    for (size_t i = 0; i < *a_ar_size; ++i) {
        if (!EEBUS_DATA_COMPARE(
                a_ar_element_cfg,
                (const void*)&(*a_ar)[i],
                b_ar_element_cfg,
                (const void*)&(*b_ar)[i]
            )) {
            return false;
        }
    }

    return true;
}

bool IsNull(const EebusDataCfg* cfg, const void* base_addr) {
    const void*** const ar = (const void***)((const uint8_t*)base_addr + cfg->offset);
    return *ar == NULL;
}

bool IsEmpty(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);

    return false;
}

bool HasIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    UNUSED(cfg);
    UNUSED(base_addr);
    // Return false is required to handle differnet nested lists partial write separately
    return false;
}

bool ListElementsHaveIdentifiers(const EebusDataCfg* cfg, const void* base_addr) {
    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    if ((*ar == NULL) || (*ar_size == 0)) {
        return false;
    }

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;
    // TODO: Check why only first element is checked for having identifiers in spine-go
    return EEBUS_DATA_HAS_IDENTIFIERS(ar_element_cfg, (void*)&(*ar)[0]);
}

EebusError ReadElements(
    const EebusDataCfg* cfg,
    const void* base_addr,
    void* dst_base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr)) {
        // Should not be written - ok
        return kEebusErrorOk;
    }

    EEBUS_DATA_DELETE(cfg, dst_base_addr);
    return EEBUS_DATA_COPY(cfg, base_addr, dst_base_addr);
}

EebusError Write(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    const void*** const src_ar = (const void***)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_ar == NULL) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        return kEebusErrorOk;
    }

    const size_t* const src_ar_size = (const size_t*)((const uint8_t*)src_base_addr + cfg->size_offset);

    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    if ((*ar != NULL) && (*ar_size != *src_ar_size)) {
        EEBUS_DATA_DELETE(cfg, base_addr);
        *ar = NULL;
    }

    if (*ar == NULL) {
        *ar = CreateListBuffer(*src_ar_size);
        if (*ar == NULL) {
            return kEebusErrorMemoryAllocate;
        }
    }

    *ar_size = *src_ar_size;

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    for (size_t i = 0; i < *src_ar_size; ++i) {
        const EebusError ret = EEBUS_DATA_WRITE(ar_element_cfg, (void*)&(*ar)[i], (const void*)&(*src_ar)[i]);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

EebusError CopyToSelectedData(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(selectors_matcher);

    if (EEBUS_DATA_IS_NULL(cfg, src_base_addr)) {
        return kEebusErrorOk;
    }

    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    const void*** const src_ar = (const void***)((const uint8_t*)src_base_addr + cfg->offset);

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;
    // Write the non-null elements from src[0] to the selected list items
    for (size_t i = 0; i < *ar_size; ++i) {
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], selectors_cfg, selectors_base_addr)) {
            const EebusError ret = EEBUS_DATA_WRITE_ELEMENTS(ar_element_cfg, (void*)&(*ar)[i], (void*)&(*src_ar)[0]);
            if (ret != kEebusErrorOk) {
                return ret;
            }
        }
    }

    return kEebusErrorOk;
}

EebusError CopyToAllData(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    void*** const ar            = (void***)((uint8_t*)base_addr + cfg->offset);
    const size_t* const ar_size = (const size_t*)((const uint8_t*)base_addr + cfg->size_offset);

    const void*** const src_ar = (const void***)((const uint8_t*)src_base_addr + cfg->offset);
    if (*src_ar == NULL) {
        // Nothing to be copied — ok
        return kEebusErrorOk;
    }

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;
    // Write the non-null elements from src[0] to all of the list items
    for (size_t i = 0; i < *ar_size; ++i) {
        const EebusError ret
            = EEBUS_DATA_WRITE_PARTIAL(ar_element_cfg, (void*)&(*ar)[i], (void*)&(*src_ar)[0], NULL, NULL, NULL);
        if (ret != kEebusErrorOk) {
            return ret;
        }
    }

    return kEebusErrorOk;
}

void** GetItemMatchingIdentifiers(const EebusDataCfg* ar_element_cfg, void** ar, size_t ar_size, const void* el) {
    for (size_t i = 0; i < ar_size; ++i) {
        if (EEBUS_DATA_IDENTIFIERS_MATCH(ar_element_cfg, &ar[i], &el)) {
            return &ar[i];
        }
    }

    return NULL;
}

EebusError MergeData(const EebusDataCfg* cfg, void* base_addr, const void* src_base_addr) {
    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    const void*** const src_ar      = (const void***)((const uint8_t*)src_base_addr + cfg->offset);
    const size_t* const src_ar_size = (const size_t*)((const uint8_t*)src_base_addr + cfg->size_offset);

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    // 1. Get the number of elements
    size_t new_size = *ar_size;
    for (size_t i = 0; i < *src_ar_size; ++i) {
        if (GetItemMatchingIdentifiers(ar_element_cfg, *ar, *ar_size, (*src_ar)[i]) == NULL) {
            ++new_size;
        }
    }

    // 2. Allocate the new buffer if new elements are to be added
    void** new_ar = *ar;
    if (new_size > *ar_size) {
        new_ar = CreateListBuffer(new_size);
        if (new_ar == NULL) {
            return kEebusErrorMemoryAllocate;
        }

        for (size_t i = 0; i < *ar_size; ++i) {
            new_ar[i] = (*ar)[i];
        }
    }

    // 3. Modify/append the elements
    EebusError ret = kEebusErrorOk;
    for (size_t i = 0, j = *ar_size; i < *src_ar_size; ++i) {
        void** const el = GetItemMatchingIdentifiers(ar_element_cfg, new_ar, *ar_size, (*src_ar)[i]);
        if (el != NULL) {
            ret = EEBUS_DATA_WRITE_PARTIAL(ar_element_cfg, el, (void*)&(*src_ar)[i], NULL, NULL, NULL);
        } else {
            ret = EEBUS_DATA_WRITE(ar_element_cfg, &new_ar[j++], (void*)&(*src_ar)[i]);
        }

        if (ret != kEebusErrorOk) {
            break;
        }
    }

    // 4. Delete the old list buffer
    if (*ar != new_ar) {
        EEBUS_FREE(*ar);
    }

    // 5. Update list buffer and size
    *ar      = new_ar;
    *ar_size = new_size;
    return ret;
}

EebusError WritePartial(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    if ((selectors_cfg != NULL) && (!EEBUS_DATA_IS_NULL(selectors_cfg, selectors_base_addr))) {
        return CopyToSelectedData(cfg, base_addr, src_base_addr, selectors_cfg, selectors_base_addr, selectors_matcher);
    } else if (!ListElementsHaveIdentifiers(cfg, src_base_addr)) {
        return CopyToAllData(cfg, base_addr, src_base_addr);
    } else {
        return MergeData(cfg, base_addr, src_base_addr);
    }
}

void DeleteElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    if (EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr)) {
        // Should not be deleted - ok
        return;
    }

    EEBUS_DATA_DELETE(cfg, base_addr);
}

void DeleteListItems(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(selectors_matcher);

    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    // 1. Get the number of elements
    size_t new_size = *ar_size;
    for (size_t i = 0; i < *ar_size; ++i) {
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], selectors_cfg, selectors_base_addr)) {
            --new_size;
        }
    }

    if (new_size == *ar_size) {
        // No data to be deleted
        return;
    }

    // 2. Allocate the new buffer
    void** const new_ar = CreateListBuffer(new_size);
    if (new_ar == NULL) {
        return;
    }

    // 3. Delete the selected elements and move the others to new buffer
    for (size_t i = 0, j = 0; i < *ar_size; ++i) {
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], selectors_cfg, selectors_base_addr)) {
            EEBUS_DATA_DELETE(ar_element_cfg, (void*)&(*ar)[i]);
        } else {
            new_ar[j++] = (*ar)[i];
        }
    }

    // 4. Delete the old list buffer
    EEBUS_FREE(*ar);

    // 5. Update list buffer and size
    *ar      = new_ar;
    *ar_size = new_size;
}

void DeleteListItemsElements(
    const EebusDataCfg* cfg,
    void* base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher,
    const EebusDataCfg* elements_cfg,
    const void* elements_base_addr
) {
    UNUSED(selectors_matcher);

    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;
    for (size_t i = 0; i < *ar_size; ++i) {
        if ((elements_base_addr == NULL)
            || EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, (void*)&(*ar)[i], selectors_cfg, selectors_base_addr)) {
            EEBUS_DATA_DELETE_ELEMENTS(ar_element_cfg, (void*)&(*ar)[i], elements_cfg, elements_base_addr);
        }
    }
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
    const bool selectors_buf_is_null = EEBUS_DATA_IS_NULL(selectors_cfg, selectors_base_addr);
    const bool elements_buf_is_null  = EEBUS_DATA_IS_NULL(elements_cfg, elements_base_addr);
    if (selectors_buf_is_null && elements_buf_is_null) {
        return;
    }

    if (!selectors_buf_is_null && elements_buf_is_null) {
        // Remove the selected items from list
        DeleteListItems(cfg, base_addr, selectors_cfg, selectors_base_addr, selectors_matcher);
    } else {
        // Remove elements form list items:
        //   - Remove elements from selected items only or
        //   - Remove elements from all of the items
        DeleteListItemsElements(
            cfg,
            base_addr,
            selectors_cfg,
            selectors_base_addr,
            selectors_matcher,
            elements_cfg,
            elements_base_addr
        );
    }
}

void Delete(const EebusDataCfg* cfg, void* base_addr) {
    void*** const ar      = (void***)((uint8_t*)base_addr + cfg->offset);
    size_t* const ar_size = (size_t*)((uint8_t*)base_addr + cfg->size_offset);

    const EebusDataCfg* const ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    if (*ar == NULL) {
        return;
    }

    for (size_t i = 0; i < *ar_size; ++i) {
        EEBUS_DATA_DELETE(ar_element_cfg, (void*)&(*ar)[i]);
    }

    EEBUS_FREE((void*)*ar);
    *ar      = NULL;
    *ar_size = 0;
}

EebusError EebusDataListDataAppend(void*** ar, size_t* ar_size, const void* el) {
    if ((ar == NULL) || (ar_size == NULL) || (el == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    void** const new_ar = CreateListBuffer(*ar_size + 1);
    if (new_ar == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    for (size_t i = 0; i < *ar_size; ++i) {
        new_ar[i] = (*ar)[i];
    }

    EEBUS_FREE(*ar);
    new_ar[*ar_size] = (void*)el;
    ++(*ar_size);
    *ar = new_ar;
    return kEebusErrorOk;
}

EebusError EebusDataListDataRemove(void*** ar, size_t* ar_size, const void* el) {
    if ((ar == NULL) || (ar_size == NULL) || (el == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    size_t n = 0;
    for (size_t i = 0; i < *ar_size; ++i) {
        if ((*ar)[i] != el) {
            ++n;
        }
    }

    void** new_ar = NULL;
    if (n > 0) {
        new_ar = CreateListBuffer(n);
        if (new_ar == NULL) {
            return kEebusErrorMemoryAllocate;
        }

        for (size_t i = 0, j = 0; i < *ar_size; ++i) {
            if ((*ar)[i] != el) {
                new_ar[j++] = (*ar)[i];
            }
        }
    }

    EEBUS_FREE(*ar);
    *ar = new_ar;
    --(*ar_size);
    return kEebusErrorOk;
}

EebusError EebusDataListDataAppendList(void*** ar, size_t* ar_size, const void** elements, size_t elements_size) {
    if ((ar == NULL) || (ar_size == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    if ((elements == NULL) || (elements_size == 0)) {
        return kEebusErrorInputArgumentNull;
    }

    void** const new_ar = CreateListBuffer(*ar_size + elements_size);
    if (new_ar == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    for (size_t i = 0; i < *ar_size; ++i) {
        new_ar[i] = (*ar)[i];
    }

    EEBUS_FREE(*ar);
    for (size_t i = 0; i < elements_size; ++i) {
        new_ar[*ar_size + i] = (void*)elements[i];
    }

    *ar_size += elements_size;
    *ar = new_ar;
    return kEebusErrorOk;
}

const void* const* EebusDataListDataFindMatching(
    const EebusDataCfg* ar_element_cfg,
    const void* const* first,
    const void* const* last,
    const void* data_to_match_base_addr
) {
    if (ar_element_cfg == NULL) {
        return NULL;
    }

    for (const void* const* p = first; p <= last; ++p) {
        if (EEBUS_DATA_SELECTORS_MATCH(ar_element_cfg, p, ar_element_cfg, data_to_match_base_addr)) {
            return p;
        }
    }

    return NULL;
}

void EebusDataListMatchIteratorConstruct(
    EebusDataListMatchIterator* self,
    const EebusDataCfg* cfg,
    const void* base_addr,
    const void* data_to_match_base_addr
) {
    self->ar_element_cfg = NULL;

    self->ar   = NULL;
    self->size = 0;

    self->data_to_match_base_addr = NULL;

    self->first   = NULL;
    self->last    = NULL;
    self->current = NULL;

    if ((base_addr == NULL) || (cfg == NULL) || (data_to_match_base_addr == NULL)) {
        return;
    }

    self->ar_element_cfg = (EebusDataCfg*)cfg->metadata;

    self->ar   = *(const void* const* const*)((uint8_t*)base_addr + cfg->offset);
    self->size = *(size_t*)((uint8_t*)base_addr + cfg->size_offset);

    if ((self->ar == NULL) || (self->size == 0)) {
        return;
    }

    self->data_to_match_base_addr = data_to_match_base_addr;

    self->first = self->ar;
    self->last  = self->ar + self->size - 1;
    self->current
        = EebusDataListDataFindMatching(self->ar_element_cfg, self->first, self->last, &self->data_to_match_base_addr);
}

const void* EebusDataListMatchIteratorGet(const EebusDataListMatchIterator* self) {
    if (self->current == NULL) {
        return NULL;
    }

    return *self->current;
}

bool EebusDataListMatchIteratorIsDone(const EebusDataListMatchIterator* self) {
    return (self->current == NULL);
}

void EebusDataListMatchIteratorNext(EebusDataListMatchIterator* self) {
    if (self->current == NULL) {
        return;
    }

    self->current = EebusDataListDataFindMatching(
        self->ar_element_cfg,
        self->current + 1,
        self->last,
        &self->data_to_match_base_addr
    );
}

static EebusError WritePartialSimple(
    const EebusDataCfg* cfg,
    void* base_addr,
    const void* src_base_addr,
    const EebusDataCfg* selectors_cfg,
    const void* selectors_base_addr,
    SelectorsMatcher selectors_matcher
) {
    UNUSED(selectors_cfg);
    UNUSED(selectors_base_addr);
    UNUSED(selectors_matcher);

    return EEBUS_DATA_WRITE_ELEMENTS(cfg, base_addr, src_base_addr);
}
