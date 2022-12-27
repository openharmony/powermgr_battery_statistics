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

#include "stats_alarm_test.h"

#include <hisysevent.h>

#include "battery_stats_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;

void StatsAlarmTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsAlarmTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsAlarmTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsAlarmTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsAlarmTest_001
 * @tc.desc: test Reset function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 2;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        usleep(POWER_CONSUMPTION_TRIGGERED_US);
    }

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsAlarmTest_002
 * @tc.desc: test GetPartStatsMah function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_002, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 2;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        usleep(POWER_CONSUMPTION_TRIGGERED_US);
    }

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsAlarmTest_003
 * @tc.desc: test GetAppStatsPercent function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_003, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 2;
    double fullPercent = 1;
    double zeroPercent = 0;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
    }

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsAlarmTest_004
 * @tc.desc: test SetOnBattery function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_004, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 2;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        usleep(POWER_CONSUMPTION_TRIGGERED_US);
    }

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    statsClient.SetOnBattery(true);
}
}