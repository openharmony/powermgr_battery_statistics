/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "stats_service_stub_test.h"

#include <message_parcel.h>

#include "stats_errors.h"
#include "stats_log.h"
#include "battery_stats_ipc_interface_code.h"
#include "battery_stats_proxy.h"
#include "battery_stats_service.h"
#include "battery_stats_stub.h"

using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace std;
using namespace testing::ext;

namespace {
/**
 * @tc.name: StatsServiceStubTest_001
 * @tc.desc: test BatteryStatsStub
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceStubTest, StatsServiceStubTest_001, TestSize.Level0)
{
    sptr<BatteryStatsService> statsService = BatteryStatsService::GetInstance();
    sptr<BatteryStatsStub> statsStub = static_cast<sptr<BatteryStatsStub>>(statsService);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int ret = statsStub->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GET),
        data, reply, option);
    EXPECT_EQ(ret, E_STATS_GET_SERVICE_FAILED);
}

/**
 * @tc.name: StatsServiceStubTest_002
 * @tc.desc: test BatteryStatsStub
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceStubTest, StatsServiceStubTest_002, TestSize.Level0)
{
    sptr<BatteryStatsService> statsService = BatteryStatsService::GetInstance();
    sptr<BatteryStatsStub> statsStub = static_cast<sptr<BatteryStatsStub>>(statsService);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor());
    
    uint32_t invalidCode =
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GET) + 100;
    int ret = statsStub->OnRemoteRequest(invalidCode, data, reply, option);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.name: StatsServiceStubTest_003
 * @tc.desc: test size of ShellDumpStub param exceed Max limit
 * @tc.type: FUNC
 * @tc.require: issueI6ARNA
 */
HWTEST_F (StatsServiceStubTest, StatsServiceStubTest_003, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "ThermalMockStubTest003 start.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<BatteryStatsService> statsService = BatteryStatsService::GetInstance();
    sptr<BatteryStatsStub> statsStub = static_cast<sptr<BatteryStatsStub>>(statsService);

    data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor());
    const int32_t PARAM_MAX_NUM = 100;
    data.WriteUint32(PARAM_MAX_NUM);
    uint32_t code = static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_DUMP);
    int32_t ret = statsStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, E_STATS_EXCEED_PARAM_LIMIT) << " ret:" << ret;

    data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor());
    data.WriteUint32(1);
    data.WriteString("");
    ret = statsStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, E_STATS_READ_PARCEL_ERROR) << " ret:" << ret;

    STATS_HILOGD(LABEL_TEST, "ThermalMockStubTest003 end.");
}
}