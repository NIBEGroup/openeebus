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
#include "src/ship/api/mdns_entry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string_view>

#include "src/common/string_util.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/string_ptr.h"

using namespace std::literals;

using MdnsEntryPtr = std::unique_ptr<MdnsEntry, decltype(&MdnsEntryDelete)>;

struct UriDeleter {
  void operator()(const char* p) const {
    StringDelete(const_cast<char*>(p));
  }
};
using UriPtr = std::unique_ptr<const char, UriDeleter>;

/**
 * Marker for txt record size to automatically detect size
 */
static const size_t kTxtRecordSizeAuto = 0xFFFFFFFF;

struct MdnsEntrySetResolveInfoTestInput {
  std::string_view description;

  std::string_view txt_record;
  size_t txt_record_size = kTxtRecordSizeAuto;

  EebusError ret = kEebusErrorOk;

  StringPtr txtvers = nullptr;
  StringPtr id      = nullptr;
  StringPtr path    = nullptr;
  StringPtr ski     = nullptr;
  StringPtr reg     = nullptr;
  StringPtr brand   = nullptr;
  StringPtr type    = nullptr;
  StringPtr model   = nullptr;
};

std::ostream& operator<<(std::ostream& os, MdnsEntrySetResolveInfoTestInput test_input) {
  return os << test_input.description;
}

class MdnsEntrySetResolveInfoTests : public ::testing::TestWithParam<MdnsEntrySetResolveInfoTestInput> {};

