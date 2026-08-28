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
 * @file vector.c
 * @brief Vector class implementation
 */

#include "vector.h"

#include <stdbool.h>
#include <string.h>

#include "src/common/eebus_malloc.h"

void VectorConstructWithDeallocator(Vector* self, void (*deallocator)(void*)) {
    if (self == NULL) {
        return;
    }

    self->elements     = NULL;
    self->num_elements = 0;
    self->buffer_size  = 0;
    self->deallocator  = deallocator;
}

Vector* VectorCreateWithDeallocator(void (*deallocator)(void*)) {
    Vector* const vector = (Vector*)EEBUS_MALLOC(sizeof(Vector));

    if (vector != NULL) {
        VectorConstructWithDeallocator(vector, deallocator);
    }

    return vector;
}

void VectorDestruct(Vector* self) {
    if (self != NULL) {
        VectorClear(self);
    }
}

void VectorClear(Vector* self) {
    if ((self != NULL) && (self->elements != NULL)) {
        EEBUS_FREE(self->elements);

        self->elements     = NULL;
        self->num_elements = 0;
        self->buffer_size  = 0;
    }
}

void VectorFreeElements(Vector* self) {
    if ((self == NULL) || (self->elements == NULL)) {
        return;
    }

    for (size_t i = 0; i < self->num_elements; ++i) {
        if (self->elements[i] != NULL) {
            if (self->deallocator != NULL) {
                self->deallocator(self->elements[i]);
            } else {
                EEBUS_FREE(self->elements[i]);
            }

            self->elements[i] = NULL;
        }
    }
}

void VectorMove(Vector* dest_vector, Vector* src_vector) {
    if ((dest_vector == NULL) || (src_vector == NULL)) {
        return;
    }

    if (dest_vector->elements != NULL) {
        VectorClear(dest_vector);
    }

    *dest_vector = *src_vector;

    src_vector->elements     = NULL;
    src_vector->num_elements = 0;
    src_vector->buffer_size  = 0;
}

size_t VectorGetSize(const Vector* self) {
    if (self == NULL) {
        return 0;
    }

    return self->num_elements;
}

void* VectorGetElement(const Vector* self, size_t index) {
    if ((self == NULL) || (index >= self->num_elements)) {
        return NULL;
    }

    return self->elements[index];
}

void VectorPushBack(Vector* self, void* element) {
    if ((self == NULL) || (element == NULL)) {
        return;
    }

    static const size_t num_elements_max = SIZE_MAX / sizeof(void*);
    if (self->num_elements >= num_elements_max) {
        // Prevent overflow in buffer size calculation
        return;
    }

    void** buffer = NULL;

    const size_t buffer_size = (self->num_elements + 1) * sizeof(void*);

    if (buffer_size > self->buffer_size) {
        buffer = (void**)EEBUS_MALLOC(buffer_size);
    }

    if (buffer != NULL) {
        self->buffer_size = buffer_size;

        if (self->elements != NULL) {
            memcpy(buffer, self->elements, self->num_elements * sizeof(void*));
            EEBUS_FREE(self->elements);
        }

        self->elements = buffer;
    }

    if (self->elements == NULL) {
        return;
    }

    self->elements[self->num_elements++] = element;
}

void VectorRemove(Vector* self, void* element) {
    if ((self == NULL) || (element == NULL)) {
        return;
    }

    size_t element_index = 0;
    bool found           = false;
    for (size_t i = 0; i < self->num_elements; ++i) {
        if (self->elements[i] == element) {
            element_index = i;
            found         = true;
            break;
        }
    }

    if (!found) {
        return;
    }

    for (size_t j = element_index; j < self->num_elements - 1; ++j) {
        self->elements[j] = self->elements[j + 1];
    }

    --self->num_elements;
}
