/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_stats_client_test.h"

#include <csignal>
#include <iostream>

#include <bt_def.h>
#include <display_power_info.h>
#include <hisysevent.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <network_search_types.h>
#include <running_lock_info.h>
#include <string_ex.h>
#include <system_ability_definition.h>
#include <wifi_hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_service.h"
#include "stats_common.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;
namespace {
static const int32_t SECOND_PER_HOUR = 3600;
static const int32_t WAIT_TIME = 1;
static std::vector<std::string> dumpArgs;
}

void BatteryStatsClientTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    dumpArgs.push_back("-batterystats");
}

void BatteryStatsClientTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

void BatteryStatsClientTest::SetUp(void)
{
    GTEST_LOG_(INFO) << __func__;
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void BatteryStatsClientTest::TearDown(void)
{
    GTEST_LOG_(INFO) << __func__;
}

/**
 * @tc.name: BatteryStatsClientTest_001
 * @tc.desc: Test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_001: test start";
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
        << " BatteryStatsClientTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_001: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_002
 * @tc.desc: Test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " BatteryStatsClientTest_002 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_002: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_003
 * @tc.desc: Test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " BatteryStatsClientTest_003 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_003: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_004
 * @tc.desc: Test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " BatteryStatsClientTest_004 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_004: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_005
 * @tc.desc: Test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " BatteryStatsClientTest_005 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_005: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_006
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_006 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_006: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_007
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_007 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_007: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_008
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_008 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_008: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_009
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_009 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_009: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_010
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_RADIO_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_010 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_010: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_011
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_011: test start";
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
        <<" BatteryStatsClientTest_011 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_011: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_012
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_WIFI_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" BatteryStatsClientTest_012 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_012: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_013
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_013: test start";
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
        <<" BatteryStatsClientTest_013 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_013: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_014
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_014: test start";
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
        <<" BatteryStatsClientTest_014 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_014: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_015
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
#if GNSS_STATE
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_GPS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" BatteryStatsClientTest_015 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_015: test end";
}
#endif

/**
 * @tc.name: BatteryStatsClientTest_016
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_016: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = 50;
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
        <<" BatteryStatsClientTest_016 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_016: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_017
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = 83;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_017 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_017: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_018
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 100;
    int32_t brightnessAfter = 110;
    double screenOnAverage = 90;
    double screenBrightnessAverage = 2;
    double deviation = 0.1;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * brightnessBefore + screenOnAverage;

    double expectedPower = average * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" BatteryStatsClientTest_018 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_018: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_019
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_019, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_019: test start";
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
        <<" BatteryStatsClientTest_019 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_019: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_020
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_020, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_020: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = 1;
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
        <<" BatteryStatsClientTest_020 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_020: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_021
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_021, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_021: test start";
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
        <<" BatteryStatsClientTest_021 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_021: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_022
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_022, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_022: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
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
        <<" BatteryStatsClientTest_022 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_022: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_023
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_023, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_023: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 110;
    int32_t brightnessAfter = 120;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_023 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_023: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_024
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_024, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_024: test start";
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
        <<" BatteryStatsClientTest_024 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_024: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_025
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_025, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_025: test start";
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
        <<" BatteryStatsClientTest_025 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_025: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_026
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_026, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_026: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = 320;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_026 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_026: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_027
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_027, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_027: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = 810;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_027 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_027: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_028
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_028, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_028: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = 85;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_028 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_028: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_029
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_029, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_029: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = 15;
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
        <<" BatteryStatsClientTest_029 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_029: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_030
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_030, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_030: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = 18;
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
        <<" BatteryStatsClientTest_030 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_030: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_031
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_031, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_031: test start";
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

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_031 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_031: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_032
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_032, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_032: test start";
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

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_032 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_032: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_033
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_033, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_033: test start";
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

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_033 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_033: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_034
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_034, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_034: test start";
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
        <<" BatteryStatsClientTest_034 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_034: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_035
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_035, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_035: test start";
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
        <<" BatteryStatsClientTest_035 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_035: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_036
 * @tc.desc: Test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_036, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_036: test start";
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

    EXPECT_EQ(expectedPower, actualPower) <<" BatteryStatsClientTest_036 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_036: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: BatteryStatsClientTest_037
 * @tc.desc: Test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_037, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_037: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);

    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" BatteryStatsClientTest_037 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_037: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: BatteryStatsClientTest_038
 * @tc.desc: Test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_038, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_038: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 120;
    int32_t brightnessAfter = 130;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" BatteryStatsClientTest_038 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_038: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: BatteryStatsClientTest_039
 * @tc.desc: Test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_039, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_039: test start";
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

    EXPECT_EQ(expectedPower, actualPower) <<" BatteryStatsClientTest_039 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_039: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: BatteryStatsClientTest_040
 * @tc.desc: Test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_040, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_040: test start";
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

    EXPECT_EQ(expectedPower, actualPower) <<" BatteryStatsClientTest_040 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_040: test end";
    statsClient.SetOnBattery(true);
}

/**
 *
 * @tc.name: BatteryStatsClientTest_041
 * @tc.desc: test Dump function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_041, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_041: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t batteryLevel = 60;
    int32_t batteryCurrent = 30;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_BATTERY", HiSysEvent::EventType::STATISTIC, "BATTERY_LEVEL",
        batteryLevel, "CURRENT_NOW", batteryCurrent);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Battery level = ")
        .append(ToString(batteryLevel))
        .append(", current now = ")
        .append(ToString(batteryCurrent))
        .append("ma");

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsClientTest_041 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_041: test end";
}

/**
 *
 * @tc.name: BatteryStatsClientTest_042
 * @tc.desc: test Dump function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_042, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_042: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10001;
    int32_t pid = 3456;
    int32_t stateLock = 1;
    int32_t stateUnlock = 0;
    int32_t type = static_cast<int>(RunningLockType::RUNNINGLOCK_SCREEN);
    std::string name = " BatteryStatsClientTest_001";

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateLock, "TYPE", type, "NAME", name);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateUnlock, "TYPE", type, "NAME", name);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("UID = ")
        .append(ToString(uid))
        .append(", PID = ")
        .append(ToString(pid))
        .append(", wakelock type = ")
        .append(ToString(type))
        .append(", wakelock name = ")
        .append(name)
        .append(", wakelock state = ")
        .append("UNLOCK");

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsClientTest_042 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_042: test end";
}

/**
 *
 * @tc.name: BatteryStatsClientTest_043
 * @tc.desc: test Dump function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_043, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_043: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 130;
    int32_t brightnessAfter = 140;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Screen is in off state, brigntness level = ")
        .append(ToString(brightnessAfter));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsClientTest_043 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_043: test end";
}

/**
 *
 * @tc.name: BatteryStatsClientTest_044
 * @tc.desc: test Dump function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_044, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_044: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t pid = 3457;
    int32_t uid = 10002;
    int32_t type = 1;
    int32_t interval = 30000;
    int32_t state = 5;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_WORKSCHEDULER", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "TYPE", type, "INTERVAL", interval, "STATE", state);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("UID = ")
        .append(ToString(uid))
        .append(", PID = ")
        .append(ToString(pid))
        .append(", work type = ")
        .append(ToString(type))
        .append(", work interval = ")
        .append(ToString(interval))
        .append(", work state = ")
        .append(ToString(state));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsClientTest_044 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_044: test end";
}

/**
 *
 * @tc.name: BatteryStatsClientTest_045
 * @tc.desc: test Dump function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_045, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_045: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    std::string partName = "Battery";
    int32_t temperature = 40;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_TEMPERATURE", HiSysEvent::EventType::STATISTIC, "NAME",
        partName, "TEMPERATURE", temperature);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Part name = ")
        .append(partName)
        .append(", temperature = ")
        .append(ToString(temperature))
        .append("degrees Celsius");

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsClientTest_045 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_045: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_046
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_046, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_046: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = 50;
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
        <<" BatteryStatsClientTest_046 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_046: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_047
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_047, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_047: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = 83;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_WIFI) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_047 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_047: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_048
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_048, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_048: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 140;
    int32_t brightnessAfter = 150;
    double screenOnAverage = 90;
    double screenBrightnessAverage = 2;
    double deviation = 0.1;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * brightnessBefore + screenOnAverage;

    double expectedPower = average * testTimeSec / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" BatteryStatsClientTest_048 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_048: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_049
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_049, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_049: test start";
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
        <<" BatteryStatsClientTest_049 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_049: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_050
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_050, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_050: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = 1;
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
        <<" BatteryStatsClientTest_050 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_050: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_051
 * @tc.desc: Test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_051, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_051: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_051 fail due to reset failed";

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
        <<" BatteryStatsClientTest_051 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_051: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_052
 * @tc.desc: Test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_052, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_052: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_052 fail due to reset failed";

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_WIFI_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" BatteryStatsClientTest_052 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_052: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_053
 * @tc.desc: Test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_053, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_053: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_053 fail due to reset failed";

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
        <<" BatteryStatsClientTest_053 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_053: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_054
 * @tc.desc: Test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_054, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_054: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_054 fail due to reset failed";

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
        <<" BatteryStatsClientTest_054 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_054: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_055
 * @tc.desc: Test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
#if GNSS_STATE
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_055, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_055: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_RADIO_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_055 fail due to reset failed";

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_GPS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" BatteryStatsClientTest_055 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_055: test end";
}
#endif

/**
 * @tc.name: BatteryStatsClientTest_056
 * @tc.desc: Test GetPartStatsMah and GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_056, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_056: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = 50;
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
        <<" BatteryStatsClientTest_056 fail due to power mismatch";

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
        <<" BatteryStatsClientTest_056 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_056: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_057
 * @tc.desc: Test GetPartStatsMah and GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_057, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_057: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = 83;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    double deviation = 0.01;
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::COMMUNICATION, "WIFI_STATE", HiSysEvent::EventType::STATISTIC, "OPER_TYPE",
        stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_057 fail due to power mismatch";

    stateOn = static_cast<int>(bluetooth::BTStateID::STATE_TURN_ON);
    stateOff = static_cast<int>(bluetooth::BTStateID::STATE_TURN_OFF);
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
        <<" BatteryStatsClientTest_057 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_057: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_058
 * @tc.desc: Test GetPartStatsMah and GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_058, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_058: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothOnAverageMa = 1;
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
        <<" BatteryStatsClientTest_058 fail due to power mismatch";

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
        <<" BatteryStatsClientTest_058 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_058: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_059
 * @tc.desc: Test GetAppStatsMah and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_059, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_059: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = 320;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_059 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_059 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_059: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_060
 * @tc.desc: Test GetAppStatsMah and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_060, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_060: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = 810;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_060 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_060 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_060: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_061
 * @tc.desc: Test GetAppStatsMah and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_061, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_061: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = 85;
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_061 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
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
        <<" BatteryStatsClientTest_061 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_061: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_062
 * @tc.desc: Test GetAppStatsMah and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_062, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_062: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = 15;
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
        <<" BatteryStatsClientTest_062 fail due to power mismatch";

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
        <<" BatteryStatsClientTest_062 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_062: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_063
 * @tc.desc: Test GetAppStatsMah and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_063, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_063: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = 18;
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

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_FLASHLIGHT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_063 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_063: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_064
 * @tc.desc: Test GetPartStatsMah and GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_064, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_064: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double phoneOnAverageMa = 50;
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
        <<" BatteryStatsClientTest_064 fail due to power mismatch";

    double audioOnAverageMa = 85;
    int32_t uid = 10003;
    int32_t pid = 3458;
    stateOn = 1;
    stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_AUDIO", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" BatteryStatsClientTest_064 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_064: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_065
 * @tc.desc: Test GetPartStatsPercent and GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_065, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_065: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
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
        <<" BatteryStatsClientTest_065 fail due to percent mismatch";

    int32_t uid = 10003;
    int32_t pid = 3458;
    stateOn = 1;
    stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_CAMERA", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" BatteryStatsClientTest_065 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_065: test end";
}