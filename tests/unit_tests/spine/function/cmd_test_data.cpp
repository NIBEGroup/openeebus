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
 * @brief Command Json print/parse functions for test purpose implementation
 */

#include "tests/unit_tests/spine/function/cmd_test_data.h"

#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/command_frame_types.h"
#include "src/spine/model/model.h"

#define CMD_TEST_EEBUS_DATA_CFG EEBUS_DATA_SEQUENCE_ROOT("cmd", FilterType, ModelGetCmdCfg()->metadata)

CmdType* CmdParse(const char* s) {
  const EebusDataCfg cfg = CMD_TEST_EEBUS_DATA_CFG;
  return (CmdType*)EEBUS_DATA_PARSE(&cfg, s);
}

char* CmdPrintUnformatted(const CmdType* self) {
  const EebusDataCfg cfg = CMD_TEST_EEBUS_DATA_CFG;
  return EEBUS_DATA_PRINT_UNFORMATTED(&cfg, &self);
}
