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
 * @brief Message Buffer implementation
 */

#include "message_buffer.h"

#include "src/common/eebus_malloc.h"

static void MessageBufferDeallocatorDefault(void* data);

void MessageBufferDeallocatorDefault(void* data) {
    EEBUS_FREE(data);
}

void MessageBufferInit(MessageBuffer* msg_buf, uint8_t* data, size_t data_size) {
    MessageBufferInitWithDeallocator(msg_buf, data, data_size, MessageBufferDeallocatorDefault);
}

void MessageBufferInitWithDeallocator(
    MessageBuffer* msg_buf,
    uint8_t* data,
    size_t data_size,
    MessageBufferDeallocator deallocator
) {
    msg_buf->data        = data;
    msg_buf->data_size   = data_size;
    msg_buf->deallocator = deallocator;
}

void MessageBufferRelease(MessageBuffer* msg_buf) {
    if ((msg_buf->deallocator != NULL) && (msg_buf->data != NULL)) {
        msg_buf->deallocator(msg_buf->data);
    }

    msg_buf->data        = NULL;
    msg_buf->data_size   = 0;
    msg_buf->deallocator = NULL;
}

void MessageBufferMove(MessageBuffer* src, MessageBuffer* dst) {
    MessageBufferRelease(dst);

    dst->data        = src->data;
    dst->data_size   = src->data_size;
    dst->deallocator = src->deallocator;

    src->data        = NULL;
    src->data_size   = 0;
    src->deallocator = 0;
}
