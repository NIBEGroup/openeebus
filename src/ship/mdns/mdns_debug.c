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
 * @brief mDNS debug logging implementation
 */

#include "src/ship/mdns/mdns_debug.h"

#if MDNS_DEBUG

#include "src/common/debug.h"

void MdnsTxtRecordPrint(const uint8_t* txt_record, uint16_t txt_record_size) {
    if (txt_record == NULL) {
        DebugPrintf("MdnsTxtRecord: (null)\n");
        return;
    }

    /* When size is given, end marks the first byte past the buffer.
     * When size is 0 the caller guarantees a zero-length sentinel byte. */
    const uint8_t* const end = (txt_record_size > 0) ? (txt_record + txt_record_size) : NULL;
    const uint8_t* ptr       = txt_record;

    DebugPrintf("MdnsTxtRecord:\n");

    while ((end == NULL) || (ptr < end)) {
        const size_t record_len = *ptr;

        if (record_len == 0) {
            break;
        }

        if ((end != NULL) && (ptr + 1 + record_len > end)) {
            DebugPrintf("  (malformed record — length %zu exceeds buffer)\n", record_len);
            break;
        }

        DebugPrintf("  \"\\%03o%.*s\"\n", (int)record_len, (int)record_len, (const char*)(ptr + 1));
        ptr += 1 + record_len;
    }
}

#else

#include "src/common/eebus_arguments.h"

void MdnsTxtRecordPrint(const uint8_t* txt_record, uint16_t txt_record_size) {
    UNUSED(txt_record);
    UNUSED(txt_record_size);
}

#endif  // MDNS_DEBUG
