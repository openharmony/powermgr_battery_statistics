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
#include "battery_stats_parser.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;

static std::shared_ptr<BatteryStatsParser> g_statsParser = nullptr;
static std::vector<std::string> dumpArgs;

static void ParserAveragePowerFile()
{
    if (g_statsParser == nullptr) {
        g_statsParser = std::make_shared<BatteryStatsParser>();
        if (!g_statsParser->Init()) {
            GTEST_LOG_(INFO) << __func__ << ": Battery stats parser initialization failed";
        }
    }
}

void StatsAlarmTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}


void StatsAlarmTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsAlarmTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsAlarmTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
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
    GTEST_LOG_(INFO) << " StatsAlarmTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsAlarmTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsAlarmTest_001: test end";
}

/**
 * @tc.name: StatsAlarmTest_002
 * @tc.desc: test GetPartStatsMah function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAlarmTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;
    double deviation = 0.01;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsAlarmTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsAlarmTest_002: test end";
}

/**
 * @tc.name: StatsAlarmTest_003
 * @tc.desc: test GetAppStatsPercent function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAlarmTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;
    double fullPercent = 1;
    double zeroPercent = 0;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsAlarmTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsAlarmTest_003: test end";
}

/**
 * @tc.name: StatsAlarmTest_004
 * @tc.desc: test SetOnBattery function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI5OKAY
 */
HWTEST_F (StatsAlarmTest, StatsAlarmTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAlarmTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("TIME", "MISC_TIME_STATISTIC_REPORT", HiSysEvent::EventType::STATISTIC, "CALLER_PID", pid,
        "CALLER_UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsAlarmTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsAlarmTest_004: test end";
    statsClient.SetOnBattery(true);
}
}