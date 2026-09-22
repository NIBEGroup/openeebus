# Migration Guide

This document describes every breaking API change introduced in this PR and
the corresponding adjustments an integrator must make in their own code.

## Migration plan

| # | What to do |
|---|---|
| 1 | Rename `EgLpListener.on_remote_entity_connect/disconnect` → `on_remote_cs_added/removed` |
| 2 | Rename `EgLpcReadActiveProductionPowerLimit` → `EgLpcReadActiveConsumptionPowerLimit` |
| 3 | Add `cb, ctx` to all `EgLpc*Set*` and `EgLpp*Set*` calls |
| 4 | Add `on_remote_eg_added` / `on_remote_eg_removed` to `CsLpListenerInterface` |
| 5 | Rename `MaMpcListener.on_remote_entity_connect/disconnect` → `on_remote_mu_added/removed` |
| 6 | Pass fourth `listener` argument (or `NULL`) to `MuMpcUseCaseCreate` |
| 7 | Add `entity_addr` parameter to `CemOhpcfListener.on_announce/on_state_report/on_clear_process` |

---

## 1. EG LP listener (`EgLpListenerInterface`)

### What changed

| Before | After |
|---|---|
| `on_remote_entity_connect` | `on_remote_cs_added` |
| `on_remote_entity_disconnect` | `on_remote_cs_removed` |
| — | `on_power_nominal_max_receive` *(new)* |

Macro renames:

```c
// Before
EG_LP_LISTENER_ON_REMOTE_ENTITY_CONNECT(obj, entity_addr)
EG_LP_LISTENER_ON_REMOTE_ENTITY_DISCONNECT(obj, entity_addr)

// After
EG_LP_LISTENER_ON_REMOTE_CS_ADDED(obj, entity_addr)
EG_LP_LISTENER_ON_REMOTE_CS_REMOVED(obj, entity_addr)
EG_LP_LISTENER_ON_POWER_NOMINAL_MAX_RECEIVE(obj, entity_addr, power_limit)  // new
```

New callback signature:

```c
void on_power_nominal_max_receive(
    EgLpListenerObject*       self,
    const EntityAddressType*  entity_addr,
    const ScaledValue*        power_limit
);
```

### What to update

1. In your `EgLpListenerInterface` struct literal, rename the function pointer
   fields: `on_remote_entity_connect` → `on_remote_cs_added`,
   `on_remote_entity_disconnect` → `on_remote_cs_removed`.

2. Rename the pointed-to functions in your implementation accordingly.

3. Optionally add `on_power_nominal_max_receive` if your application needs the
   nominal max power value (set to `NULL` otherwise).

---

## 2. EG LP public API — renamed and new read functions

### What changed

| Before | After |
|---|---|
| `EgLpcReadActiveProductionPowerLimit(...)` | `EgLpcReadActiveConsumptionPowerLimit(...)` |
| `EgLppReadActiveProductionPowerLimit(...)` | `EgLppReadActiveProductionPowerLimit(...)` *(unchanged)* |
| — | `EgLpcReadFailsafeConsumptionActivePowerLimit(...)` *(new)* |
| — | `EgLppReadFailsafeProductionActivePowerLimit(...)` *(new)* |
| — | `EgLpcReadFailsafeDurationMinimum(...)` *(new)* |
| — | `EgLppReadFailsafeDurationMinimum(...)` *(new)* |
| — | `EgLpcReadPowerConsumptionNominalMax(...)` *(new)* |
| — | `EgLppReadPowerProductionNominalMax(...)` *(new)* |

All `Set*` functions gained `ResultMessageCallback cb` and `void* ctx`
parameters:

```c
// Before
EebusError EgLpcSetActiveConsumptionPowerLimit(
    EgLpUseCaseObject* self,
    const EntityAddressType* remote_entity_addr,
    const ScaledValue* load_limit
);

// After
EebusError EgLpcSetActiveConsumptionPowerLimit(
    EgLpUseCaseObject*          self,
    const EntityAddressType*    remote_entity_addr,
    const ScaledValue*          load_limit,
    ResultMessageCallback       cb,
    void*                       ctx
);
```

This applies to all `EgLpc*` and `EgLpp*` set functions
(`SetActiveConsumptionPowerLimit`, `SetActiveProductionPowerLimit`,
`SetFailsafeConsumptionActivePowerLimit`, `SetFailsafeProductionActivePowerLimit`,
`SetFailsafeDurationMinimum`).

### What to update

1. Rename `EgLpcReadActiveProductionPowerLimit` to
   `EgLpcReadActiveConsumptionPowerLimit` at every call site.

2. Add `cb, ctx` (or `NULL, NULL`) to every `EgLpc*Set*` and `EgLpp*Set*`
   call.

---

## 3. CS LP listener (`CsLpListenerInterface`)

### What changed

Two new methods were added:

```c
void on_remote_eg_added(CsLpListenerObject* self, const EntityAddressType* entity_addr);
void on_remote_eg_removed(CsLpListenerObject* self, const EntityAddressType* entity_addr);
```

