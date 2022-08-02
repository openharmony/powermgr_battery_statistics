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

#include <display_power_info.h>
#include <hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_parser.h"
#include "power_mgr_client.h"

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

static void SetLastBrightness(int32_t lastBrightness)
{
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);

    GTEST_LOG_(INFO) << __func__ << ": Set last screen brightness value = " << lastBrightness;
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", lastBrightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    auto& statsClient = BatteryStatsClient::GetInstance();
    GTEST_LOG_(INFO) << __func__ << ": Battery stats client reset";
    statsClient.Reset();
}

void StatsClientTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
    auto& pms = PowerMgrClient::GetInstance();
    pms.SuspendDevice();
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
 * @tc.name: StatsDisplayTest_001
 * @tc.desc: test Reset function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsDisplayTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsDisplayTest_001: test end";
}

/**
 * @tc.name: StatsDisplayTest_002
 * @tc.desc: test SetOnBattery function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 120;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsDisplayTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_002: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsDisplayTest_003
 * @tc.desc: test GetPartStatsMah function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

    double expectedPower = average * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_003 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_003: test end";
}

/**
 * @tc.name: StatsDisplayTest_004
 * @tc.desc: test GetPartStatsPercent function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsDisplayTest_004 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_004: test end";
}

/**
 * @tc.name: StatsDisplayTest_005
 * @tc.desc: test GetBatteryStats function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

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
        <<" StatsDisplayTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_005: test end";
}

/**
 * @tc.name: StatsDisplayTest_006
 * @tc.desc: test Last brightness value exists, but no BRIGHTNESS_NIT event notification, screen power consumption
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_006: test end";
}

/**
 * @tc.name: StatsDisplayTest_007
 * @tc.desc: test SCREEN_STATE event are sent repeatedly, screen power consumption
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 2 * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_007 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_007: test end";
}

/**
 * @tc.name: StatsDisplayTest_008
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    int32_t brightness = 100;

    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsDisplayTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_008: test end";
}

/**
 * @tc.name: StatsDisplayTest_009
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_009 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_009: test end";
}

/**
 * @tc.name: StatsDisplayTest_010
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent delay
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double screenOnPower = screenOnAverage * 2 * testTimeSec;
    double lastBrightnessPower = screenBrightnessAverage * lastBrightness * testTimeSec;
    double curBrightnessPower = screenBrightnessAverage * currentBrightness * testTimeSec;

    double expectedPower = (screenOnPower + lastBrightnessPower + curBrightnessPower) / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_010: test end";
}

/**
 * @tc.name: StatsDisplayTest_011
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent with invalid value
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsDisplayTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDisplayTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t invalidBrightness1 = -1;
    int32_t invalidBrightness2 = 300;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    double deviation = 0.1;

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness2);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 3 * testTimeSec / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE((abs(expectedPower - actualPower)) / expectedPower, deviation)
        <<" StatsDisplayTest_011 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsDisplayTest_011: test end";
}
}