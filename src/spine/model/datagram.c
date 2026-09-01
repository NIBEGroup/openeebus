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
 * @brief SPINE Datagram functions implementation
 */

#include "src/spine/model/datagram.h"

#include "src/common/eebus_arguments.h"
#include "src/common/eebus_data/eebus_data.h"
#include "src/spine/model/feature_types.h"
#include "src/spine/model/model.h"

bool DatagramHeaderIsValid(const HeaderType* header) {
    if (header == NULL) {
        return false;
    }

    bool ok = (header->spec_version != NULL);

    ok = ok && FeatureAddressIsValid(header->src_addr);
    ok = ok && FeatureAddressIsValid(header->dest_addr);
    if (ok && (header->originator_addr != NULL)) {
        ok = ok && FeatureAddressIsValid(header->originator_addr);
        ok = ok && (header->originator_addr->device != NULL);
    }

    ok = ok && (header->msg_cnt != NULL);
    ok = ok && (header->cmd_classifier != NULL);

    return ok;
}

bool DatagramPayloadIsValid(const PayloadType* payload) {
    UNUSED(payload);
    // TODO: Implement function
    return true;
}

bool DatagramIsValid(const DatagramType* datagram) {
    if (datagram == NULL) {
        return false;
    }

    return DatagramHeaderIsValid(datagram->header) && DatagramPayloadIsValid(datagram->payload);
}

void DatagramDelete(DatagramType* datagram) {
    EEBUS_DATA_DELETE(ModelGetDatagramCfg(), &datagram);
}

DatagramType* DatagramParse(const char* s) {
    return (DatagramType*)EEBUS_DATA_PARSE(ModelGetDatagramCfg(), s);
}

char* DatagramPrintUnformatted(const DatagramType* datagram) {
    return EEBUS_DATA_PRINT_UNFORMATTED(ModelGetDatagramCfg(), &datagram);
}

DatagramType* DatagramCopy(const DatagramType* datagram) {
    DatagramType* datagram_copy = NULL;
    EEBUS_DATA_COPY(ModelGetDatagramCfg(), &datagram, &datagram_copy);
    return datagram_copy;
}

bool DatagramHeaderCompare(const HeaderType* header_a, const HeaderType* header_b) {
    const EebusDataCfg cfg = EEBUS_DATA_SEQUENCE_TMP(HeaderType, ModelGetDatagramHeaderCfg());
    return EEBUS_DATA_COMPARE(&cfg, &header_a, &cfg, &header_b);
}
