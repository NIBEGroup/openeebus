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
#include "src/service/api/eebus_service_config.h"

#include <gtest/gtest.h>

#include <string_view>

#include "mocks/ship/tls_certificate/tls_certificate_mock.h"
#include "tests/src/memory_leak.inc"

struct EebusServiceConfigCreateTestInput {
  std::string_view description;
  const char* vendor;
  const char* brand;
  const char* model;
  const char* serial;
  const char* device_type;
  const EntityTypeType* entity_types;
  size_t entity_types_size;
  int32_t port;
  bool expect_null;
};

std::ostream& operator<<(std::ostream& os, EebusServiceConfigCreateTestInput Test) {
  return os << Test.description;
}

class EebusServiceConfigCreateTests : public ::testing::TestWithParam<EebusServiceConfigCreateTestInput> {};

TEST_P(EebusServiceConfigCreateTests, eebus_service_config_create_tests) {
  // Arrange: Initialize the Configuration init parameters from test input
  const char* vendor = GetParam().vendor;
  const char* brand  = GetParam().brand;
  const char* model  = GetParam().model;
  const char* serial = GetParam().serial;

  const char* device_type = GetParam().device_type;

  const int32_t port = GetParam().port;

  // Act: Initialise Configuration
  std::unique_ptr<EebusServiceConfig, decltype(&EebusServiceConfigDelete)> cfg{
      EebusServiceConfigCreate(vendor, brand, model, serial, device_type, port),
      EebusServiceConfigDelete
  };

  // Assert: Verify with expected return value
  if (GetParam().expect_null) {
    EXPECT_EQ(cfg, nullptr);
  } else {
    EXPECT_NE(cfg, nullptr);
  }

  // Explicitly reset the cfg before checking the memory
  cfg.reset();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

static constexpr EntityTypeType entity_types[] = {kEntityTypeTypeCEM};

INSTANTIATE_TEST_SUITE_P(
    EebusServiceConfigCreate,
    EebusServiceConfigCreateTests,
    ::testing::Values(
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid vendor code 1",
            .vendor            = "",
            .brand             = "brand",
            .model             = "model",
            .serial            = "serial",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 0,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid vendor code 2",
            .vendor            = "",
            .brand             = "brand",
            .model             = "model",
            .serial            = "serial",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid brand",
            .vendor            = "vendor",
            .brand             = "",
            .model             = "model",
            .serial            = "serial",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid model",
            .vendor            = "vendor",
            .brand             = "brand",
            .model             = "",
            .serial            = "serial",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid serial number",
            .vendor            = "vendor",
            .brand             = "brand",
            .model             = "model",
            .serial            = "",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Invalid device type",
            .vendor            = "vendor",
            .brand             = "brand",
            .model             = "model",
            .serial            = "serial",
            .device_type       = "",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = true,
        },
        EebusServiceConfigCreateTestInput{
            .description       = "Init success",
            .vendor            = "vendor",
            .brand             = "brand",
            .model             = "model",
            .serial            = "serial",
            .device_type       = "EnergyManagementSystem",
            .entity_types      = entity_types,
            .entity_types_size = ARRAY_SIZE(entity_types),
            .port              = 4567,
            .expect_null       = false,
        }
    )
);

TEST(ConfigurationTests, eebus_service_config_fields_test) {
  std::unique_ptr<EebusServiceConfig, decltype(&EebusServiceConfigDelete)> cfg{
      EebusServiceConfigCreate("vendor", "brand", "model", "serial", "EnergyManagementSystem", 4567),
      EebusServiceConfigDelete
  };

  EXPECT_NE(cfg, nullptr);

  EebusServiceConfigSetRegisterAutoAccept(cfg.get(), true);
  EXPECT_EQ(EebusServiceConfigGetRegisterAutoAccept(cfg.get()), true);

  // Pre-trust is the default
  EXPECT_EQ(EebusServiceConfigGetTrustMode(cfg.get()), kEebusTrustModePreTrust);

  EebusServiceConfigSetTrustMode(cfg.get(), kEebusTrustModePostTrust);
  EXPECT_EQ(EebusServiceConfigGetTrustMode(cfg.get()), kEebusTrustModePostTrust);

  EXPECT_STREQ(EebusServiceConfigGetShipId(cfg.get()), "brand-serial");

  EXPECT_STREQ(EebusServiceConfigGetMdnsServiceName(cfg.get()), "brand-serial");

  const char* const alternate = "alternate";

  EebusServiceConfigSetAlternateIdentifier(cfg.get(), alternate);
  EXPECT_STREQ(EebusServiceConfigGetShipId(cfg.get()), alternate);

  EebusServiceConfigSetAlternateMdnsServiceName(cfg.get(), alternate);
  EXPECT_STREQ(EebusServiceConfigGetMdnsServiceName(cfg.get()), alternate);

  EXPECT_EQ(EebusServiceConfigGetPort(cfg.get()), 4567);

  EXPECT_STREQ(EebusServiceConfigGetVendorCode(cfg.get()), "vendor");
  EXPECT_STREQ(EebusServiceConfigGetDeviceBrand(cfg.get()), "brand");
  EXPECT_STREQ(EebusServiceConfigGetDeviceModel(cfg.get()), "model");
  EXPECT_STREQ(EebusServiceConfigGetDeviceSerialNumber(cfg.get()), "serial");
  EXPECT_STREQ(EebusServiceConfigGetDeviceType(cfg.get()), "EnergyManagementSystem");

  EXPECT_EQ(EebusServiceConfigGetFeatureSet(cfg.get()), kNetworkManagementFeatureSetTypeSmart);
}
