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
 * @tc.name: StatsCommunicationTest_001
 * @tc.desc: test Reset function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_001: test start";
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
        << " StatsCommunicationTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_001: test end";
}

/**
 * @tc.name: StatsCommunicationTest_002
 * @tc.desc: test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_002: test start";
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
        <<" StatsCommunicationTest_002 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_002: test end";
}

/**
 * @tc.name: StatsCommunicationTest_003
 * @tc.desc: test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
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
        <<" StatsCommunicationTest_003 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_003: test end";
}

/**
 * @tc.name: StatsCommunicationTest_004
 * @tc.desc: test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_004: test start";
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
        <<" StatsCommunicationTest_004 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_004: test end";
}

/**
 * @tc.name: StatsCommunicationTest_005
 * @tc.desc: test SetOnBattery function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_005: test start";
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

    EXPECT_EQ(expectedPower, actualPower) <<" StatsCommunicationTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_005: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsCommunicationTest_006
 * @tc.desc: test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
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
        <<" StatsCommunicationTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_006: test end";
}

/**
 * @tc.name: StatsCommunicationTest_007
 * @tc.desc: test GetTotalDataBytes and GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsCommunicationTest_007 fail due to reset failed";

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
        <<" StatsCommunicationTest_007 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_007: test end";
}

/**
 * @tc.name: StatsCommunicationTest_008
 * @tc.desc: test GetTotalDataBytes function(Wifi)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsCommunicationTest_008 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_008: test end";
}

/**
 * @tc.name: StatsCommunicationTest_009
 * @tc.desc: test GetTotalDataBytes function(Wifi)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsCommunicationTest_009 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_009: test end";
}

/**
 * @tc.name: StatsCommunicationTest_010
 * @tc.desc: test GetTotalDataBytes(Wifi) and GetTotalTimeSecond(Sensor Gravity) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsCommunicationTest_010 fail due to reset failed";

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
        <<" StatsCommunicationTest_010 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_010: test end";
}

/**
 * @tc.name: StatsCommunicationTest_011
 * @tc.desc: test GetTotalDataBytes(Wifi) and GetTotalTimeSecond(Sensor Proximity) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_TX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " StatsCommunicationTest_011 fail due to reset failed";

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
        <<" StatsCommunicationTest_011 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_011: test end";
}

/**
 * @tc.name: StatsCommunicationTest_012
 * @tc.desc: test GetPartStatsMah(Wifi) and GetPartStatsPercent(Bluetooth) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
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
        <<" StatsCommunicationTest_012 fail due to power mismatch";

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
        <<" StatsCommunicationTest_012 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_012: test end";
}

/**
 * @tc.name: StatsCommunicationTest_013
 * @tc.desc: test GetPartStatsPercent(Wifi) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsCommunicationTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCommunicationTest_013: test start";
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
        <<" StatsCommunicationTest_013 fail due to percent mismatch";

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    sleep(testWaitTimeSec);
    actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCommunicationTest_013 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCommunicationTest_013: test end";
}
}