Corresponding macros:

```c
CS_LP_LISTENER_ON_REMOTE_EG_ADDED(obj, entity_addr)
CS_LP_LISTENER_ON_REMOTE_EG_REMOVED(obj, entity_addr)
```

### What to update

Add `on_remote_eg_added` and `on_remote_eg_removed` to your
`CsLpListenerInterface` struct literal. These can be `NULL` if your
application does not need to react to EG entity lifecycle events.

---

## 4. MA MPC listener (`MaMpcListenerInterface`)

### What changed

| Before | After |
|---|---|
| `on_remote_entity_connect` | `on_remote_mu_added` |
| `on_remote_entity_disconnect` | `on_remote_mu_removed` |

Macro renames:

```c
MA_MPC_LISTENER_ON_REMOTE_ENTITY_CONNECT(obj, entity_addr)
MA_MPC_LISTENER_ON_REMOTE_ENTITY_DISCONNECT(obj, entity_addr)
// →
MA_MPC_LISTENER_ON_REMOTE_MU_ADDED(obj, entity_addr)
MA_MPC_LISTENER_ON_REMOTE_MU_REMOVED(obj, entity_addr)
```

### What to update

Rename the function pointer fields and the pointed-to functions as above.

---

## 5. MA MPC public API — new `ReadMeasurementsData`

### What changed

A new function allows the MA actor to explicitly request measurement data from
a remote MU entity:

```c
EebusError MaMpcReadMeasurementsData(
    const MaMpcUseCaseObject*   self,
    const EntityAddressType*    remote_entity_addr,
    ReplyMessageCallback        cb,
    void*                       ctx
);
```

### What to update

No existing call sites break. Optionally use this function to trigger a
measurement read on demand instead of waiting for the next notify.

---

## 6. MU MPC — new `MuMpcListenerInterface` and `MuMpcUseCaseCreate` signature

### What changed

`MuMpcUseCaseCreate` gained a new `listener` parameter:

```c
// Before
MuMpcUseCaseObject* MuMpcUseCaseCreate(
    EntityLocalObject*      local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig*      cfg
);

// After
MuMpcUseCaseObject* MuMpcUseCaseCreate(
    EntityLocalObject*      local_entity,
    ElectricalConnectionIdType ec_id,
    const MuMpcConfig*      cfg,
    MuMpcListenerObject*    listener   // NEW — may be NULL
);
```

The new `MuMpcListenerInterface` allows the application to receive
notifications when a remote MA entity connects or disconnects:

```c
struct MuMpcListenerInterface {
  void (*destruct)(MuMpcListenerObject* self);
  void (*on_remote_ma_added)(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
  void (*on_remote_ma_removed)(MuMpcListenerObject* self, const EntityAddressType* entity_addr);
};
```

### What to update

1. Update every `MuMpcUseCaseCreate` call to pass a fourth argument. Pass
   `NULL` to keep the existing behaviour.

2. Optionally implement `MuMpcListenerInterface` if your application needs to
   react to MA entity lifecycle events.

---

## 7. CEM OHPCF listener — `entity_addr` added to callbacks

### What changed

All three action callbacks now receive the remote compressor entity address
as their last argument:

```c
// Before
void on_announce(CemOhpcfListenerObject*, const OptionalPowerConsumption*);
void on_state_report(CemOhpcfListenerObject*, CompressorOhpcfState, const EebusDuration*);
void on_clear_process(CemOhpcfListenerObject*);

// After
void on_announce(CemOhpcfListenerObject*, const OptionalPowerConsumption*,
                 const EntityAddressType* entity_addr);
void on_state_report(CemOhpcfListenerObject*, CompressorOhpcfState, const EebusDuration*,
                     const EntityAddressType* entity_addr);
void on_clear_process(CemOhpcfListenerObject*, const EntityAddressType* entity_addr);
```

Macro renames (signatures updated accordingly):

```c
CEM_OHPCF_LISTENER_ON_ANNOUNCE(obj, optional_power_consumption, entity_addr)
CEM_OHPCF_LISTENER_ON_STATE_REPORT(obj, state, start_time, entity_addr)
CEM_OHPCF_LISTENER_ON_CLEAR_PROCESS(obj, entity_addr)
```

Additionally `CemOhpcfReadSmartData` is new:

```c
EebusError CemOhpcfReadSmartData(
    const CemOhpcfUseCaseObject* self,
    const EntityAddressType*     remote_entity_addr,
    ReplyMessageCallback         cb,
    void*                        ctx
);
```

### What to update

1. Add `const EntityAddressType* entity_addr` as the last parameter to your
   `on_announce`, `on_state_report`, and `on_clear_process` implementations.

2. Update the `CemOhpcfListenerInterface` struct literal to point to the
   updated functions.

3. Update `CemOhpcfScheduleOptionalPowerConsumption` call sites to pass
   `ResultMessageCallback cb` and `void* ctx` (or `NULL, NULL`).

