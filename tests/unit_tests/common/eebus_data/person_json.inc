#ifndef TESTS_UNIT_TESTS_COMMON_DATA_PERSON_DATA_INC_
#define TESTS_UNIT_TESTS_COMMON_DATA_PERSON_DATA_INC_

#include "person.h"
#include "src/common/eebus_data/eebus_data.h"

static const EebusDataCfg address_data_cfg[] = {
    EEBUS_DATA_STRING("street", Address, street),
    EEBUS_DATA_STRING("city", Address, city),
    EEBUS_DATA_END,
};

static const EebusDataCfg address_elements_cfg[] = {
    EEBUS_DATA_TAG("street", AddressElements, street),
    EEBUS_DATA_TAG("city", AddressElements, city),
    EEBUS_DATA_END,
};

static const EebusDataCfg person_sequence_data_cfg[] = {
    EEBUS_DATA_STRING("name", Person, name),
    EEBUS_DATA_UINT8("age", Person, age),
    EEBUS_DATA_SEQUENCE("address", Person, address, address_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg person_sequence_elements_cfg[] = {
    EEBUS_DATA_TAG("name", PersonElements, name),
    EEBUS_DATA_TAG("age", PersonElements, age),
    EEBUS_DATA_SEQUENCE("address", PersonElements, address, address_elements_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg person_data_cfg = EEBUS_DATA_SEQUENCE_ROOT("person", Person, person_sequence_data_cfg);

static const EebusDataCfg person_elements_cfg
    = EEBUS_DATA_SEQUENCE_ROOT("personElements", PersonElements, person_sequence_elements_cfg);

#endif  // TESTS_UNIT_TESTS_COMMON_DATA_PERSON_DATA_INC_
