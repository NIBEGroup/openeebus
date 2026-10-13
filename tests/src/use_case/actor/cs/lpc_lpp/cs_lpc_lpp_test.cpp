/*
 * Copyright 2026 NIBE AB
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
 * @brief CS LPC and CS LPP on the same entity: they share one LoadControl server feature (with one limit each)
 * and one DeviceConfiguration server feature. Each use case adds its own write approval callback to the shared
 * features, and a write is only applied once every callback approved it.
 */

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cs_lp_listener_mock.h"
#include "mocks/use_case/api/cs_lpc_approver_mock.h"
#include "src/common/array_util.h"
#include "src/spine/device/device_local.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/cs/lpc/cs_lpc.h"
#include "src/use_case/actor/cs/lpp/cs_lpp.h"
#include "tests/src/use_case/actor/cs/lpc/receive/discovery_response.inc"
#include "tests/src/use_case/actor/cs/lpc/receive/load_control_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpc_lpp/receive/limit_write.inc"
#include "tests/src/use_case/use_case_test_fixture.h"

namespace cs_lpc_lpp_test {

using testing::_;
using testing::HasSubstr;
using testing::Invoke;
using testing::Return;

constexpr int kLpcLimitId = 0;
constexpr int kLppLimitId = 1;

class CsLpcLppTestFixture : public UseCaseTestFixture {
 public:
  CsLpcLppTestFixture() : UseCaseTestFixture("HeatPump", "HeatPump", "123456789") {};

  void SetUpUseCase() override {
    uint32_t entity_ids[1]{static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local_.get())))};

    EntityLocalObject* const entity = EntityLocalCreate(
        device_local_.get(),
        kEntityTypeTypeCEM,
        entity_ids,
        ARRAY_SIZE(entity_ids),
        kHeartbeatTimeout
    );

    lpc_listener_mock_.reset(CsLpListenerMockCreate());
    lpp_listener_mock_.reset(CsLpListenerMockCreate());
    lpc_.reset(CsLpcUseCaseCreate(entity, 0, CS_LP_LISTENER_OBJECT(lpc_listener_mock_.get())));
    lpp_.reset(CsLppUseCaseCreate(entity, 0, CS_LP_LISTENER_OBJECT(lpp_listener_mock_.get())));

    lpc_approver_mock_.reset(CsLpcApproverMockCreate());
    lpp_approver_mock_.reset(CsLpcApproverMockCreate());
    ON_CALL(*lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _))
        .WillByDefault(Invoke([this](CsLpcApproverObject*, const char* ski, MsgCounterType msg_cnt, auto...) {
          CsLpApproveWrite(lpc_.get(), ski, msg_cnt);
        }));
    ON_CALL(*lpp_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _))
        .WillByDefault(Invoke([this](CsLpcApproverObject*, const char* ski, MsgCounterType msg_cnt, auto...) {
          CsLpApproveWrite(lpp_.get(), ski, msg_cnt);
        }));

    DEVICE_LOCAL_ADD_ENTITY(device_local_.get(), entity);

    // Only the answers to the limit writes matter here, not the exact content of the setup traffic
    EXPECT_CALL(*data_write_mock_->gmock, WriteMessage(_, _, _))
        .WillRepeatedly(Invoke([this](DataWriterObject*, const uint8_t* msg, size_t size) {
          sent_.emplace_back(reinterpret_cast<const char*>(msg), size);
        }));
  };

  void TearDownUseCase() override {
    EXPECT_CALL(*lpc_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    EXPECT_CALL(*lpp_listener_mock_->gmock, Destruct(_)).WillOnce(Return());
    EXPECT_CALL(*lpc_approver_mock_->gmock, Destruct(_)).WillOnce(Return());
    EXPECT_CALL(*lpp_approver_mock_->gmock, Destruct(_)).WillOnce(Return());
    CsLpSetWriteApprover(lpc_.get(), nullptr);
    CsLpSetWriteApprover(lpp_.get(), nullptr);
    lpc_.reset();
    lpp_.reset();
    lpc_listener_mock_.reset();
    lpp_listener_mock_.reset();
    lpc_approver_mock_.reset();
    lpp_approver_mock_.reset();
  };

  // A remote EG that knows our features and is bound to the shared LoadControl server (messages from the LPC tests).
  // The use case discovery is skipped, so the listeners aren't notified: the tests read the limits back instead.
  void SetUpBoundRemote() {
    HandleMessage(cs_lpc_test::receive::discovery_response);
    HandleMessage(cs_lpc_test::receive::load_control_binding_request);
  }

  std::string ResultFor(int msg_counter) {
    const std::string reference = R"("msgCounterReference":)" + std::to_string(msg_counter);
    for (const std::string& msg : sent_) {
      if ((msg.find(reference) != std::string::npos) && (msg.find("resultData") != std::string::npos)) {
        return msg;
      }
    }
    return "";
  }

 protected:
  std::vector<std::string> sent_;

  std::unique_ptr<CsLpListenerMock, decltype(&CsLpListenerMockDelete)> lpc_listener_mock_{
      nullptr,
      CsLpListenerMockDelete
  };
  std::unique_ptr<CsLpListenerMock, decltype(&CsLpListenerMockDelete)> lpp_listener_mock_{
      nullptr,
      CsLpListenerMockDelete
  };
  std::unique_ptr<CsLpcApproverMock, decltype(&CsLpcApproverMockDelete)> lpc_approver_mock_{
      nullptr,
      CsLpcApproverMockDelete
  };
  std::unique_ptr<CsLpcApproverMock, decltype(&CsLpcApproverMockDelete)> lpp_approver_mock_{
      nullptr,
      CsLpcApproverMockDelete
  };
  std::unique_ptr<CsLpUseCaseObject, decltype(&CsLpUseCaseDelete)> lpc_{nullptr, CsLpUseCaseDelete};
  std::unique_ptr<CsLpUseCaseObject, decltype(&CsLpUseCaseDelete)> lpp_{nullptr, CsLpUseCaseDelete};
};

