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
 * @brief Function implementation
 */

#include <string.h>

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_malloc.h"
#include "src/common/num_ptr.h"
#include "src/spine/api/function_interface.h"
#include "src/spine/feature/operations.h"
#include "src/spine/model/cmd.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/model.h"

typedef struct Function Function;

struct Function {
    /** Implements the Function Interface */
    FunctionObject obj;

    FunctionType type;
    void* data;
    OperationsObject* operations;
};

#define FUNCTION(obj) ((Function*)(obj))

static void Destruct(FunctionObject* self);
static CmdType* CreateReadCmd(const FunctionObject* self, const FilterType* filter_partial);
static FunctionType GetFunctionType(const FunctionObject* self);
static const void* GetData(const FunctionObject* self);
static CmdType* CreateReplyCmd(const FunctionObject* self);
static CmdType* CreateNotifyCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static CmdType* CreateWriteCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);
static void* DataCopy(const FunctionObject* self);
static EebusError UpdateData(
    FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool wr_remote,
    bool persist
);
static const OperationsObject* GetOperations(const FunctionObject* self);
static void SetOperations(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial);

static const FunctionInterface function_methods = {
    .destruct          = Destruct,
    .create_read_cmd   = CreateReadCmd,
    .get_function_type = GetFunctionType,
    .get_data          = GetData,
    .create_reply_cmd  = CreateReplyCmd,
    .create_notify_cmd = CreateNotifyCmd,
    .create_write_cmd  = CreateWriteCmd,
    .data_copy         = DataCopy,
    .update_data       = UpdateData,
    .get_operations    = GetOperations,
    .set_operations    = SetOperations,
};

static void FunctionConstruct(Function* self, FunctionType type);
static EebusError
AddDataToWriteCmd(const Function* self, CmdType* cmd, const void* data, const FilterType* filter_partial);
static size_t GetFiltersNum(const FilterType* filter_partial, const FilterType* filter_delete);
static EebusError AddFiltersToWriteCmd(
    const Function* self,
    CmdType* cmd,
    const FilterType* filter_partial,
    const FilterType* filter_delete
);

void FunctionConstruct(Function* self, FunctionType type) {
    // Override "virtual functions table"
    FUNCTION_INTERFACE(self) = &function_methods;

    self->type       = type;
    self->data       = ModelFunctionDataCreateEmpty(type);
    self->operations = NULL;
}

FunctionObject* FunctionCreate(FunctionType type) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(type);
    if (cfg == NULL) {
        return NULL;
    }

    Function* const function = (Function*)EEBUS_MALLOC(sizeof(Function));
    if (function == NULL) {
        return NULL;
    }

    FunctionConstruct(function, type);

    return FUNCTION_OBJECT(function);
}

void Destruct(FunctionObject* self) {
    Function* const function = FUNCTION(self);

    OperationsDelete(function->operations);
    function->operations = NULL;

    ModelFunctionDataDelete(function->type, function->data);
    function->data = NULL;
}

