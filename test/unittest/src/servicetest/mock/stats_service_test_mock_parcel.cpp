/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "stats_log.h"

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
    STATS_HILOGI(LABEL_TEST, "StatsServiceTestMockParcel_001 start");
    ASSERT_NE(g_statsServiceProxy, nullptr);
    g_statsServiceProxy->ResetIpc();
    g_statsServiceProxy->SetOnBatteryIpc(true);
    ParcelableBatteryStatsList parcelableEntityList;
    int32_t tempError;
    g_statsServiceProxy->GetBatteryStatsIpc(parcelableEntityList, tempError);
    auto infoList = parcelableEntityList.statsList_;
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    double appStatsMah;
    g_statsServiceProxy->GetAppStatsMahIpc(uid, appStatsMah, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, appStatsMah);
    double appStatsPercent;
    g_statsServiceProxy->GetAppStatsPercentIpc(uid, appStatsPercent, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, appStatsPercent);
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    double partStatsMah;
    g_statsServiceProxy->GetPartStatsMahIpc(consumptionType, partStatsMah, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, partStatsMah);
    double partStatsPercent;
    g_statsServiceProxy->GetPartStatsPercentIpc(consumptionType, partStatsPercent, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, partStatsPercent);
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    uint64_t totalTimeSecond;
    g_statsServiceProxy->GetTotalTimeSecondIpc(statsType, StatsUtils::INVALID_VALUE, totalTimeSecond);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, totalTimeSecond);
    uint64_t totalDataBytes;
    g_statsServiceProxy->GetTotalDataBytesIpc(statsType, StatsUtils::INVALID_VALUE, totalDataBytes);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, totalDataBytes);
    STATS_HILOGI(LABEL_TEST, "StatsServiceTestMockParcel_001 end");
}
}