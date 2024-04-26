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

#include "stats_service_test_mock_parcel.h"

#include "battery_stats_service.h"
#include "stats_service_test_proxy.h"

using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace std;
using namespace testing::ext;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
} // namespace

void StatsServiceTestMockParcel::SetUpTestCase()
{
    ParserAveragePowerFile();
    g_statsService = BatteryStatsService::GetInstance();
    g_statsService->OnStart();

    if (g_statsServiceProxy == nullptr) {
        g_statsServiceProxy = std::make_shared<StatsServiceTestProxy>(g_statsService);
    }
}

void StatsServiceTestMockParcel::TearDownTestCase()
{
    g_statsService->OnStop();
}

namespace {
/**
 * @tc.name: StatsServiceTestMockParcel_001
 * @tc.desc: test BatteryStatsStub by mock parcel
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceTestMockParcel, StatsServiceTestMockParcel_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    EXPECT_TRUE(g_statsServiceProxy->Reset());
    EXPECT_TRUE(g_statsServiceProxy->SetOnBattery(true));
    auto infoList = g_statsServiceProxy->GetBatteryStats();
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetAppStatsMah(uid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetAppStatsPercent(uid));
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetPartStatsMah(consumptionType));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetPartStatsPercent(consumptionType));
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetTotalTimeSecond(statsType, StatsUtils::INVALID_VALUE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsServiceProxy->GetTotalDataBytes(statsType, StatsUtils::INVALID_VALUE));
    std::vector<std::string> dumpArgs;
    dumpArgs.push_back("-batterystats");
    EXPECT_EQ("remote error", g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size()));
}
}