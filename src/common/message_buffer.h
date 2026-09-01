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
 * @brief Message Buffer declaration
 */
#ifndef SRC_COMMON_MESSAGE_BUFFER_H_
#define SRC_COMMON_MESSAGE_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef void (*MessageBufferDeallocator)(void*);

typedef struct MessageBuffer MessageBuffer;

struct MessageBuffer {
    uint8_t* data;
    size_t data_size;
    MessageBufferDeallocator deallocator;
};

/**
 * @brief Initialise the Message Buffer. It is expected that buffer @p data has
 * been cereated using EEBUS_MALLOC() so default deallocator based on EEBUS_FREE()
 * is assigned
 * @param msg_buf Message Buffer instance to be initialised
 * @param data Data ownership of which is shifted to Message Buffer
 * @param data_size Size of @p data
 */
void MessageBufferInit(MessageBuffer* msg_buf, uint8_t* data, size_t data_size);

/**
 * @brief Initialise the Message Buffer with specific data deallocator.
 * Pass @p deallocator = NULL when dealing with statically allocated data.
 * @param msg_buf Message Buffer instance to be initialised
 * @param data Data ownership of which is shifted to Message Buffer
 * @param data_size Size of @p data
 * @param deallocator Deallocator to be called with @p data as argument while
 * Message Buffer release procedure
 */
void MessageBufferInitWithDeallocator(
    MessageBuffer* msg_buf,
    uint8_t* data,
    size_t data_size,
    MessageBufferDeallocator deallocator
);

/**
 * @brief Release the Message Buffer instance
 * @param msg_buf Message Buffer instance to be released
 */
void MessageBufferRelease(MessageBuffer* msg_buf);

/**
 * @brief Releases the dst then passes the data ownership from src to dst.
 * @param src Message Buffer instance to move data from
 * @param dst Message Buffer instance to move data to
 */
void MessageBufferMove(MessageBuffer* src, MessageBuffer* dst);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_MESSAGE_BUFFER_H_
