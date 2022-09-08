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

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS::Telephony;
using namespace OHOS;
using namespace std;


void StatsPowerMgrTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsPowerMgrTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsPowerMgrTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsPowerMgrTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsPowerMgrTest_009
 * @tc.desc: test GetTotalTimeSecond function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_009, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
}

/**
 * @tc.name: StatsPowerMgrTest_010
 * @tc.desc: test GetAppStatsMah function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_010, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorGravityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsPowerMgrTest_011
 * @tc.desc: test GetAppStatsPercent function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_011, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_012
 * @tc.desc: test GetAppStatsMah(Sensor Gravity) and GetAppStatsPercent(Sensor Proximity) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_012, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorGravityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_013
 * @tc.desc: test GetTotalTimeSecond function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_013, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsPowerMgrTest_014
 * @tc.desc: test GetAppStatsMah function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_014, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorProximityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsPowerMgrTest_015
 * @tc.desc: test GetAppStatsPercent function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_015, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_016
 * @tc.desc: test GetAppStatsMah(Sensor Proximity) and GetAppStatsPercent(Torch) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_016, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorProximityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}
}