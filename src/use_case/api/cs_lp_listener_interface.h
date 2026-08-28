/**
 * @file
 * @brief CS Limitation of Power Listener interface declarations
 */

#ifndef SRC_USE_CASE_API_CS_LP_LISTENER_INTERFACE_H_
#define SRC_USE_CASE_API_CS_LP_LISTENER_INTERFACE_H_

#include <stdbool.h>

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/model/scaled_value.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief CS LP Listener Interface
 * (CS LP Listener "virtual functions table" declaration)
 */
typedef struct CsLpListenerInterface CsLpListenerInterface;

/**
 * @brief CS LP Listener Object type definition
 * ("abstract class", has no members but only pointer to
 * "virtual functions table")
 */
typedef struct CsLpListenerObject CsLpListenerObject;

/**
 * @brief CsLpListener Interface Structure
 */
struct CsLpListenerInterface {
    void (*destruct)(CsLpListenerObject* self);
    void (*on_remote_eg_added)(CsLpListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_remote_eg_removed)(CsLpListenerObject* self, const EntityAddressType* entity_addr);
    void (*on_power_limit_receive)(
        CsLpListenerObject* self,
        const ScaledValue* power_limit,
        const DurationType* duration,
        bool is_active
    );
    void (*on_failsafe_power_limit_receive)(CsLpListenerObject* self, const ScaledValue* power_limit);
    void (*on_failsafe_duration_receive)(CsLpListenerObject* self, const DurationType* duration);
    void (*on_heartbeat_receive)(CsLpListenerObject* self, uint64_t heartbeat_counter);
};

/**
 * @brief CS LP Listener Object Structure
 */
struct CsLpListenerObject {
    const CsLpListenerInterface* interface_;
};

/**
 * @brief CS LP Listener pointer typecast
 */
#define CS_LP_LISTENER_OBJECT(obj) ((CsLpListenerObject*)(obj))

/**
 * @brief CS LP Listener Interface class pointer typecast
 */
#define CS_LP_LISTENER_INTERFACE(obj) (CS_LP_LISTENER_OBJECT(obj)->interface_)

/**
 * @brief CS LP Listener Destruct caller definition
 */
#define CS_LP_LISTENER_DESTRUCT(obj) (CS_LP_LISTENER_INTERFACE(obj)->destruct(obj))

/**
 * @brief CS LP Listener On Remote EG Added caller definition
 */
#define CS_LP_LISTENER_ON_REMOTE_EG_ADDED(obj, entity_addr) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_remote_eg_added(obj, entity_addr))

/**
 * @brief CS LP Listener On Remote EG Removed caller definition
 */
#define CS_LP_LISTENER_ON_REMOTE_EG_REMOVED(obj, entity_addr) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_remote_eg_removed(obj, entity_addr))

/**
 * @brief CS LP Listener On Power Limit Receive caller definition
 */
#define CS_LP_LISTENER_ON_POWER_LIMIT_RECEIVE(obj, power_limit, duration, is_active) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_power_limit_receive(obj, power_limit, duration, is_active))

/**
 * @brief CS LP Listener On Failsafe Power Limit Receive caller definition
 */
#define CS_LP_LISTENER_ON_FAILSAFE_POWER_LIMIT_RECEIVE(obj, power_limit) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_failsafe_power_limit_receive(obj, power_limit))

/**
 * @brief CS LP Listener On Failsafe Duration Receive caller definition
 */
#define CS_LP_LISTENER_ON_FAILSAFE_DURATION_RECEIVE(obj, duration) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_failsafe_duration_receive(obj, duration))

/**
 * @brief CS LP Listener On Heartbeat Receive caller definition
 */
#define CS_LP_LISTENER_ON_HEARTBEAT_RECEIVE(obj, heartbeat_counter) \
    (CS_LP_LISTENER_INTERFACE(obj)->on_heartbeat_receive(obj, heartbeat_counter))

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_CS_LP_LISTENER_INTERFACE_H_
