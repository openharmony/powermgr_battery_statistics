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

#include "stats_location_test.h"

#include <hisysevent.h>

#include "battery_stats_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;


void StatsLocationTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}


void StatsLocationTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsLocationTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsLocationTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsLocationTest_001
 * @tc.desc: test Reset function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsLocationTest_002
 * @tc.desc: test GetAppStatsMah function(Gnss)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_002, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsLocationTest_003
 * @tc.desc: test GetAppStatsPercent function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_003, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsLocationTest_004
 * @tc.desc: test GNSS_STATE event are sent repeatedly, gnss power consumption(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_004, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsLocationTest_005
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_005, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateInvaildOn = "star";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsLocationTest_006
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is On)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_006, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    std::string stateInvaildOn = "star";
    std::string stateInvaildOff = "stp";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsLocationTest_007
 * @tc.desc: test GetTotalTimeSecond function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_007, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_GNSS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
}

/**
 * @tc.name: StatsLocationTest_008
 * @tc.desc:  test test GetAppStatsMah function with different UID(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_008, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo,
        "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne,
        "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uidOne);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected first uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual first uid consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    expectedPower = POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uidTwo);
    devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected second uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual second uid consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsLocationTest_009
 * @tc.desc: test SetOnBattery function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_009, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsLocationTest_010
 * @tc.desc: test GetPartStatsMah function with battery changed(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsLocationTest, StatsLocationTest_010, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(false);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(true);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}
}