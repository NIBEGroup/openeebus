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
 * @brief EEBUS CLI Handler interface declarations
 */

#ifndef SRC_CLI_EEBUS_CLI_HANDLER_INTERFACE_H_
#define SRC_CLI_EEBUS_CLI_HANDLER_INTERFACE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EEBUS CLI Handler Interface
 * (EEBUS CLI Handler "virtual functions table" declaration)
 */
typedef struct EebusCliHandlerInterface EebusCliHandlerInterface;

/**
 * @brief EEBUS CLI Handler Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EebusCliHandlerObject EebusCliHandlerObject;

/**
 * @brief EebusCliHandler Interface Structure
 */
struct EebusCliHandlerInterface {
    void (*destruct)(EebusCliHandlerObject* self);
    void (*handle_cmd)(const EebusCliHandlerObject* self, const char* const* tokens, size_t num_tokens);
};

/**
 * @brief EEBUS CLI Handler Object Structure
 */
struct EebusCliHandlerObject {
    const EebusCliHandlerInterface* interface_;
};

/**
 * @brief EEBUS CLI Handler pointer typecast
 */
#define EEBUS_CLI_HANDLER_OBJECT(obj) ((EebusCliHandlerObject*)(obj))

/**
 * @brief EEBUS CLI Handler Interface class pointer typecast
 */
#define EEBUS_CLI_HANDLER_INTERFACE(obj) (EEBUS_CLI_HANDLER_OBJECT(obj)->interface_)

/**
 * @brief EEBUS CLI Handler Destruct caller definition
 */
#define EEBUS_CLI_HANDLER_DESTRUCT(obj) (EEBUS_CLI_HANDLER_INTERFACE(obj)->destruct(obj))

/**
 * @brief EEBUS CLI Handler Handle Cmd caller definition
 */
#define EEBUS_CLI_HANDLER_HANDLE_CMD(obj, tokens, num_tokens) \
    (EEBUS_CLI_HANDLER_INTERFACE(obj)->handle_cmd(obj, tokens, num_tokens))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_CLI_EEBUS_CLI_HANDLER_INTERFACE_H_
