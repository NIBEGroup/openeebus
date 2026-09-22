/**
 * @file
 * @brief Energy Guard Limitation of Power Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_EG_LP_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_EG_LP_LISTENER_INTERFACE_H_

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief EG LP Listener Interface
 * (EG LP Listener "virtual functions table" declaration)
 */
typedef struct EgLpListenerInterface EgLpListenerInterface;

/**
 * @brief EG LP Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct EgLpListenerObject EgLpListenerObject;

/**
 * @brief EgLpListener Interface Structure
 */
struct EgLpListenerInterface {
  void (*destruct)(EgLpListenerObject* self);
  void (*on_remote_cs_added)(EgLpListenerObject* self, const EntityAddressType* entity_addr);
  void (*on_remote_cs_removed)(EgLpListenerObject* self, const EntityAddressType* entity_addr);
  void (*on_power_limit_receive)(
      EgLpListenerObject* self,
      const EntityAddressType* entity_addr,
      const ScaledValue* power_limit,
      const DurationType* duration,
      bool is_active
  );
  void (*on_failsafe_power_limit_receive)(
      EgLpListenerObject* self,
      const EntityAddressType* entity_addr,
      const ScaledValue* power_limit
  );
  void (*on_failsafe_duration_receive)(
      EgLpListenerObject* self,
      const EntityAddressType* entity_addr,
      const DurationType* duration
  );
  void (*on_heartbeat_receive)(
      EgLpListenerObject* self,
      const EntityAddressType* entity_addr,
      uint64_t heartbeat_counter
  );
  void (*on_power_nominal_max_receive)(
      EgLpListenerObject* self,
      const EntityAddressType* entity_addr,
      const ScaledValue* power_limit
  );
};

/**
 * @brief EG LP Listener Object Structure
 */
struct EgLpListenerObject {
  const EgLpListenerInterface* interface_;
};

/**
 * @brief EG LP Listener pointer typecast
 */
#define EG_LP_LISTENER_OBJECT(obj) ((EgLpListenerObject*)(obj))

/**
 * @brief EG LP Listener Interface class pointer typecast
 */
#define EG_LP_LISTENER_INTERFACE(obj) (EG_LP_LISTENER_OBJECT(obj)->interface_)

/**
 * @brief EG LP Listener Destruct caller definition
 */
#define EG_LP_LISTENER_DESTRUCT(obj) (EG_LP_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief EG LP Listener On Remote CS Added caller definition
 */
#define EG_LP_LISTENER_ON_REMOTE_CS_ADDED(obj, entity_addr) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_remote_cs_added(obj, entity_addr))

/**
 * @brief EG LP Listener On Remote CS Removed caller definition
 */
#define EG_LP_LISTENER_ON_REMOTE_CS_REMOVED(obj, entity_addr) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_remote_cs_removed(obj, entity_addr))

/**
 * @brief EG LP Listener On Power Limit Receive caller definition
 */
#define EG_LP_LISTENER_ON_POWER_LIMIT_RECEIVE(obj, entity_addr, power_limit, duration, is_active) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_power_limit_receive(obj, entity_addr, power_limit, duration, is_active))

/**
 * @brief EG LP Listener On Failsafe Power Limit Receive caller definition
 */
#define EG_LP_LISTENER_ON_FAILSAFE_POWER_LIMIT_RECEIVE(obj, entity_addr, power_limit) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_failsafe_power_limit_receive(obj, entity_addr, power_limit))

/**
 * @brief EG LP Listener On Failsafe Duration Receive caller definition
 */
#define EG_LP_LISTENER_ON_FAILSAFE_DURATION_RECEIVE(obj, entity_addr, duration) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_failsafe_duration_receive(obj, entity_addr, duration))

/**
 * @brief EG LP Listener On Heartbeat Receive caller definition
 */
#define EG_LP_LISTENER_ON_HEARTBEAT_RECEIVE(obj, entity_addr, heartbeat_counter) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_heartbeat_receive(obj, entity_addr, heartbeat_counter))

/**
 * @brief EG LP Listener On Power Nominal Max Receive caller definition
 */
#define EG_LP_LISTENER_ON_POWER_NOMINAL_MAX_RECEIVE(obj, entity_addr, power_limit) \
  (EG_LP_LISTENER_INTERFACE(obj)->on_power_nominal_max_receive(obj, entity_addr, power_limit))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_EG_LP_LISTENER_INTERFACE_H_
