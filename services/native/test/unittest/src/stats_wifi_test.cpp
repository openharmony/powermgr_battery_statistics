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
 * @tc.name: StatsWifiTest_001
 * @tc.desc: test Reset function(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsWifiTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsWifiTest_001: test end";
}

/**
 * @tc.name: StatsWifiTest_002
 * @tc.desc: test GetPartStatsMah function(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_002: test end";
}

/**
 * @tc.name: StatsWifiTest_003
 * @tc.desc: test GetPartStatsPercent function(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsWifiTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_003: test end";
}

/**
 * @tc.name: StatsWifiTest_004
 * @tc.desc: test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;
    
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
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
        <<" StatsWifiTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_004: test end";
}

/**
 * @tc.name: StatsWifiTest_005
 * @tc.desc: test WIFI_CONNECTION event are sent repeatedly, wifi on power consumption(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_005: test end";
}

/**
 * @tc.name: StatsWifiTest_006
 * @tc.desc: test test GetAppStatsMah function, Wifi connection abnormal state test(Wifi is off)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 3;
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsWifiTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_006: test end";
}

/**
 * @tc.name: StatsWifiTest_007
 * @tc.desc: test test GetAppStatsMah function, Wifi connection abnormal state test(Wifi is on)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_007 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_007: test end";
}

/**
 * @tc.name: StatsWifiTest_008
 * @tc.desc: test GetTotalTimeSecond function(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_WIFI_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsWifiTest_008 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_008: test end";
}

/**
 * @tc.name: StatsWifiTest_09
 * @tc.desc: test Reset function(Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_09, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_09: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsWifiTest_09 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsWifiTest_09: test end";
}

/**
 * @tc.name: StatsWifiTest_010
 * @tc.desc: test GetPartStatsMah function(Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    long testWaitTimeSec = 1;
    double deviation = 0.01;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = count * wifiScanAverageMa;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_010: test end";
}

/**
 * @tc.name: StatsWifiTest_011
 * @tc.desc: test GetPartStatsPercent function(Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    double fullPercent = 1;
    double zeroPercent = 0;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsWifiTest_011 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_011: test end";
}

/**
 * @tc.name: StatsWifiTest_012
 * @tc.desc: test GetBatteryStats function(Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    long testWaitTimeSec = 1;
    double deviation = 0.01;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = count * wifiScanAverageMa;
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
        <<" StatsWifiTest_012 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_012: test end";
}

/**
 * @tc.name: StatsWifiTest_013
 * @tc.desc: test GetTotalDataBytes function (Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_013: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int16_t count = 10;
    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_SCAN);
    EXPECT_EQ(data, count) << " StatsWifiTest_013 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsWifiTest_013: test end";
}

/**
 * @tc.name: StatsWifiTest_014
 * @tc.desc: test GetPartStatsMah function(Wifi connection & Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_014: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;
    int16_t count = 10;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double wifiOnPower = testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double wifiScanPower = count * wifiScanAverageMa;

    double expectedPower = wifiOnPower + wifiScanPower;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_014 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_014: test end";
}


/**
 * @tc.name: StatsWifiTest_015
 * @tc.desc: test SetOnBattery function(Wifi connection & Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    int16_t count = 10;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    for (int16_t i = 0; i < count; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
    }
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsWifiTest_015 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_015: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsWifiTest_016
 * @tc.desc: test GetPartStatsMah function with battery changed(Wifi connection)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_016: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double deviation = 0.01;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * wifiOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_016 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_016: test end";
}

/**
 * @tc.name: StatsWifiTest_017
 * @tc.desc: test GetPartStatsMah function with battery changed(Wifi scan)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    long testWaitTimeSec = 1;
    double deviation = 0.01;
    int16_t totalCount = 20;
    int16_t delayCount = 10;
    int16_t startDelayPos = 5;

    for (int16_t i = 0; i < totalCount; i++) {
        HiSysEvent::Write("COMMUNICATION", "WIFI_SCAN", HiSysEvent::EventType::STATISTIC);
        GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
        sleep(testWaitTimeSec);
        if (i == startDelayPos) {
            statsClient.SetOnBattery(false);
            sleep(testWaitTimeSec);
        } else if (i == startDelayPos + delayCount)
        {
            statsClient.SetOnBattery(true);
            sleep(testWaitTimeSec);
        }
    }
    sleep(testWaitTimeSec);

    double expectedPower = (totalCount - delayCount) * wifiScanAverageMa;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsWifiTest_017 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_017: test end";
}

/**
 * @tc.name: StatsWifiTest_018
 * @tc.desc: test GetPartStatsPercent(Wifi on) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsWifiTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsWifiTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(Wifi::WifiConnectionType::CONNECT);
    int32_t stateOff = static_cast<int32_t>(Wifi::WifiConnectionType::DISCONNECT);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("COMMUNICATION", "WIFI_CONNECTION", HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsWifiTest_018 fail due to percent mismatch";

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
        <<" StatsWifiTest_018 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsWifiTest_018: test end";
}
}