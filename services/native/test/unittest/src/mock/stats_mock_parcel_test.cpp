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

#include "stats_mock_parcel_test.h"

#include <hisysevent.h>

#include "ipc_object_stub.h"

#include "battery_stats_client.h"
#include "battery_stats_proxy.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
/**
 * @tc.name: StatsMockParcelTest_001
 * @tc.desc: test BatteryStatsClient function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockParcelTest, StatsMockParcelTest_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(true);
    auto infoList = statsClient.GetBatteryStats();
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetAppStatsMah(uid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetAppStatsPercent(uid));
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetPartStatsMah(consumptionType));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetPartStatsPercent(consumptionType));
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetTotalTimeSecond(statsType));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsClient.GetTotalDataBytes(statsType));
    std::vector<std::string> dumpArgs;
    dumpArgs.push_back("-batterystats");
    EXPECT_EQ("can't connect service", statsClient.Dump(dumpArgs));
}

/**
 * @tc.name: StatsMockParcelTest_002
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is null)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockParcelTest, StatsMockParcelTest_002, TestSize.Level0)
{
    std::shared_ptr<BatteryStatsProxy> sptrStatsProxy = std::make_shared<BatteryStatsProxy>(nullptr);
    sptrStatsProxy->Reset();
    sptrStatsProxy->SetOnBattery(true);
    auto infoList = sptrStatsProxy->GetBatteryStats();
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetAppStatsMah(uid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetAppStatsPercent(uid));
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetPartStatsMah(consumptionType));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetPartStatsPercent(consumptionType));
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetTotalTimeSecond(statsType, StatsUtils::INVALID_VALUE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetTotalDataBytes(statsType, StatsUtils::INVALID_VALUE));
    std::vector<std::string> dumpArgs;
    dumpArgs.push_back("-batterystats");
    EXPECT_EQ("remote error", sptrStatsProxy->ShellDump(dumpArgs, dumpArgs.size()));
}

/**
 * @tc.name: StatsMockParcelTest_003
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is not null)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockParcelTest, StatsMockParcelTest_003, TestSize.Level0)
{
    sptr<IPCObjectStub> sptrRemoteObj = new IPCObjectStub();
    std::shared_ptr<BatteryStatsProxy> sptrStatsProxy = std::make_shared<BatteryStatsProxy>(sptrRemoteObj);
    sptrStatsProxy->Reset();
    sptrStatsProxy->SetOnBattery(true);
    auto infoList = sptrStatsProxy->GetBatteryStats();
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetAppStatsMah(uid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetAppStatsPercent(uid));
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetPartStatsMah(consumptionType));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetPartStatsPercent(consumptionType));
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetTotalTimeSecond(statsType, StatsUtils::INVALID_VALUE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, sptrStatsProxy->GetTotalDataBytes(statsType, StatsUtils::INVALID_VALUE));
    std::vector<std::string> dumpArgs;
    dumpArgs.push_back("-batterystats");
    EXPECT_EQ("remote error", sptrStatsProxy->ShellDump(dumpArgs, dumpArgs.size()));
}

/**
 * @tc.name: StatsMockParcelTest_004
 * @tc.desc: test class BatteryStatsInfo function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockParcelTest, StatsMockParcelTest_004, TestSize.Level0)
{
    std::shared_ptr<BatteryStatsInfo> sptrStatsInfo = std::make_shared<BatteryStatsInfo>();
    EXPECT_NE(sptrStatsInfo, nullptr);
    int32_t uid = 1004;
    int32_t userId = 10005;
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    double totalPowerMah = 1000.0;
    sptrStatsInfo->SetUid(uid);
    sptrStatsInfo->SetUserId(userId);
    sptrStatsInfo->SetConsumptioType(consumptionType);
    sptrStatsInfo->SetPower(totalPowerMah);
    EXPECT_EQ(uid, sptrStatsInfo->GetUid());
    EXPECT_EQ(userId, sptrStatsInfo->GetUserId());
    EXPECT_EQ(consumptionType, sptrStatsInfo->GetConsumptionType());
    EXPECT_EQ(totalPowerMah, sptrStatsInfo->GetPower());

    Parcel infoParcel = {};
    EXPECT_FALSE(sptrStatsInfo->Marshalling(infoParcel));
}
}