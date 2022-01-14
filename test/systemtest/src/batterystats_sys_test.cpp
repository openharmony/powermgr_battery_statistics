/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "batterystats_sys_test.h"

#include <unistd.h>
#include <cmath>

#include "string_ex.h"
#include "hisysevent.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"
#include "stats_helper.h"
#include "stats_common.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;

static sptr<BatteryStatsService> g_service;
static const int32_t US_PER_MS = 1000;
static const double MS_PER_SECOND = 1000.0;
static const int32_t SECOND_PER_HOUR = 3600;

void BatterystatsSysTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    g_service = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    g_service->OnStart();
}

void BatterystatsSysTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    g_service->OnStop();
    DelayedStatsSpSingleton<BatteryStatsService>::DestroyInstance();
    delete g_service;
}

void BatterystatsSysTest::SetUp(void)
{
    GTEST_LOG_(INFO) << __func__;
}

void BatterystatsSysTest::TearDown(void)
{
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 *
 * @tc.name: ConsumptionStats_001
 * @tc.desc: test Radio consumption
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest, ConsumptionStats_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ConsumptionStats_001: test start";
    // auto& batteryStatsClient = BatteryStatsClient::GetInstance();
    // batteryStatsClient.Reset();

    g_service->Reset();
    StatsHelper::SetOnBattery(true);

    unsigned int testTimeUs = 2000000;
    int32_t levelBefore = 1;
    int32_t levelAfter = 2;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "RADIO", HiSysEvent::EventType::STATISTIC, "TYPE",
        StatsUtils::STATS_TYPE_RADIO_SCAN, "STATE", StatsUtils::STATS_STATE_NETWORK_SEARCH);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    usleep(testTimeUs);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "RADIO", HiSysEvent::EventType::STATISTIC, "TYPE",
        StatsUtils::STATS_TYPE_RADIO_SCAN, "STATE", StatsUtils::STATS_STATE_NETWORK_IN_SERVICE);

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "RADIO", HiSysEvent::EventType::STATISTIC, "TYPE",
        StatsUtils::STATS_TYPE_RADIO_ON, "STATE", StatsUtils::STATS_STATE_NETWORK_IN_SERVICE, "LEVEL", levelBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    usleep(testTimeUs);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "RADIO", HiSysEvent::EventType::STATISTIC, "TYPE",
        StatsUtils::STATS_TYPE_RADIO_ON, "STATE", StatsUtils::STATS_STATE_NETWORK_IN_SERVICE, "LEVEL", levelAfter);

    double radioOnAverage = g_service->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON,
        levelBefore);
    double radioScanAverage = g_service->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_SCAN);

    GTEST_LOG_(INFO) << __func__ << ": radion on average = " << radioOnAverage << "ma";
    GTEST_LOG_(INFO) << __func__ << ": radion scan average = " << radioScanAverage << "ma";

    long testTimeMs = testTimeUs / US_PER_MS;
    double expectedPowerMah = (radioOnAverage + radioScanAverage) * testTimeMs / MS_PER_SECOND / SECOND_PER_HOUR;
    double actualPowerMah = g_service->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);

    auto radioOnTimeMs = g_service->GetBatteryStatsCore()->GetTotalTimeMs(StatsUtils::STATS_TYPE_RADIO_ON, levelBefore);
    auto radioScanTimeMs = g_service->GetBatteryStatsCore()->GetTotalTimeMs(StatsUtils::STATS_TYPE_RADIO_SCAN);

    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPowerMah << "mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPowerMah << "mAh";

    long testTImeSec = testTimeMs / MS_PER_SECOND;
    long radioScanTimeSec = round(radioScanTimeMs / MS_PER_SECOND);
    long radioOnTimeSec = round(radioOnTimeMs / MS_PER_SECOND);

    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeMs << "ms";
    GTEST_LOG_(INFO) << __func__ << ": actual radio on time = " << radioOnTimeMs << "ms";
    GTEST_LOG_(INFO) << __func__ << ": actual radio scan time = " << radioScanTimeMs << "ms";

    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTImeSec << "s";
    GTEST_LOG_(INFO) << __func__ << ": actual radio on time = " << radioScanTimeSec << "s";
    GTEST_LOG_(INFO) << __func__ << ": actual radio scan time = " << radioOnTimeSec << "s";

    EXPECT_EQ(testTImeSec, radioOnTimeSec) << "ConsumptionStats_001 fail due to radio on time";
    EXPECT_EQ(testTImeSec, radioScanTimeSec) << "ConsumptionStats_001 fail due to radio scan time";
    EXPECT_LE(abs(expectedPowerMah - actualPowerMah), deviation)
        << "ConsumptionStats_001 fail due to consumption deviation larger than 0.01";
    GTEST_LOG_(INFO) << "ConsumptionStats_001: test end";
}
} // namespace