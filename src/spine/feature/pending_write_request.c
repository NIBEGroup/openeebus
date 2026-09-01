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
 * @brief Pending Write Request implementation
 */

#include "src/spine/feature/pending_write_request.h"
#include "src/common/api/eebus_data_interface.h"
#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_countdown/eebus_countdown.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/spine/api/device_local_interface.h"
#include "src/spine/api/pending_write_request_interface.h"
#include "src/spine/feature/feature.h"
#include "src/spine/model/cmd.h"
#include "src/spine/model/model.h"

typedef struct PendingWriteRequest PendingWriteRequest;

struct PendingWriteRequest {
    /** Implements the Pending Write Request Interface */
    PendingWriteRequestObject obj;

    const char* ski;
    uint64_t msg_counter;
    uint64_t num_approvals;
    EebusCountdown countdown;

    CommandClassifierType cmd_classifier;
    CmdType* cmd;
    HeaderType* header;
    FeatureAddressType* feature_address;
};

#define PENDING_WRITE_REQUEST(obj) ((PendingWriteRequest*)(obj))

static void Destruct(PendingWriteRequestObject* self);
static const char* GetSki(PendingWriteRequestObject* self);
static uint64_t GetMessageCounter(PendingWriteRequestObject* self);
static EebusError GetMessage(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg);
static uint64_t GetNumberOfApprovals(PendingWriteRequestObject* self);
static void AddApproval(PendingWriteRequestObject* self);
static uint32_t GetRemainingTime(PendingWriteRequestObject* self);
static void UpdateRemainingTime(PendingWriteRequestObject* self);
static bool HasExpired(PendingWriteRequestObject* self);

static const PendingWriteRequestInterface pending_write_request_methods = {
    .destruct                = Destruct,
    .get_ski                 = GetSki,
    .get_message_counter     = GetMessageCounter,
    .get_message             = GetMessage,
    .get_number_of_approvals = GetNumberOfApprovals,
    .add_approval            = AddApproval,
    .get_remaining_time      = GetRemainingTime,
    .update_remaining_time   = UpdateRemainingTime,
    .has_expired             = HasExpired,
};

static EebusError PendingWriteRequestCopyMessageInfo(PendingWriteRequest* self, const Message* msg);

static EebusError PendingWriteRequestConstruct(PendingWriteRequest* self, const Message* msg);

EebusError PendingWriteRequestCopyMessageInfo(PendingWriteRequest* self, const Message* msg) {
    EebusError err = EEBUS_DATA_COPY(ModelGetHeaderCfg(), &msg->request_header, &self->header);
    if ((self->header == NULL) || (err != kEebusErrorOk)) {
        return kEebusErrorInit;
    }

    self->cmd_classifier = msg->cmd_classifier;

    self->cmd = CmdCopy(msg->cmd);
    if (self->cmd == NULL) {
        return kEebusErrorInit;
    }

    self->feature_address = FeatureAddressCopy(FEATURE_GET_ADDRESS(FEATURE_OBJECT(msg->feature_remote)));
    if (self->feature_address == NULL) {
        return kEebusErrorInit;
    }

    return kEebusErrorOk;
}

EebusError PendingWriteRequestConstruct(PendingWriteRequest* self, const Message* msg) {
    // Override "virtual functions table"
    PENDING_WRITE_REQUEST_INTERFACE(self) = &pending_write_request_methods;

    self->ski             = NULL;
    self->msg_counter     = 0;
    self->num_approvals   = 0;
    self->cmd_classifier  = (CommandClassifierType)0;
    self->countdown       = EEBUS_COUNTDOWN(TIME_MS_TO_S(kDefaultMaxResponseDelayMs));
    self->cmd             = NULL;
    self->header          = NULL;
    self->feature_address = NULL;

    // Initialize member variables
    self->ski = StringCopy(DEVICE_REMOTE_GET_SKI(msg->device_remote));
    if (self->ski == NULL) {
        return kEebusErrorInit;
    }

    if (msg->request_header->msg_cnt != NULL) {
        self->msg_counter = *msg->request_header->msg_cnt;
    } else {
        return kEebusErrorInit;
    }

    const EebusError err = PendingWriteRequestCopyMessageInfo(self, msg);
    if (err != kEebusErrorOk) {
        return err;
    }

    return kEebusErrorOk;
}