TEST_P(MdnsEntrySetResolveInfoTests, MdnsEntrySetResolveInfoTests) {
  // Arrange: Initialize the mDNS entry instance
  MdnsEntryPtr mdns_entry{MdnsEntryCreate("test_name", ".local", 0), &MdnsEntryDelete};

  // Act: Run the mDNS txt record parsing procedure
  const char* const txt_record = GetParam().txt_record.data();
  size_t txt_record_size       = GetParam().txt_record_size;
  if (txt_record_size == kTxtRecordSizeAuto) {
    txt_record_size = GetParam().txt_record.size();
  }

  MdnsEntrySetHost(mdns_entry.get(), "DESKTOP-IAKQS71.local.:4769");
  MdnsEntrySetPort(mdns_entry.get(), 4769);
  const EebusError ret = MdnsEntryParseTxtRecord(mdns_entry.get(), txt_record, (uint16_t)txt_record_size);

  // Assert: Verify with expected return value and txt record fields values
  EXPECT_EQ(GetParam().ret, ret);
  EXPECT_EQ(GetParam().txtvers, StringPtr(MdnsEntryGetTxtVers(mdns_entry.get())));
  EXPECT_EQ(GetParam().id, StringPtr(MdnsEntryGetId(mdns_entry.get())));
  EXPECT_EQ(GetParam().path, StringPtr(MdnsEntryGetPath(mdns_entry.get())));
  EXPECT_EQ(GetParam().ski, StringPtr(MdnsEntryGetSki(mdns_entry.get())));
  EXPECT_EQ(GetParam().reg, StringPtr(MdnsEntryGetRegister(mdns_entry.get())));
  EXPECT_EQ(GetParam().brand, StringPtr(MdnsEntryGetBrand(mdns_entry.get())));
  EXPECT_EQ(GetParam().type, StringPtr(MdnsEntryGetType(mdns_entry.get())));
  EXPECT_EQ(GetParam().model, StringPtr(MdnsEntryGetModel(mdns_entry.get())));

  mdns_entry.reset();

  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

static MdnsEntryPtr MakeEntry(const char* host, int port, const char* path_txt_record) {
  MdnsEntryPtr entry{MdnsEntryCreate("name", ".local", 0), &MdnsEntryDelete};
  MdnsEntrySetHost(entry.get(), host);
  MdnsEntrySetPort(entry.get(), port);
  if (path_txt_record != nullptr) {
    MdnsEntryParseTxtRecord(entry.get(), path_txt_record, (uint16_t)strlen(path_txt_record));
  }

  return entry;
}

TEST(MdnsEntryToUriTest, NormalHostPortPath) {
  MdnsEntryPtr entry{MakeEntry("myhost.local", 4769, "\013path=/ship/")};
  UriPtr uri{MdnsEntryToUri(entry.get())};
  EXPECT_STREQ(uri.get(), "wss://myhost.local:4769/ship/");
  uri.reset();
  entry.reset();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

TEST(MdnsEntryToUriTest, TrailingDotStripped) {
  MdnsEntryPtr entry{MakeEntry("myhost.local.", 4769, "\013path=/ship/")};
  UriPtr uri{MdnsEntryToUri(entry.get())};
  EXPECT_STREQ(uri.get(), "wss://myhost.local:4769/ship/");
  uri.reset();
  entry.reset();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

TEST(MdnsEntryToUriTest, NullHostReturnsNull) {
  MdnsEntryPtr entry{MdnsEntryCreate("name", ".local", 0), &MdnsEntryDelete};
  EXPECT_EQ(MdnsEntryToUri(entry.get()), nullptr);
  entry.reset();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}

INSTANTIATE_TEST_SUITE_P(
    MdnsEntrySetResolveInfoTests,
    MdnsEntrySetResolveInfoTests,
    ::testing::Values(
        MdnsEntrySetResolveInfoTestInput{
            .description     = "Test txt buffer overrun"sv,
            .txt_record      = "\012txtvers=1",
            .txt_record_size = 012,
            .ret             = kEebusErrorParse,
            .txtvers         = nullptr,
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test txt buffer underrun"sv,
            .txt_record  = "\011txtvers=10",
            .ret         = kEebusErrorParse,
            .txtvers     = "1",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test no key"sv,
            .txt_record  = "\004=111",
            .ret         = kEebusErrorParse,
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test no value"sv,
            .txt_record  = "\010txtvers=",
            .ret         = kEebusErrorParse,
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test LpcControllableSystem txt record"sv,
            .txt_record  = "\011txtvers=1"
                           "\042id=LpcControllableSystem-001-A0001"
                           "\013path=/ship/"
                           "\054ski=4144df31913b5147324175e64c3c688908a5f3c3"
                           "\016register=false",
            .txtvers     = "1",
            .id          = "LpcControllableSystem-001-A0001",
            .path        = "/ship/",
            .ski         = "4144df31913b5147324175e64c3c688908a5f3c3",
            .reg         = "false",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test enjoyelec txt record"sv,
            .txt_record  = "\011txtvers=1"
                           "\013path=/ship/"
                           "\021id=Enjoyelec_HEMS"
                           "\054ski=47c21b636fa1d15631c5c424b6378b9dcf4cbf7d"
                           "\017brand=enjoyelec"
                           "\012model=HEMS"
                           "\033type=EnergyManagementSystem"
                           "\016register=false",
            .txtvers     = "1",
            .id          = "Enjoyelec_HEMS",
            .path        = "/ship/",
            .ski         = "47c21b636fa1d15631c5c424b6378b9dcf4cbf7d",
            .reg         = "false",
            .brand       = "enjoyelec",
            .type        = "EnergyManagementSystem",
            .model       = "HEMS",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test Swistec txt record"sv,
            .txt_record  = "\011txtvers=1"
                           "\054ski=8ce6d8719688b9c59246dfd7b7579aac9c519cad"
                           "\021id=DSWIC022060001"
                           "\013path=/ship/"
                           "\016register=false"
                           "\015model=SwisBox"
                           "\017type=ControlBox"
                           "\015brand=Swistec",
            .txtvers     = "1",
            .id          = "DSWIC022060001",
            .path        = "/ship/",
            .ski         = "8ce6d8719688b9c59246dfd7b7579aac9c519cad",
            .reg         = "false",
            .brand       = "Swistec",
            .type        = "ControlBox",
            .model       = "SwisBox",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test Ecoflow txt record"sv,
            .txt_record  = "\016register=false"
                           "\024type=ChargingStation"
                           "\020model=PowerPulse"
                           "\015brand=Ecoflow"
                           "\054ski=9fadfb6bdd5b607810e90b596e16165cd21d1e92"
                           "\032id=Ecoflow-PowerPulse-demo"
                           "\013path=/ship/"
                           "\011txtvers=1",
            .txtvers     = "1",
            .id          = "Ecoflow-PowerPulse-demo",
            .path        = "/ship/",
            .ski         = "9fadfb6bdd5b607810e90b596e16165cd21d1e92",
            .reg         = "false",
            .brand       = "Ecoflow",
            .type        = "ChargingStation",
            .model       = "PowerPulse",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test Schneider Electric txt record"sv,
            .txt_record  = "\011txtvers=1"
                           "\021id=1u0022-i2g-191"
                           "\013path=/ship/"
                           "\054ski=cf581a5b1fba5e00292cc235dcc43614891fd48b"
                           "\016register=false"
                           "\017model=HEMSlogic"
                           "\011type=HEMS"
                           "\030brand=Schneider Electric",
            .txtvers     = "1",
            .id          = "1u0022-i2g-191",
            .path        = "/ship/",
            .ski         = "cf581a5b1fba5e00292cc235dcc43614891fd48b",
            .reg         = "false",
            .brand       = "Schneider Electric",
            .type        = "HEMS",
            .model       = "HEMSlogic",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test PPC txt record"sv,
            .txt_record  = "\027model=Steuereinrichtung"
                           "\011type=GCPH"
                           "\011brand=PPC"
                           "\054ski=a1fea7752748d3f8a4b696f1957224b7c5a5f530"
                           "\016register=false"
                           "\013path=/ship/"
                           "\055id=i:32266_u:002518F14BAF_r:Steuereinrichtung"
                           "\011txtvers=1",
            .txtvers     = "1",
            .id          = "i:32266_u:002518F14BAF_r:Steuereinrichtung",
            .path        = "/ship/",
            .ski         = "a1fea7752748d3f8a4b696f1957224b7c5a5f530",
            .reg         = "false",
            .brand       = "PPC",
            .type        = "GCPH",
            .model       = "Steuereinrichtung",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test MENNEKES txt record"sv,
            .txt_record  = "\014model=AMEDIO"
                           "\011type=EVSE"
                           "\016brand=MENNEKES"
                           "\054ski=5f582502a2b9f4333c2258ef0c9e3940d7ae6b50"
                           "\016register=false"
                           "\013path=/ship/"
                           "\027id=AMEDIO_1349201_08794"
                           "\011txtvers=1",
            .txtvers     = "1",
            .id          = "AMEDIO_1349201_08794",
            .path        = "/ship/",
            .ski         = "5f582502a2b9f4333c2258ef0c9e3940d7ae6b50",
            .reg         = "false",
            .brand       = "MENNEKES",
            .type        = "EVSE",
            .model       = "AMEDIO",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test test_eebus_brand txt record"sv,
            .txt_record  = "\030model=test_eebus_gateway"
                           "\027type=test_eebus_Gateway"
                           "\026brand=test_eebus_brand"
                           "\054ski=b5411c134aff8a64ea20621ddfa8eb37cb66c500"
                           "\016register=false"
                           "\013path=/ship/"
                           "\020id=test_eebus_ID"
                           "\011txtvers=1"
                           "\021server_port=39220",
            .txtvers     = "1",
            .id          = "test_eebus_ID",
            .path        = "/ship/",
            .ski         = "b5411c134aff8a64ea20621ddfa8eb37cb66c500",
            .reg         = "false",
            .brand       = "test_eebus_brand",
            .type        = "test_eebus_Gateway",
            .model       = "test_eebus_gateway",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test Elli txt record"sv,
            .txt_record  = "\015model=Wallbox"
                           "\014type=Wallbox"
                           "\012brand=Elli"
                           "\054ski=0a88ab0d65f2b0116cadbdf1cf955512d4795b52"
                           "\016register=false"
                           "\013path=/ship/"
                           "\032id=Elli-Wallbox-2137A1MKNZ"
                           "\011txtvers=1"
                           "\047org.freedesktop.Avahi.cookie=4053815733",
            .txtvers     = "1",
            .id          = "Elli-Wallbox-2137A1MKNZ",
            .path        = "/ship/",
            .ski         = "0a88ab0d65f2b0116cadbdf1cf955512d4795b52",
            .reg         = "false",
            .brand       = "Elli",
            .type        = "Wallbox",
            .model       = "Wallbox",
        },
        MdnsEntrySetResolveInfoTestInput{
            .description = "Test Elli txt record"sv,
            .txt_record  = "\011type=GCPH"
                           "\054ski=efb02aecb789f309f5169f99ac7b9487e04a1d9b"
                           "\006serial"
                           "\016register=false"
                           "\013path=/ship/"
                           "\027model=Steuereinrichtung"
                           "\057id=i:32266_u:EPPCC001161955_r:Steuereinrichtung"
                           "\005cat=1"
                           "\011brand=PPC"
                           "\011txtvers=1",
            .txtvers     = "1",
            .id          = "i:32266_u:EPPCC001161955_r:Steuereinrichtung",
            .path        = "/ship/",
            .ski         = "efb02aecb789f309f5169f99ac7b9487e04a1d9b",
            .reg         = "false",
            .brand       = "PPC",
            .type        = "GCPH",
            .model       = "Steuereinrichtung",
        }
    )
);