TEST_F(CsLpcLppTestFixture, LpcAndLppHaveOneLimitEach) {
  LoadLimit limit{{0}};
  const ScaledValue lpc_value{100, 0};
  const ScaledValue lpp_value{-100, 0};
  EXPECT_EQ(CsLpcSetActiveConsumptionPowerLimit(lpc_.get(), &lpc_value, false, true), kEebusErrorOk);
  EXPECT_EQ(CsLppSetActiveProductionPowerLimit(lpp_.get(), &lpp_value, false, true), kEebusErrorOk);

  EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(lpc_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(100, 0));
  EXPECT_EQ(CsLppGetActiveProductionPowerLimit(lpp_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(-100, 0));
}

TEST_F(CsLpcLppTestFixture, LppLimitWriteIsAppliedWhileLpcHasAnApprover) {
  CsLpSetWriteApprover(lpc_.get(), CS_LPC_APPROVER_OBJECT(lpc_approver_mock_.get()));
  SetUpBoundRemote();

  EXPECT_CALL(*lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _)).Times(0);

  HandleMessage(receive::limit_write(100, kLppLimitId, -3000).c_str());

  EXPECT_THAT(ResultFor(100), HasSubstr(R"("errorNumber":0)"));
  LoadLimit limit{{0}};
  EXPECT_EQ(CsLppGetActiveProductionPowerLimit(lpp_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(-3000, 0));
  EXPECT_TRUE(limit.is_active);
}

TEST_F(CsLpcLppTestFixture, LpcLimitWriteIsAppliedWhileLppHasAnApprover) {
  CsLpSetWriteApprover(lpp_.get(), CS_LPC_APPROVER_OBJECT(lpp_approver_mock_.get()));
  SetUpBoundRemote();

  EXPECT_CALL(*lpp_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _)).Times(0);

  HandleMessage(receive::limit_write(100, kLpcLimitId, 3000).c_str());

  EXPECT_THAT(ResultFor(100), HasSubstr(R"("errorNumber":0)"));
  LoadLimit limit{{0}};
  EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(lpc_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(3000, 0));
  EXPECT_TRUE(limit.is_active);
}

TEST_F(CsLpcLppTestFixture, EachLimitWriteOnlyReachesItsOwnApprover) {
  CsLpSetWriteApprover(lpc_.get(), CS_LPC_APPROVER_OBJECT(lpc_approver_mock_.get()));
  CsLpSetWriteApprover(lpp_.get(), CS_LPC_APPROVER_OBJECT(lpp_approver_mock_.get()));
  SetUpBoundRemote();

  EXPECT_CALL(*lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, ScaledValueEq(3000, 0), _, true));
  EXPECT_CALL(*lpp_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, ScaledValueEq(-3000, 0), _, true));

  HandleMessage(receive::limit_write(100, kLpcLimitId, 3000).c_str());
  HandleMessage(receive::limit_write(101, kLppLimitId, -3000).c_str());

  EXPECT_THAT(ResultFor(100), HasSubstr(R"("errorNumber":0)"));
  EXPECT_THAT(ResultFor(101), HasSubstr(R"("errorNumber":0)"));
  LoadLimit limit{{0}};
  EXPECT_EQ(CsLpcGetActiveConsumptionPowerLimit(lpc_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(3000, 0));
  EXPECT_EQ(CsLppGetActiveProductionPowerLimit(lpp_.get(), &limit), kEebusErrorOk);
  EXPECT_THAT(&limit.value, ScaledValueEq(-3000, 0));
}

TEST_F(CsLpcLppTestFixture, UnknownLimitIdIsRejected) {
  CsLpSetWriteApprover(lpc_.get(), CS_LPC_APPROVER_OBJECT(lpc_approver_mock_.get()));
  CsLpSetWriteApprover(lpp_.get(), CS_LPC_APPROVER_OBJECT(lpp_approver_mock_.get()));
  SetUpBoundRemote();

  EXPECT_CALL(*lpc_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _)).Times(0);
  EXPECT_CALL(*lpp_approver_mock_->gmock, OnPowerLimitApprovalRequested(_, _, _, _, _, _)).Times(0);

  HandleMessage(receive::limit_write(100, 42, 3000).c_str());

  EXPECT_THAT(ResultFor(100), HasSubstr(R"("errorNumber":7)"));
}

}  // namespace cs_lpc_lpp_test
