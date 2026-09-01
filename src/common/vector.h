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
 * @file vector.h
 * @brief A simplified class similar to C++ std::vector
 * Provides possibility to dynamically create a contiguous
 * elements sequence by pushing new elements back
 */

#ifndef SRC_COMMON_VECTOR_H_
#define SRC_COMMON_VECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/** Vector type definition */
typedef struct Vector Vector;

/** Vector structure */
struct Vector {
    /** Elements array */
    void** elements;

    /** Actual number of vector elements */
    size_t num_elements;

    /** Actual vector buffer size */
    size_t buffer_size;

    /** Vector elements custom deallocator */
    void (*deallocator)(void*);
};

/**
 * @brief Vector constructor with custom deallocator
 * @param self Vector instance to be constructed
 * @param deallocator Custom deallocator function to free vector elements on VectorFreeElements() call.
 * If deallocator is NULL, elements will be freed using EEBUS_FREE
 */
void VectorConstructWithDeallocator(Vector* self, void (*deallocator)(void*));

/**
 * @brief Vector constructor
 * @param self Vector instance to be constructed
 */
static inline void VectorConstruct(Vector* self) {
    VectorConstructWithDeallocator(self, NULL);
}

/**
 * @brief Dynamically make and construct vector with custom deallocator
 * @param deallocator Custom deallocator function to free vector elements on VectorFreeElements() call.
 * If deallocator is NULL, elements will be freed using EEBUS_FREE
 * @return self Pointer on vector instance
 */
Vector* VectorCreateWithDeallocator(void (*deallocator)(void*));

/**
 * @brief Dynamically make and construct vector
 * @return self Pointer on vector instance
 */
static inline Vector* VectorCreate(void) {
    return VectorCreateWithDeallocator(NULL);
}

/**
 * @brief Clean vector data function. Deallocate memory which
 * is used to store pointers on vector's elements.
 * @note Function don't free vector instance which
 * is allocated using VectorCreate() function
 * @param self Vector instance to be destructed
 */
void VectorDestruct(Vector* self);

/**
 * @brief Vector elements destructor
 * The function will call deallocator function if it is set,
 * otherwise it will call EEBUS_FREE for each element
 * @param self Vector instance which elements to free
 */
void VectorFreeElements(Vector* self);

/**
 * @brief Move elements to the other vector
 * @note Function clear dest_vector before moving and clear src_vector after
 * @param dest_vector Vector instance which elements to move
 * @param src_vector Vector source instance
 */
void VectorMove(Vector* dest_vector, Vector* src_vector);

/**
 * @brief Get the number of elements in the vector
 * @param self Vector instance to get size
 * @return Number of elements in the vector
 */
size_t VectorGetSize(const Vector* self);

/**
 * @brief Get vector element by index
 * @param self Vector instance to get the element
 * @param index Vector element index
 */
void* VectorGetElement(const Vector* self, size_t index);

/**
 * @brief Push new element to the array end. If currently allocated
 * memory is not enough to keep the element it will be re-allocated
 * @param self Vector instance to push the new element into
 * @param element New element to be pushed into vector
 */
void VectorPushBack(Vector* self, void* element);

/**
 * @brief Removes the first found element that matches specified one
 * @param self Vector to remove the element from
 * @param element Vector element to be removed
 */
void VectorRemove(Vector* self, void* element);

/**
 * @brief Removes all elements from the vector
 * @param self Vector to clear
 */
void VectorClear(Vector* self);

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_VECTOR_H_
