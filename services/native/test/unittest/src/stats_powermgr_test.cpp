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

#include <bt_def.h>
#include <hisysevent.h>
#include <network_search_types.h>
#include <wifi_hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_parser.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
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
 * @tc.name: StatsPowerMgrTest_001
 * @tc.desc: test Reset function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);
    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsPowerMgrTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_001: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_002
 * @tc.desc: test GetTotalTimeSecond function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsPowerMgrTest_002 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_002: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_003
 * @tc.desc: test GetPartStatsMah function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ACTIVE);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_003 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_003: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_004
 * @tc.desc: test GetPartStatsPercent function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_004 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_004: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_005
 * @tc.desc: test GetBatteryStats function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ACTIVE);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_PHONE) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_005: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_006
 * @tc.desc: test SetOnBattery function(Phone)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsPowerMgrTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_006: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsPowerMgrTest_007
 * @tc.desc: test GetPartStatsMah(Phone) and GetPartStatsPercent(Wifi) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ACTIVE);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_007 fail due to power mismatch";

    stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_007 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_007: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_008
 * @tc.desc: test GetPartStatsMah(Phone) and GetAppStatsMah(Audio) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ACTIVE);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_008 fail due to power mismatch";

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_008: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_009
 * @tc.desc: test GetTotalTimeSecond function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsPowerMgrTest_009 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_009: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_010
 * @tc.desc: test GetAppStatsMah function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * sensorGravityOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_010: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_011
 * @tc.desc: test GetAppStatsPercent function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_011 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_011: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_012
 * @tc.desc: test GetAppStatsMah(Sensor Gravity) and GetAppStatsPercent(Sensor Proximity) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * sensorGravityOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_012 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_012 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_012: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_013
 * @tc.desc: test GetTotalTimeSecond function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_013: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsPowerMgrTest_013 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_013: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_014
 * @tc.desc: test GetAppStatsMah function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_014: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * sensorProximityOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_014 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_014: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_015
 * @tc.desc: test GetAppStatsPercent function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_015 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_015: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_016
 * @tc.desc: test GetAppStatsMah(Sensor Proximity) and GetAppStatsPercent(Torch) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_016: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * sensorProximityOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_063 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_016 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_016: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_017
 * @tc.desc: test GetPartStatsMah function(Radio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateScan = static_cast<int>(Telephony::RegServiceState::REG_STATE_SEARCH);
    int32_t stateInService = static_cast<int>(Telephony::RegServiceState::REG_STATE_IN_SERVICE);
    int32_t signalBefore = 0;
    int32_t signalAfter = 1;
    double radioLevel0OnAverage = 160;
    double radioLevel1OnAverage = 240;
    double radioScanAverage = 30;
    double deviation = 0.1;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE", stateScan,
        "SIGNAL", signalBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE",
        stateInService, "SIGNAL", signalAfter);
    sleep(testWaitTimeSec);

    double expectedLevelPower = radioLevel0OnAverage * testTimeSec + radioLevel1OnAverage * testWaitTimeSec;
    double expectedScanPower = radioScanAverage * testTimeSec;
    double expectedPower = (expectedLevelPower + expectedScanPower) / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsPowerMgrTest_017 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_017: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_018
 * @tc.desc: test GetPartStatsPercent function(Radio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateScan = static_cast<int>(Telephony::RegServiceState::REG_STATE_SEARCH);
    int32_t stateInService = static_cast<int>(Telephony::RegServiceState::REG_STATE_IN_SERVICE);
    int32_t signalBefore = 1;
    int32_t signalAfter = 2;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE", stateScan,
        "SIGNAL", signalBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE",
        stateInService, "SIGNAL", signalAfter);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_018 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_018: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_019
 * @tc.desc: test GetBatteryStats function(Radio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_019, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_019: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateScan = static_cast<int>(Telephony::RegServiceState::REG_STATE_SEARCH);
    int32_t stateInService = static_cast<int>(Telephony::RegServiceState::REG_STATE_IN_SERVICE);
    int32_t signalBefore = 3;
    int32_t signalAfter = 4;
    double radioLevel3OnAverage = 390;
    double radioLevel4OnAverage = 470;
    double radioScanAverage = 30;
    double deviation = 0.1;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE", stateScan,
        "SIGNAL", signalBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE",
        stateInService, "SIGNAL", signalAfter);
    sleep(testWaitTimeSec);

    double expectedLevelPower = radioLevel3OnAverage * testTimeSec + radioLevel4OnAverage * testWaitTimeSec;
    double expectedScanPower = radioScanAverage * testTimeSec;
    double expectedPower = (expectedLevelPower + expectedScanPower) / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_RADIO) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsPowerMgrTest_019 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_019: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_020
 * @tc.desc: test GetTotalDataBytes function(Radio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_020, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_020: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_RADIO_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsPowerMgrTest_020 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_020: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_021
 * @tc.desc: test SetOnBattery function(Radio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_021, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_021: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateScan = static_cast<int>(Telephony::RegServiceState::REG_STATE_SEARCH);
    int32_t stateInService = static_cast<int>(Telephony::RegServiceState::REG_STATE_IN_SERVICE);
    int32_t signalBefore = 2;
    int32_t signalAfter = 3;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE", stateScan,
        "SIGNAL", signalBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RADIO", HiSysEvent::EventType::STATISTIC, "STATE",
        stateInService, "SIGNAL", signalAfter);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsPowerMgrTest_021 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_021: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsPowerMgrTest_022
 * @tc.desc: test GetPartStatsMah function(BlueTooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_022, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_022: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_022 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_022: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_023
 * @tc.desc: test GetPartStatsPercent function(Bluetooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_023, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_023: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_023 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_023: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_024
 * @tc.desc: test GetBatteryStats function(Bluetooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_024, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_024: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothOnAverageMa / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_024 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_024: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_025
 * @tc.desc: test GetTotalDataBytes function(Bluetooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_025, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_025: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsPowerMgrTest_025 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_025: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_026
 * @tc.desc: test GetTotalDataBytes function(Bluetooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_026, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_026: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsPowerMgrTest_026 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_026: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_027
 * @tc.desc: test SetOnBattery function(Bluetooth)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_027, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_027: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsPowerMgrTest_027 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_027: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsPowerMgrTest_028
 * @tc.desc: test GetTotalDataBytes(Bluetooth) and GetTotalTimeSecond(Phone) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_028, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_028: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsPowerMgrTest_028 fail due to reset failed";

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsPowerMgrTest_028 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_028: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_029
 * @tc.desc: test GetPartStatsMah(Bluetooth) and GetPartStatsPercent(Phone) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_029, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_029: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "BLUETOOTH_BR_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "BR_STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_029 fail due to power mismatch";

    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_PHONE", HiSysEvent::EventType::STATISTIC, "STATE",
        stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_029 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_029: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_030
 * @tc.desc: test Reset function(Alarm)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_030, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_030: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "ALARM_TRIGGER", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid);
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
        << " StatsPowerMgrTest_030 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_030: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_031
 * @tc.desc: test Reset function(Alarm)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_031, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_031: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;
    double deviation = 0.01;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "ALARM_TRIGGER", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPowerMgrTest_031 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_031: test end";
}

/**
 * @tc.name: StatsPowerMgrTest_032
 * @tc.desc: test GetAppStatsPercent function(Alarm)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsPowerMgrTest_032, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_032: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;
    double fullPercent = 1;
    double zeroPercent = 0;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "ALARM_TRIGGER", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPowerMgrTest_032 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPowerMgrTest_032: test end";
}
}