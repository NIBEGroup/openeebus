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
 * @brief Function interface declarations
 */

#ifndef SRC_SPINE_API_FUNCTION_INTERFACE_H_
#define SRC_SPINE_API_FUNCTION_INTERFACE_H_

#include "src/common/eebus_errors.h"
#include "src/spine/api/operations_interface.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/function_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Function Interface
 * (Function "virtual functions table" declaration)
 */
typedef struct FunctionInterface FunctionInterface;

/**
 * @brief Function Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct FunctionObject FunctionObject;

/**
 * @brief Function Interface Structure
 */
struct FunctionInterface {
    void (*destruct)(FunctionObject* self);
    CmdType* (*create_read_cmd)(const FunctionObject* self, const FilterType* filter_partial);
    FunctionType (*get_function_type)(const FunctionObject* self);
    const void* (*get_data)(const FunctionObject* self);
    CmdType* (*create_reply_cmd)(const FunctionObject* self);
    CmdType* (*create_notify_cmd)(
        const FunctionObject* self,
        const void* new_data,
        const FilterType* filter_partial,
        const FilterType* filter_delete
    );
    CmdType* (*create_write_cmd)(
        const FunctionObject* self,
        const void* new_data,
        const FilterType* filter_partial,
        const FilterType* filter_delete
    );
    void* (*data_copy)(const FunctionObject* self);
    EebusError (*update_data)(
        FunctionObject* self,
        const void* new_data,
        const FilterType* filter_partial,
        const FilterType* filter_delete,
        bool wr_remote,
        bool persist
    );
    const OperationsObject* (*get_operations)(const FunctionObject* self);
    void (*set_operations)(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial);
};

/**
 * @brief Function Object Structure
 */
struct FunctionObject {
    const FunctionInterface* interface_;
};

/**
 * @brief Function pointer typecast
 */
#define FUNCTION_OBJECT(obj) ((FunctionObject*)(obj))

/**
 * @brief Function Interface class pointer typecast
 */
#define FUNCTION_INTERFACE(obj) (FUNCTION_OBJECT(obj)->interface_)

/**
 * @brief Function Destruct caller definition
 */
#define FUNCTION_DESTRUCT(obj) (FUNCTION_INTERFACE(obj)->destruct(obj))

/**
 * @brief Function Create Read Cmd caller definition
 */
#define FUNCTION_CREATE_READ_CMD(obj, filter_partial) (FUNCTION_INTERFACE(obj)->create_read_cmd(obj, filter_partial))

/**
 * @brief Function Get Function Type caller definition
 */
#define FUNCTION_GET_FUNCTION_TYPE(obj) (FUNCTION_INTERFACE(obj)->get_function_type(obj))

/**
 * @brief Function Get Data caller definition
 */
#define FUNCTION_GET_DATA(obj) (FUNCTION_INTERFACE(obj)->get_data(obj))

/**
 * @brief Function Create Reply Cmd caller definition
 */
#define FUNCTION_CREATE_REPLY_CMD(obj) (FUNCTION_INTERFACE(obj)->create_reply_cmd(obj))

/**
 * @brief Function Create Notify Cmd caller definition
 */
#define FUNCTION_CREATE_NOTIFY_CMD(obj, new_data, filter_partial, filter_delete) \
    (FUNCTION_INTERFACE(obj)->create_notify_cmd(obj, new_data, filter_partial, filter_delete))

/**
 * @brief Function Create Write Cmd caller definition
 */
#define FUNCTION_CREATE_WRITE_CMD(obj, new_data, filter_partial, filter_delete) \
    (FUNCTION_INTERFACE(obj)->create_write_cmd(obj, new_data, filter_partial, filter_delete))

/**
 * @brief Function Data Copy caller definition
 */
#define FUNCTION_DATA_COPY(obj) (FUNCTION_INTERFACE(obj)->data_copy(obj))

/**
 * @brief Function Update Data caller definition
 */
#define FUNCTION_UPDATE_DATA(obj, new_data, filter_partial, filter_delete, wr_remote, persist) \
    (FUNCTION_INTERFACE(obj)->update_data(obj, new_data, filter_partial, filter_delete, wr_remote, persist))

/**
 * @brief Function Get Operations caller definition
 */
#define FUNCTION_GET_OPERATIONS(obj) (FUNCTION_INTERFACE(obj)->get_operations(obj))

/**
 * @brief Function Set Operations caller definition
 */
#define FUNCTION_SET_OPERATIONS(obj, read, read_partial, write, write_partial) \
    (FUNCTION_INTERFACE(obj)->set_operations(obj, read, read_partial, write, write_partial))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_API_FUNCTION_INTERFACE_H_
