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

#include "stats_mock_peer_test.h"

#include "battery_stats_client.h"
#include "battery_stats_proxy.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
/**
 * @tc.name: StatsMockPeerTest_001
 * @tc.desc: test BatteryStatsClient function(remoteObject is null)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsMockPeerTest, StatsMockPeerTest_001, TestSize.Level0)
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
}