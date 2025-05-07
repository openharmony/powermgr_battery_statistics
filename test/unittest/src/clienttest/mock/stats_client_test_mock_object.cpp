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

#include "stats_client_test_mock_object.h"
#include "stats_log.h"

#include "battery_stats_proxy.h"
#include "mock_stats_remote_object.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
/**
 * @tc.name: StatsClientTestMockObject_001
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is mock, "SendRequest" function return false)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsClientTestMockObject, StatsClientTestMockObject_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockObject_001 start");
    sptr<MockStatsRemoteObject> sptrRemoteObj = new MockStatsRemoteObject();
    EXPECT_TRUE(sptrRemoteObj != nullptr);
    std::shared_ptr<BatteryStatsProxy> sptrStatsProxy = std::make_shared<BatteryStatsProxy>(sptrRemoteObj);
    EXPECT_TRUE(sptrStatsProxy != nullptr);
    sptrStatsProxy->ResetIpc();
    sptrStatsProxy->SetOnBatteryIpc(true);
    ParcelableBatteryStatsList parcelableEntityList;
    int32_t tempError;
    sptrStatsProxy->GetBatteryStatsIpc(parcelableEntityList, tempError);
    auto infoList = parcelableEntityList.statsList_;
    EXPECT_TRUE(infoList.empty());
    int32_t uid = 1004;
    double appStatsMah;
    sptrStatsProxy->GetAppStatsMahIpc(uid, appStatsMah, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, appStatsMah);
    double appStatsPercent;
    sptrStatsProxy->GetAppStatsPercentIpc(uid, appStatsPercent, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, appStatsPercent);
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    double partStatsMah;
    sptrStatsProxy->GetPartStatsMahIpc(consumptionType, partStatsMah, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, partStatsMah);
    double partStatsPercent;
    sptrStatsProxy->GetPartStatsPercentIpc(consumptionType, partStatsPercent, tempError);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, partStatsPercent);
    StatsUtils::StatsType statsType = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    uint64_t totalTimeSecond;
    sptrStatsProxy->GetTotalTimeSecondIpc(statsType, StatsUtils::INVALID_VALUE, totalTimeSecond);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, totalTimeSecond);
    uint64_t totalDataBytes;
    sptrStatsProxy->GetTotalDataBytesIpc(statsType, StatsUtils::INVALID_VALUE, totalDataBytes);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, totalDataBytes);
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockObject_001 end");
}
}
