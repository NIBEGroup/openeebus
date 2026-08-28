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
#include "src/spine/model/measurement_types.h"
#include "src/common/eebus_data/eebus_data_list.h"
#include "src/common/num_ptr.h"
#include "src/spine/model/function_types.h"
#include "src/spine/model/model.h"

MeasurementListDataType* MeasurementsCreateEmpty(void) {
    return (MeasurementListDataType*)ModelFunctionDataCreateEmpty(kFunctionTypeMeasurementListData);
}

void MeasurementsDelete(MeasurementListDataType* measurement_data_list) {
    ModelFunctionDataDelete(kFunctionTypeMeasurementListData, measurement_data_list);
}

EebusError MeasurementsAdd(MeasurementListDataType* measurement_data_list, MeasurementDataType* measurement_data) {
    if ((measurement_data_list == NULL) || (measurement_data == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    MeasurementDataType* const measurement_data_copy = MeasurementDataCopy(measurement_data);

    EebusError err = EebusDataListDataAppend(
        (void***)&measurement_data_list->measurement_data,
        &measurement_data_list->measurement_data_size,
        measurement_data_copy
    );

    if (err != kEebusErrorOk) {
        MeasurementDataDelete(measurement_data_copy);
    }

    return err;
}

MeasurementDataType* MeasurementDataCopy(const MeasurementDataType* measurement_data) {
    return (MeasurementDataType*)ModelDataListElementCopy(kFunctionTypeMeasurementListData, measurement_data);
}

void MeasurementDataDelete(MeasurementDataType* measurement_data) {
    ModelDataListElementDelete(kFunctionTypeMeasurementListData, measurement_data);
}

MeasurementConstraintsListDataType* MeasurementConstraintsCreateEmpty(void) {
    return (MeasurementConstraintsListDataType*)ModelFunctionDataCreateEmpty(
        kFunctionTypeMeasurementConstraintsListData
    );
}

void MeasurementConstraintsDelete(MeasurementConstraintsListDataType* measurement_constraints) {
    ModelFunctionDataDelete(kFunctionTypeMeasurementConstraintsListData, measurement_constraints);
}

EebusError MeasurementConstraintsAdd(
    MeasurementConstraintsListDataType* measurements_constraints,
    const MeasurementConstraintsDataType* constraints_data
) {
    if ((measurements_constraints == NULL) || (constraints_data == NULL)) {
        return kEebusErrorInputArgumentNull;
    }

    MeasurementConstraintsDataType* const constraints_data_copy = MeasurementConstraintsDataCopy(constraints_data);

    EebusError err = EebusDataListDataAppend(
        (void***)&measurements_constraints->measurement_constraints_data,
        &measurements_constraints->measurement_constraints_data_size,
        constraints_data_copy
    );

    if (err != kEebusErrorOk) {
        MeasurementConstraintsDataDelete(constraints_data_copy);
    }

    return err;
}

MeasurementConstraintsDataType* MeasurementConstraintsDataCopy(const MeasurementConstraintsDataType* constraints_data) {
    return (
        MeasurementConstraintsDataType*
    )ModelDataListElementCopy(kFunctionTypeMeasurementConstraintsListData, constraints_data);
}

void MeasurementConstraintsDataDelete(MeasurementConstraintsDataType* constraints_data) {
    ModelDataListElementDelete(kFunctionTypeMeasurementConstraintsListData, constraints_data);
}

EebusError MeasurementConstraintsSetId(MeasurementConstraintsDataType* constraints_data, MeasurementIdType id) {
    if (constraints_data == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    if (constraints_data->measurement_id != NULL) {
        EEBUS_FREE((void*)constraints_data->measurement_id);
    }

    constraints_data->measurement_id = Uint32Create(id);
    return (constraints_data->measurement_id != NULL) ? kEebusErrorOk : kEebusErrorMemoryAllocate;
}
