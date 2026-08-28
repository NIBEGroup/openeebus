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
#include "src/common/string_util.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "src/common/eebus_malloc.h"

char* StringCopy(const char* src) {
    if (src == NULL) {
        return NULL;
    }

    char* const dst = (char*)EEBUS_MALLOC(strlen(src) + 1);

    if (dst != NULL) {
        strcpy(dst, src);
    }

    return dst;
}

char* StringNCopy(const char* src, size_t n) {
    if ((src == NULL) || (n == 0)) {
        return NULL;
    }

    char* const dst = (char*)EEBUS_MALLOC(n + 1);

    if (dst != NULL) {
        memset(dst, 0, n + 1);
        strncpy(dst, src, n);
    }

    return dst;
}

bool StringNCompare(const char* a, const char* b, size_t n) {
    if ((a == NULL) || (b == NULL)) {
        return false;
    }

    for (size_t i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return false;
        }

        if (a[i] == '\0') {
            return true;
        }
    }

    return a[n] == '\0' && b[n] == '\0';
}

const char* StringFmtSprintf(const char* format, ...) {
    char* s = NULL;
    va_list args1;
    va_start(args1, format);
    va_list args2;
    va_copy(args2, args1);

    int32_t size = vsnprintf(NULL, 0, format, args1);
    va_end(args1);

    if (size > 0) {
        ++size;
        s = EEBUS_MALLOC((size_t)size);
        if (s != NULL) {
            vsnprintf(s, (size_t)size, format, args2);
        }
    }

    va_end(args2);

    return s;
}

const char* StringRemoveToken(char* s, const char* token) {
    if (StringIsEmpty(s) || StringIsEmpty(token)) {
        return s;
    }

    const size_t token_len = strlen(token);

    char* p = s;
    while ((p = strstr(p, token)) != NULL) {
        const size_t tail = strlen(p + token_len) + 1;  // include NULL terminator
        memmove(p, p + token_len, tail);
    }

    return s;
}

char* StringWithHex(const uint8_t* data, size_t data_len) {
    if ((data == NULL) || (data_len == 0)) {
        return NULL;
    }

    char* const s = EEBUS_MALLOC(data_len * 2 + 1);
    if (s == NULL) {
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < data_len; ++i) {
        const uint8_t h = (data[i] >> 4) & 0x0F;

        s[j++] = ((h >= 10) ? 'a' - 10 : '0') + h;

        const uint8_t l = data[i] & 0x0F;

        s[j++] = ((l >= 10) ? 'a' - 10 : '0') + l;
    }

    s[j] = '\0';

    return s;
}

char* StringToken(char* s, const char* delimiters, char** p) {
    if ((s == NULL) && ((p == NULL) || (*p == NULL))) {
        return NULL;
    }

    char* start = s ? s : *p;

    // Skip leading delimiters
    start += strspn(start, delimiters);
    if (*start == '\0') {
        // No more tokens
        if (p) {
            *p = NULL;
        }

        return NULL;
    }

    // Find the end of the token
    char* end = start + strcspn(start, delimiters);
    if (*end != '\0') {
        *end = '\0';
        if (p) {
            *p = end + 1;
        }
    } else {
        if (p) {
            *p = NULL;
        }
    }

    return start;
}

char* StringToUpper(const char* s) {
    if (s == NULL) {
        return NULL;
    }

    const size_t len   = strlen(s);
    char* const result = (char*)EEBUS_MALLOC(len + 1);
    if (result == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        result[i] = (char)toupper((unsigned char)s[i]);
    }

    result[len] = '\0';

    return result;
}

char* StringGroupByN(const char* s, size_t n) {
    if (s == NULL || n == 0) {
        return NULL;
    }

    const size_t len         = strlen(s);
    const size_t num_spaces  = (len > 0) ? ((len - 1) / n) : 0;
    const size_t grouped_len = len + num_spaces;

    char* const result = (char*)EEBUS_MALLOC(grouped_len + 1);
    if (result == NULL) {
        return NULL;
    }

    size_t k = 0;
    for (size_t i = 0; i < len; ++i) {
        if (i > 0 && (i % n) == 0) {
            result[k++] = ' ';
        }

        result[k++] = s[i];
    }

    result[k] = '\0';

    return result;
}
