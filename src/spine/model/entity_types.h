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
 * @brief SPEINE Entity related types declarations
 */

#ifndef SRC_SPINE_MODEL_ENTITY_TYPES_H_
#define SRC_SPINE_MODEL_ENTITY_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define DEVICE_INFORMATION_ENTITY_ID ((uint32_t)0)

typedef struct EntityAddressType EntityAddressType;

struct EntityAddressType {
    const char* device;            /**< "device" */
    const uint32_t* const* entity; /**< "entity" */
    size_t entity_size;
};

typedef int32_t EntityTypeType;

enum {
    kEntityTypeTypeBattery,                       /**< "Battery" */
    kEntityTypeTypeCompressor,                    /**< "Compressor" */
    kEntityTypeTypeDeviceInformation,             /**< "DeviceInformation" */
    kEntityTypeTypeDHWCircuit,                    /**< "DHWCircuit" */
    kEntityTypeTypeDHWStorage,                    /**< "DHWStorage" */
    kEntityTypeTypeDishwasher,                    /**< "Dishwasher" */
    kEntityTypeTypeDryer,                         /**< "Dryer" */
    kEntityTypeTypeElectricalImmersionHeater,     /**< "ElectricalImmersionHeater" */
    kEntityTypeTypeFan,                           /**< "Fan" */
    kEntityTypeTypeGasHeatingAppliance,           /**< "GasHeatingAppliance" */
    kEntityTypeTypeGeneric,                       /**< "Generic" */
    kEntityTypeTypeHeatingBufferStorage,          /**< "HeatingBufferStorage" */
    kEntityTypeTypeHeatingCircuit,                /**< "HeatingCircuit" */
    kEntityTypeTypeHeatingObject,                 /**< "HeatingObject" */
    kEntityTypeTypeHeatingZone,                   /**< "HeatingZone" */
    kEntityTypeTypeHeatPumpAppliance,             /**< "HeatPumpAppliance" */
    kEntityTypeTypeHeatSinkCircuit,               /**< "HeatSinkCircuit" */
    kEntityTypeTypeHeatSourceCircuit,             /**< "HeatSourceCircuit" */
    kEntityTypeTypeHeatSourceUnit,                /**< "HeatSourceUnit" */
    kEntityTypeTypeHvacController,                /**< "HVACController" */
    kEntityTypeTypeHvacRoom,                      /**< "HVACRoom" */
    kEntityTypeTypeInstantDHWheater,              /**< "InstantDHWHeater" */
    kEntityTypeTypeInverter,                      /**< "Inverter" */
    kEntityTypeTypeOilHeatingAppliance,           /**< "OilHeatingAppliance" */
    kEntityTypeTypePump,                          /**< "Pump" */
    kEntityTypeTypeRefrigerantCircuit,            /**< "RefrigerantCircuit" */
    kEntityTypeTypeSmartEnergyAppliance,          /**< "SmartEnergyAppliance" */
    kEntityTypeTypeSolarDHWStorage,               /**< "SolarDHWStorage" */
    kEntityTypeTypeSolarThermalCircuit,           /**< "SolarThermalCircuit" */
    kEntityTypeTypeSubMeterElectricity,           /**< "SubMeterElectricity" */
    kEntityTypeTypeTemperatureSensor,             /**< "TemperatureSensor" */
    kEntityTypeTypeWasher,                        /**< "Washer" */
    kEntityTypeTypeBatterySystem,                 /**< "BatterySystem" */
    kEntityTypeTypeElectricityGenerationSystem,   /**< "ElectricityGenerationSystem" */
    kEntityTypeTypeElectricityStorageSystem,      /**< "ElectricityStorageSystem" */
    kEntityTypeTypeGridConnectionPointOfPremises, /**< "GridConnectionPointOfPremises" */
    kEntityTypeTypeHousehold,                     /**< "Household" */
    kEntityTypeTypePVSystem,                      /**< "PVSystem" */
    kEntityTypeTypeEV,                            /**< "EV" */
    kEntityTypeTypeEVSE,                          /**< "EVSE" */
    kEntityTypeTypeChargingOutlet,                /**< "ChargingOutlet" */
    kEntityTypeTypeCEM,                           /**< "CEM" */
    kEntityTypeTypePV,                            /**< "PV" */
    kEntityTypeTypePVESHybrid,                    /**< "PVESHybrid" */
    kEntityTypeTypeElectricalStorage,             /**< "ElectricalStorage" */
    kEntityTypeTypePVString,                      /**< "PVString" */
    kEntityTypeTypeGridGuard,                     /**< "GridGuard" */
    kEntityTypeTypeControllableSystem,            /**< "ControllableSystem" */
};

EntityAddressType* EntityAddressCreate(const char* device, const uint32_t* entity_ids, size_t entity_ids_size);
void EntityAddressDelete(EntityAddressType* self);
EntityAddressType* EntityAddressCopy(const EntityAddressType* self);
bool EntityAddressCompare(const EntityAddressType* addr_a, const EntityAddressType* addr_b);
bool EntityAddressMatchIds(const EntityAddressType* self, const uint32_t* const* entity_ids, size_t entity_ids_size);
void EntityAddressPrint(const char* fmt, const EntityAddressType* addr);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SPINE_MODEL_ENTITY_TYPES_H_