PendingWriteRequestObject* PendingWriteRequestCreate(const Message* msg) {
    PendingWriteRequest* const pending_write_request = (PendingWriteRequest*)EEBUS_MALLOC(sizeof(PendingWriteRequest));
    if (pending_write_request == NULL) {
        return NULL;
    }

    const EebusError err = PendingWriteRequestConstruct(pending_write_request, msg);
    if (err != kEebusErrorOk) {
        PendingWriteRequestDelete(PENDING_WRITE_REQUEST_OBJECT(pending_write_request));
        return NULL;
    }

    return PENDING_WRITE_REQUEST_OBJECT(pending_write_request);
}

void Destruct(PendingWriteRequestObject* self) {
    PendingWriteRequest* pwr = PENDING_WRITE_REQUEST(self);

    StringDelete((char*)pwr->ski);
    pwr->ski = NULL;

    ModelDataDelete(ModelGetHeaderCfg(), pwr->header);
    pwr->header = NULL;

    CmdDelete(pwr->cmd);
    pwr->cmd = NULL;

    FeatureAddressDelete(pwr->feature_address);
    pwr->feature_address = NULL;
}

const char* GetSki(PendingWriteRequestObject* self) {
    PendingWriteRequest* pwr = PENDING_WRITE_REQUEST(self);
    return pwr->ski;
}

uint64_t GetMessageCounter(PendingWriteRequestObject* self) {
    PendingWriteRequest* pwr = PENDING_WRITE_REQUEST(self);
    return pwr->msg_counter;
}

EebusError GetMessage(PendingWriteRequestObject* self, FeatureLocalObject* fl, Message* msg) {
    PendingWriteRequest* const pwr = PENDING_WRITE_REQUEST(self);

    if (fl == NULL || msg == NULL) {
        return kEebusErrorInputArgumentNull;
    }

    DeviceLocalObject* dl = FEATURE_LOCAL_GET_DEVICE(fl);
    if (dl == NULL) {
        return kEebusErrorNotAvailable;
    }

    DeviceRemoteObject* dr = DEVICE_LOCAL_GET_REMOTE_DEVICE_WITH_ADDRESS(dl, pwr->feature_address->device);
    if (dr == NULL) {
        return kEebusErrorNotAvailable;
    }

    FeatureRemoteObject* fr = DEVICE_REMOTE_GET_FEATURE_WITH_ADDRESS(dr, pwr->feature_address);
    if (fr == NULL) {
        return kEebusErrorNotAvailable;
    }

    EntityRemoteObject* er = FEATURE_REMOTE_GET_ENTITY(fr);
    if (er == NULL) {
        return kEebusErrorNotAvailable;
    }

    *msg = (Message){
        .request_header = pwr->header,
        .cmd_classifier = pwr->cmd_classifier,
        .cmd            = pwr->cmd,
        .filter_partial = CmdGetFilterPartial(pwr->cmd),
        .filter_delete  = CmdGetFilterDelete(pwr->cmd),
        .feature_remote = fr,
        .entity_remote  = er,
        .device_remote  = dr,
    };

    return kEebusErrorOk;
}

uint64_t GetNumberOfApprovals(PendingWriteRequestObject* self) {
    PendingWriteRequest* pwr = PENDING_WRITE_REQUEST(self);
    return pwr->num_approvals;
}

void AddApproval(PendingWriteRequestObject* self) {
    PendingWriteRequest* pwr = PENDING_WRITE_REQUEST(self);
    pwr->num_approvals++;
}

uint32_t GetRemainingTime(PendingWriteRequestObject* self) {
    return EebusCountdownGetRemaining(&PENDING_WRITE_REQUEST(self)->countdown);
}

void UpdateRemainingTime(PendingWriteRequestObject* self) {
    EebusCountdownTick(&PENDING_WRITE_REQUEST(self)->countdown);
}

bool HasExpired(PendingWriteRequestObject* self) {
    return EebusCountdownHasExpired(&PENDING_WRITE_REQUEST(self)->countdown);
}
