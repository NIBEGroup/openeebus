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
 * @brief Eebus Thread interface declarations
 */

#ifndef SRC_COMMON_API_EEBUS_THREAD_INTERFACE_H_
#define SRC_COMMON_API_EEBUS_THREAD_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Eebus Thread Interface
 * (Eebus Thread "virtual functions table" declaration)
 */
typedef struct EebusThreadInterface EebusThreadInterface;

/**
 * @brief Eebus Thread Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusThreadObject EebusThreadObject;

/**
 * @brief EebusThread Interface Structure
 */
struct EebusThreadInterface {
    void (*destruct)(EebusThreadObject* self);
    void (*join)(EebusThreadObject* self);
};

/**
 * @brief Eebus Thread Object Structure
 */
struct EebusThreadObject {
    const EebusThreadInterface* interface_;
};

/**
 * @brief Eebus Thread pointer typecast
 */
#define EEBUS_THREAD_OBJECT(obj) ((EebusThreadObject*)(obj))

/**
 * @brief Eebus Thread Interface class pointer typecast
 */
#define EEBUS_THREAD_INTERFACE(obj) (EEBUS_THREAD_OBJECT(obj)->interface_)

/**
 * @brief Eebus Thread Destruct caller definition
 */
#define EEBUS_THREAD_DESTRUCT(obj) (EEBUS_THREAD_INTERFACE(obj)->destruct(obj))

/**
 * @brief Eebus Thread Join caller definition
 */
#define EEBUS_THREAD_JOIN(obj) (EEBUS_THREAD_INTERFACE(obj)->join(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_API_EEBUS_THREAD_INTERFACE_H_
