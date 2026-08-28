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
 * @brief Debug functions implementation
 */

#include <libwebsockets.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <sys/timeb.h>
#include <windows.h>
#endif

void DebugPrintf(const char* format, ...) {
    // TODO: Add platform independent timestamp printing
    char timestamp_buf[50] = "";
    lwsl_timestamp(LLL_NOTICE, timestamp_buf, sizeof(timestamp_buf));
    printf("%s ", timestamp_buf);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void DebugHexdump(void* data, size_t data_size) {
    // TODO: Add platform independent hexdump
#ifndef GTEST
    lwsl_hexdump_notice(data, data_size);
#endif  // GTEST
}