EebusError AddDataToReadCmd(const Function* self, CmdType* cmd) {
    const EebusDataCfg* const cfg = ModelGetDataCfg(self->type);

    cmd->data_choice_type_id = self->type;
    if (EEBUS_DATA_CREATE_EMPTY(cfg, &cmd->data_choice) == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

EebusError AddFilterToReadCmd(const Function* self, CmdType* cmd, const FilterType* filter_partial) {
    if (filter_partial == NULL) {
        return kEebusErrorOk;
    }

    const FilterType** const filter = (const FilterType**)EEBUS_MALLOC(sizeof(FilterType*));

    cmd->filter = filter;
    if (cmd->filter == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    filter[cmd->filter_size] = FilterPartialCreate(
        cmd->data_choice_type_id,
        filter_partial->filter_id,
        filter_partial->data_selectors_choice,
        filter_partial->data_elements_choice
    );

    if (filter[cmd->filter_size] == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    ++cmd->filter_size;

    cmd->function = Int32Create(self->type);
    if (cmd->function == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

CmdType* CreateReadCmd(const FunctionObject* self, const FilterType* filter_partial) {
    const Function* const function = FUNCTION(self);

    CmdType* cmd = CmdCreateEmpty();
    if (cmd == NULL) {
        return NULL;
    }

    if ((AddDataToReadCmd(function, cmd) != kEebusErrorOk)
        || (AddFilterToReadCmd(function, cmd, filter_partial) != kEebusErrorOk)) {
        CmdDelete(cmd);
        return NULL;
    }

    cmd->data_choice_type_id = function->type;
    return cmd;
}

FunctionType GetFunctionType(const FunctionObject* self) {
    const Function* const function = FUNCTION(self);
    return function->type;
}

const void* GetData(const FunctionObject* self) {
    const Function* const function = FUNCTION(self);
    return function->data;
}

CmdType* CreateReplyCmd(const FunctionObject* self) {
    const Function* const function = FUNCTION(self);

    CmdType* cmd = CmdCreateEmpty();
    if (cmd == NULL) {
        return NULL;
    }

    const EebusDataCfg* const cfg = ModelGetDataCfg(function->type);
    if (EEBUS_DATA_COPY(cfg, &function->data, &cmd->data_choice) != kEebusErrorOk) {
        CmdDelete(cmd);
        return NULL;
    }

    cmd->data_choice_type_id = function->type;
    return cmd;
}

CmdType* CreateNotifyCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
    return FUNCTION_CREATE_WRITE_CMD(self, new_data, filter_partial, filter_delete);
}

EebusError AddDataToWriteCmd(const Function* self, CmdType* cmd, const void* data, const FilterType* filter_partial) {
    UNUSED(filter_partial);
    const EebusDataCfg* const cfg = ModelGetDataCfg(self->type);

    cmd->data_choice_type_id = self->type;
    return EEBUS_DATA_COPY(cfg, &data, &cmd->data_choice);
}

size_t GetFiltersNum(const FilterType* filter_partial, const FilterType* filter_delete) {
    size_t n = 0;
    if (filter_partial != NULL) {
        ++n;
    }

    if (filter_delete != NULL) {
        ++n;
    }

    return n;
}

EebusError AddFiltersToWriteCmd(
    const Function* self,
    CmdType* cmd,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
    const size_t n = GetFiltersNum(filter_partial, filter_delete);
    if (n == 0) {
        return kEebusErrorOk;
    }

    const FilterType** const filter = (const FilterType**)EEBUS_MALLOC(sizeof(FilterType*) * n);

    cmd->filter = filter;
    if (cmd->filter == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    memset(filter, 0, sizeof(FilterType*) * n);

    if (filter_partial != NULL) {
        filter[cmd->filter_size] = FilterPartialCreate(
            cmd->data_choice_type_id,
            filter_partial->filter_id,
            filter_partial->data_selectors_choice,
            NULL
        );

        if (filter[cmd->filter_size] == NULL) {
            return kEebusErrorMemoryAllocate;
        }

        ++cmd->filter_size;
    }

    if (filter_delete != NULL) {
        filter[cmd->filter_size] = FilterDeleteCreate(
            cmd->data_choice_type_id,
            filter_delete->filter_id,
            filter_delete->data_selectors_choice,
            filter_delete->data_elements_choice
        );

        if (filter[cmd->filter_size] == NULL) {
            return kEebusErrorMemoryAllocate;
        }

        ++cmd->filter_size;
    }

    cmd->function = Int32Create(self->type);
    if (cmd->function == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    return kEebusErrorOk;
}

CmdType* CreateWriteCmd(
    const FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete
) {
    const Function* const function = FUNCTION(self);

    CmdType* cmd = CmdCreateEmpty();
    if (cmd == NULL) {
        return NULL;
    }

    if ((AddDataToWriteCmd(function, cmd, new_data, filter_partial) != kEebusErrorOk)
        || (AddFiltersToWriteCmd(function, cmd, filter_partial, filter_delete))) {
        CmdDelete(cmd);
        return NULL;
    }

    return cmd;
}

void* DataCopy(const FunctionObject* self) {
    const Function* const function = FUNCTION(self);
    return ModelFunctionDataCopy(function->type, function->data);
}

EebusError UpdateData(
    FunctionObject* self,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool wr_remote,
    bool persist
) {
    UNUSED(wr_remote);
    Function* const function = FUNCTION(self);

    const EebusDataCfg* const cfg = ModelGetDataCfg(function->type);

    if ((filter_partial == NULL) && (filter_delete == NULL) && persist) {
        EEBUS_DATA_DELETE(cfg, &function->data);
        return EEBUS_DATA_WRITE(cfg, &function->data, &new_data);
    }

    if (filter_delete != NULL) {
        const EebusDataCfg* const selectors_cfg = ModelGetDataSelectorsCfg(function->type);
        const void* const selectors             = filter_delete->data_selectors_choice;
        const EebusDataCfg* const elements_cfg  = ModelGetDataElementsCfg(function->type);
        const void* const elements              = filter_delete->data_elements_choice;

        EEBUS_DATA_DELETE_PARTIAL(cfg, &function->data, selectors_cfg, &selectors, NULL, elements_cfg, &elements);
    }

    if (filter_partial != NULL) {
        const EebusDataCfg* const selectors_cfg = ModelGetDataSelectorsCfg(function->type);
        const void* const selectors             = filter_partial->data_selectors_choice;

        return EEBUS_DATA_WRITE_PARTIAL(cfg, &function->data, &new_data, selectors_cfg, &selectors, NULL);
    }

    return kEebusErrorOk;
}

const OperationsObject* GetOperations(const FunctionObject* self) {
    return FUNCTION(self)->operations;
}

void SetOperations(FunctionObject* self, bool read, bool read_partial, bool write, bool write_partial) {
    Function* const function = FUNCTION(self);

    if (function->operations != NULL) {
        return;
    }

    function->operations = OperationsCreate(read, read_partial, write, write_partial);
}
