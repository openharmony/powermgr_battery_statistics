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

#include "stats_client_test_mock_parcel.h"
#include "stats_log.h"

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
 * @tc.name: StatsClientTestMockParcel_001
 * @tc.desc: test BatteryStatsClient function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsClientTestMockParcel, StatsClientTestMockParcel_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_001 start");
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
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_001 end");
}

/**
 * @tc.name: StatsClientTestMockParcel_002
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is null)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsClientTestMockParcel, StatsClientTestMockParcel_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_002 start");
    std::shared_ptr<BatteryStatsProxy> sptrStatsProxy = std::make_shared<BatteryStatsProxy>(nullptr);
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
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_002 end");
}

/**
 * @tc.name: StatsClientTestMockParcel_003
 * @tc.desc: test BatteryStatsProxy function(IRemoteObject is not null)
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsClientTestMockParcel, StatsClientTestMockParcel_003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_003 start");
    sptr<IPCObjectStub> sptrRemoteObj = new IPCObjectStub();
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
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_003 end");
}

/**
 * @tc.name: StatsClientTestMockParcel_004
 * @tc.desc: test class BatteryStatsInfo function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsClientTestMockParcel, StatsClientTestMockParcel_004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_004 start");
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
    STATS_HILOGI(LABEL_TEST, "StatsClientTestMockParcel_004 end");
}
}
