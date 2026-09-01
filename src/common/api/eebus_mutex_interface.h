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
 * @brief EEBUS Mutex interface declarations
 */

#ifndef SRC_COMMON_API_EEBUS_MUTEX_INTERFACE_H_
#define SRC_COMMON_API_EEBUS_MUTEX_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS Mutex Interface
 * (EEBUS Mutex "virtual functions table" declaration)
 */
typedef struct EebusMutexInterface EebusMutexInterface;

/**
 * @brief EEBUS Mutex Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusMutexObject EebusMutexObject;

/**
 * @brief EebusMutex Interface Structure
 */
struct EebusMutexInterface {
    void (*destruct)(EebusMutexObject* self);
    void (*lock)(EebusMutexObject* self);
    void (*unlock)(EebusMutexObject* self);
};

/**
 * @brief EEBUS Mutex Object Structure
 */
struct EebusMutexObject {
    const EebusMutexInterface* interface;
};

/**
 * @brief EEBUS Mutex pointer typecast
 */
#define EEBUS_MUTEX_OBJECT(obj) ((EebusMutexObject*)(obj))

/**
 * @brief EEBUS Mutex Interface class pointer typecast
 */
#define EEBUS_MUTEX_INTERFACE(obj) (EEBUS_MUTEX_OBJECT(obj)->interface)

/**
 * @brief EEBUS Mutex Destruct caller definition
 */
#define EEBUS_MUTEX_DESTRUCT(obj) (EEBUS_MUTEX_INTERFACE(obj)->destruct(obj))

/**
 * @brief EEBUS Mutex Lock caller definition
 */
#define EEBUS_MUTEX_LOCK(obj) (EEBUS_MUTEX_INTERFACE(obj)->lock(obj))

/**
 * @brief EEBUS Mutex Unlock caller definition
 */
#define EEBUS_MUTEX_UNLOCK(obj) (EEBUS_MUTEX_INTERFACE(obj)->unlock(obj))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_API_EEBUS_MUTEX_INTERFACE_H_
