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

#include "stats_mock_object_test.h"

#include "battery_stats_proxy.h"
#include "mock_stats_remote_object.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
/**
 * @tc.name: StatsMockObjectTest_001
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is mock, "SendRequest" function return false)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockObjectTest, StatsMockObjectTest_001, TestSize.Level0)
{
    sptr<MockStatsRemoteObject> sptrRemoteObj = new MockStatsRemoteObject();
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
}