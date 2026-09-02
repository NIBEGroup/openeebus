/**
 * @file
 * @brief EG LP Listener mock implementation
 */

#include "eg_lp_listener_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_arguments.h"
#include "src/use_case/api/eg_lp_listener_interface.h"

static void Destruct(EgLpListenerObject* self);
static void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr);
static void OnPowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
);
static void
OnFailsafeDurationReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, const DurationType* duration);
static void
OnHeartbeatReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, uint64_t heartbeat_counter);
static void OnPowerNominalMaxReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
);

static const EgLpListenerInterface eg_lp_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_cs_added              = OnRemoteCsAdded,
    .on_remote_cs_removed            = OnRemoteCsRemoved,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
    .on_power_nominal_max_receive    = OnPowerNominalMaxReceive,
};

static EebusError EgLpListenerMockConstruct(EgLpListenerMock* self);

EebusError EgLpListenerMockConstruct(EgLpListenerMock* self) {
  // Override "virtual functions table"
  EG_LP_LISTENER_INTERFACE(self) = &eg_lp_listener_methods;

  self->gmock = new EgLpListenerGMock();
  if (self->gmock == nullptr) {
    return kEebusErrorMemoryAllocate;
  }

  return kEebusErrorOk;
}

EgLpListenerMock* EgLpListenerMockCreate(void) {
  EgLpListenerMock* const mock = (EgLpListenerMock*)EEBUS_MALLOC(sizeof(EgLpListenerMock));
  if (mock == nullptr) {
    return nullptr;
  }

  if (EgLpListenerMockConstruct(mock) != kEebusErrorOk) {
    EgLpListenerMockDelete(mock);
    return nullptr;
  }

  return mock;
}

void Destruct(EgLpListenerObject* self) {
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteCsAdded(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCsAdded(self, entity_addr);
}

void OnRemoteCsRemoved(EgLpListenerObject* self, const EntityAddressType* entity_addr) {
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnRemoteCsRemoved(self, entity_addr);
}

void OnPowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
) {
  UNUSED(entity_addr);
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnPowerLimitReceive(self, power_limit, duration, is_active);
}

void OnFailsafePowerLimitReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
) {
  UNUSED(entity_addr);
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnFailsafePowerLimitReceive(self, power_limit);
}

void OnFailsafeDurationReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const DurationType* duration
) {
  UNUSED(entity_addr);
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnFailsafeDurationReceive(self, duration);
}

void OnHeartbeatReceive(EgLpListenerObject* self, const EntityAddressType* entity_addr, uint64_t heartbeat_counter) {
  UNUSED(entity_addr);
  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnHeartbeatReceive(self, heartbeat_counter);
}

void OnPowerNominalMaxReceive(
    EgLpListenerObject* self,
    const EntityAddressType* entity_addr,
    const ScaledValue* power_limit
) {
  UNUSED(entity_addr);

  EgLpListenerMock* const mock = EG_LP_LISTENER_MOCK(self);
  mock->gmock->OnPowerNominalMaxReceive(self, power_limit);
}
