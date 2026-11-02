#ifndef TESTS_UNIT_TESTS_COMMON_DATA_INFO_DATA_INC_
#define TESTS_UNIT_TESTS_COMMON_DATA_INFO_DATA_INC_

#include "src/common/eebus_data/eebus_data.h"
#include "tests/unit_tests/common/eebus_data/info.h"
#include "tests/unit_tests/common/eebus_data/somebody_json.inc"

static const EebusDataCfg info_choice_data_cfg[] = {
    EEBUS_DATA_CHOICE_ELEMENT_EMPTY("padding"),
    EEBUS_DATA_CHOICE_ELEMENT(sequence, "employee", Employee, employee_sequence_data_cfg),
    EEBUS_DATA_CHOICE_ELEMENT(tag, "tag", InfoTag, NULL),
    EEBUS_DATA_CHOICE_ELEMENT(sequence, "person", Person, person_sequence_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg info_sequence_data_cfg[] = {
    EEBUS_DATA_CHOICE(Info, data, info_choice_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg info_data_cfg = EEBUS_DATA_SEQUENCE_ROOT("info", Info, info_sequence_data_cfg);

#endif  // TESTS_UNIT_TESTS_COMMON_DATA_INFO_DATA_INC_
