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
 * @brief Pending Reply implementation
 */

#include "src/spine/feature/pending_reply.h"

#include "src/common/api/eebus_timer_interface.h"
#include "src/common/eebus_arguments.h"
#include "src/common/eebus_countdown/eebus_countdown.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"
#include "src/spine/feature/feature.h"

typedef struct PendingReply PendingReply;

struct PendingReply {
    /** Implements the Pending Reply Interface */
    PendingReplyObject obj;

    MsgCounterType msg_cnt_ref;
    ReplyMessageCallback cb;
    void* ctx;
    FeatureAddressType* remote_feature_addr;
    FunctionType function_type;
    char* ski;
    EebusCountdown countdown;
};

#define PENDING_REPLY(obj) ((PendingReply*)(obj))

static void Destruct(PendingReplyObject* self);
static MsgCounterType GetMsgCntRef(const PendingReplyObject* self);
static const FeatureAddressType* GetRemoteFeatureAddress(const PendingReplyObject* self);
static const char* GetSki(const PendingReplyObject* self);
static bool HasExpired(const PendingReplyObject* self);
static void UpdateTime(PendingReplyObject* self);
static void Fire(const PendingReplyObject* self, const ReplyMessage* reply_msg, EebusError err);

static const PendingReplyInterface pending_reply_methods = {
    .destruct                   = Destruct,
    .get_msg_cnt_ref            = GetMsgCntRef,
    .get_remote_feature_address = GetRemoteFeatureAddress,
    .get_ski                    = GetSki,
    .has_expired                = HasExpired,
    .update_time                = UpdateTime,
    .fire                       = Fire,
};

static void PendingReplyConstruct(
    PendingReply* self,
    MsgCounterType msg_cnt_ref,
    const FeatureAddressType* remote_feature_address,
    FunctionType function_type,
    const char* ski,
    ReplyMessageCallback cb,
    void* ctx
);

void PendingReplyConstruct(
    PendingReply* self,
    MsgCounterType msg_cnt_ref,
    const FeatureAddressType* remote_feature_address,
    FunctionType function_type,
    const char* ski,
    ReplyMessageCallback cb,
    void* ctx
) {
    // Override "virtual functions table"
    PENDING_REPLY_INTERFACE(self) = &pending_reply_methods;

    self->msg_cnt_ref         = msg_cnt_ref;
    self->remote_feature_addr = FeatureAddressCopy(remote_feature_address);
    self->function_type       = function_type;
    self->ski                 = StringCopy(ski);
    self->cb                  = cb;
    self->ctx                 = ctx;
    self->countdown           = EEBUS_COUNTDOWN(TIME_MS_TO_S(kDefaultMaxResponseDelayMs));
}

PendingReplyObject* PendingReplyCreate(
    MsgCounterType msg_cnt_ref,
    const FeatureAddressType* remote_feature_address,
    FunctionType function_type,
    const char* ski,
    ReplyMessageCallback cb,
    void* ctx
) {
    PendingReply* const self = (PendingReply*)EEBUS_MALLOC(sizeof(PendingReply));
    if (self == NULL) {
        return NULL;
    }

    PendingReplyConstruct(self, msg_cnt_ref, remote_feature_address, function_type, ski, cb, ctx);
    return PENDING_REPLY_OBJECT(self);
}

void Destruct(PendingReplyObject* self) {
    PendingReply* const pr = PENDING_REPLY(self);
    FeatureAddressDelete(pr->remote_feature_addr);
    EEBUS_FREE(pr->ski);
}

MsgCounterType GetMsgCntRef(const PendingReplyObject* self) {
    return PENDING_REPLY(self)->msg_cnt_ref;
}

const FeatureAddressType* GetRemoteFeatureAddress(const PendingReplyObject* self) {
    return PENDING_REPLY(self)->remote_feature_addr;
}

const char* GetSki(const PendingReplyObject* self) {
    return PENDING_REPLY(self)->ski;
}

bool HasExpired(const PendingReplyObject* self) {
    return EebusCountdownHasExpired(&PENDING_REPLY(self)->countdown);
}

void UpdateTime(PendingReplyObject* self) {
    EebusCountdownTick(&PENDING_REPLY(self)->countdown);
}

void Fire(const PendingReplyObject* self, const ReplyMessage* reply_msg, EebusError err) {
    PendingReply* const pr = PENDING_REPLY(self);
    if (err == kEebusErrorOk) {
        pr->cb(reply_msg, pr->remote_feature_addr, err, pr->ctx);
    } else {
        const ReplyMessage timeout_msg
            = {.msg_cnt_ref = pr->msg_cnt_ref, .ski = pr->ski, .function_type = pr->function_type};
        pr->cb(&timeout_msg, pr->remote_feature_addr, err, pr->ctx);
    }
}
