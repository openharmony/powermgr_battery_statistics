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

#include "stats_client_test.h"

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

void StatsClientTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}


void StatsClientTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsClientTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsClientTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsLocationTest_001
 * @tc.desc: test Reset function(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsLocationTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsLocationTest_001: test end";
}

/**
 * @tc.name: StatsLocationTest_002
 * @tc.desc: test GetAppStatsMah function(Gnss)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsLocationTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_002: test end";
}

/**
 * @tc.name: StatsLocationTest_003
 * @tc.desc: test GetAppStatsPercent function(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsLocationTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_003: test end";
}

/**
 * @tc.name: StatsLocationTest_004
 * @tc.desc: test GNSS_STATE event are sent repeatedly, gnss power consumption(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsLocationTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_004: test end";
}

/**
 * @tc.name: StatsLocationTest_005
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is off)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateInvaildOn = "star";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsLocationTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_005: test end";
}

/**
 * @tc.name: StatsLocationTest_006
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is On)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    std::string stateInvaildOn = "star";
    std::string stateInvaildOff = "stp";
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsLocationTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_006: test end";
}

/**
 * @tc.name: StatsLocationTest_007
 * @tc.desc: test GetTotalTimeSecond function(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_GNSS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsLocationTest_007 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_007: test end";
}

/**
 * @tc.name: StatsLocationTest_008
 * @tc.desc:  test test GetAppStatsMah function with different UID(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo,
        "STATE", stateOff);
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne,
        "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uidOne);
    GTEST_LOG_(INFO) << __func__ << ": expected first uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual first uid consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsLocationTest_008 fail due to first uid power mismatch";

    expectedPower = testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uidTwo);
    GTEST_LOG_(INFO) << __func__ << ": expected second uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual second uid consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsLocationTest_008 fail due to second uid power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_008: test end";
}

/**
 * @tc.name: StatsLocationTest_009
 * @tc.desc: test SetOnBattery function(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsLocationTest_009 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_009: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsLocationTest_010
 * @tc.desc: test GetPartStatsMah function with battery changed(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double deviation = 0.01;

    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("LOCATION", "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid,
        "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * gnssOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsLocationTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_010: test end";
}
}