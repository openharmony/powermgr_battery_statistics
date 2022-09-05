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

#include "stats_powermgr_test.h"

#include <bt_def.h>
#include <call_manager_inner_type.h>
#include <hisysevent.h>
#include <wifi_hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_parser.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS::Telephony;
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

void StatsPowerMgrTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}

void StatsPowerMgrTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsPowerMgrTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsPowerMgrTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsPowerMgrTest_009
 * @tc.desc: test GetTotalTimeSecond function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_009, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_010, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_011, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_012, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_013, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_014, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_015, TestSize.Level0)
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
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_016, TestSize.Level0)
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
}