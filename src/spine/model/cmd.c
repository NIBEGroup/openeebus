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
 * @brief SPINE Datagram Command helper subroutines implementation
 */

#include "src/common/api/eebus_data_interface.h"
#include "src/common/eebus_malloc.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/filter.h"
#include "src/spine/model/model.h"

CmdType* CmdCreateEmpty(void) {
    const EebusDataCfg* const cfg = ModelGetCmdCfg();
    return ModelDataCreateEmpty(cfg);
}

void CmdDelete(CmdType* cmd) {
    if (cmd == NULL) {
        return;
    }
    const EebusDataCfg* const cfg = ModelGetCmdCfg();
    EEBUS_DATA_DELETE(cfg, &cmd);
}

CmdType* CmdCopy(const CmdType* cmd) {
    const EebusDataCfg* const cfg = ModelGetCmdCfg();

    CmdType* cmd_copy = NULL;

    const EebusError err = EEBUS_DATA_COPY(cfg, &cmd, &cmd_copy);

    if (err != kEebusErrorOk) {
        CmdDelete(cmd_copy);
        return NULL;
    }

    return cmd_copy;
}

const FilterType* CmdGetFilterWithType(const CmdType* cmd, FilterTypeType filter_type) {
    if ((cmd == NULL) || (cmd->filter == NULL) || (cmd->filter_size == 0)) {
        return NULL;
    }

    for (size_t i = 0; i < cmd->filter_size; ++i) {
        if (FilterGetType(cmd->filter[i]) == filter_type) {
            return cmd->filter[i];
        }
    }

    return NULL;
}

const FilterType* CmdGetFilterPartial(const CmdType* cmd) {
    return CmdGetFilterWithType(cmd, kFilterTypePartial);
}

const FilterType* CmdGetFilterDelete(const CmdType* cmd) {
    return CmdGetFilterWithType(cmd, kFilterTypeDelete);
}

EebusError CmdAddFilterPartialEmpty(CmdType* cmd) {
    cmd->filter_size                = 0;
    const FilterType** const filter = (const FilterType**)EEBUS_MALLOC(sizeof(cmd->filter[0]));

    cmd->filter = filter;
    if (cmd->filter == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    filter[0] = FilterPartialCreate(0, NULL, NULL, NULL);
    if (filter[0] == NULL) {
        return kEebusErrorMemoryAllocate;
    }

    cmd->filter_size++;
    return kEebusErrorOk;
}
