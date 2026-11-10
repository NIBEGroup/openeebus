#ifndef TESTS_UNIT_TESTS_COMMON_DATA_EMPLOYEE_DATA_INC_
#define TESTS_UNIT_TESTS_COMMON_DATA_EMPLOYEE_DATA_INC_

#include "src/common/eebus_data/eebus_data.h"
#include "tests/unit_tests/common/eebus_data/employee.h"

static const EnumMapping postions[] = {
    {"developer", kPositionDeveloper},
    {"manager", kPositionManager},
    {"tester", kPositionTester},
    {NULL},
};

static const EebusDataCfg report_data_cfg = EEBUS_DATA_LIST_ELEMENT(numeric, sizeof(uint8_t), &json_num_conv_int8);

static const EebusDataCfg employee_sequence_data_cfg[] = {
    EEBUS_DATA_STRING("name", Employee, name),
    EEBUS_DATA_STRING("surname", Employee, surname),
    EEBUS_DATA_UINT32("id", Employee, id),
    EEBUS_DATA_UINT8("age", Employee, age),
    EEBUS_DATA_ENUM("position", Employee, position, postions),
    EEBUS_DATA_UINT32("salary", Employee, salary),
    EEBUS_DATA_TAG("married", Employee, is_married),
    EEBUS_DATA_BOOL("mastersDegree", Employee, has_masters_degree),
    EEBUS_DATA_LIST("report", Employee, report, &report_data_cfg),
    EEBUS_DATA_END,
};

static const EebusDataCfg employee_sequence_elements_cfg[] = {
    EEBUS_DATA_TAG("name", EmployeeElements, name),
    EEBUS_DATA_TAG("surname", EmployeeElements, surname),
    EEBUS_DATA_TAG("id", EmployeeElements, id),
    EEBUS_DATA_TAG("age", EmployeeElements, age),
    EEBUS_DATA_TAG("position", EmployeeElements, position),
    EEBUS_DATA_TAG("salary", EmployeeElements, salary),
    EEBUS_DATA_TAG("married", EmployeeElements, is_married),
    EEBUS_DATA_TAG("mastersDegree", EmployeeElements, has_masters_degree),
    EEBUS_DATA_TAG("report", EmployeeElements, report),
    EEBUS_DATA_END,
};

static const EebusDataCfg employee_data_cfg
    = EEBUS_DATA_SEQUENCE_ROOT("employee", Employee, employee_sequence_data_cfg);

static const EebusDataCfg employee_elements_cfg
    = EEBUS_DATA_SEQUENCE_ROOT("employeeElements", EmployeeElements, employee_sequence_elements_cfg);

#endif  // TESTS_UNIT_TESTS_COMMON_DATA_EMPLOYEE_DATA_INC_
