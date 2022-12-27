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

#include "stats_display_test.h"

#include <display_power_info.h>
#include <hisysevent.h>

#include "stats_log.h"
#include "battery_stats_client.h"
#include "power_mgr_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;


static void SetLastBrightness(int32_t lastBrightness)
{
    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);

    GTEST_LOG_(INFO) << __func__ << ": Set last screen brightness value = " << lastBrightness;
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", lastBrightness);
    usleep(StatsTest::POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    auto& statsClient = BatteryStatsClient::GetInstance();
    GTEST_LOG_(INFO) << __func__ << ": Battery stats client reset";
    statsClient.Reset();
}

void StatsDisplayTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
    auto& pms = PowerMgrClient::GetInstance();
    pms.SuspendDevice();
}

void StatsDisplayTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsDisplayTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsDisplayTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsDisplayTest_001
 * @tc.desc: test Reset function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsDisplayTest_002
 * @tc.desc: test SetOnBattery function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_002, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 120;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsDisplayTest_003
 * @tc.desc: test GetPartStatsMah function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_003, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

    double expectedPower = average * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_004
 * @tc.desc: test GetPartStatsPercent function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_004, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsDisplayTest_005
 * @tc.desc: test GetBatteryStats function(Screen)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_005, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

    double expectedPower = average * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_006
 * @tc.desc: test Last brightness value exists, but no BRIGHTNESS_NIT event notification, screen power consumption
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_006, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_007
 * @tc.desc: test SCREEN_STATE event are sent repeatedly, screen power consumption
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_007, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 2 * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_008
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_008, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t brightness = 100;

    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(POWER_CONSUMPTION_DURATION_US);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsDisplayTest_009
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_009, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    usleep(POWER_CONSUMPTION_DURATION_US);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_010
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent delay
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_010, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double screenOnPower = screenOnAverage * 2 * POWER_CONSUMPTION_DURATION_US;
    double lastBrightnessPower = screenBrightnessAverage * lastBrightness * POWER_CONSUMPTION_DURATION_US;
    double curBrightnessPower = screenBrightnessAverage * currentBrightness * POWER_CONSUMPTION_DURATION_US;

    double expectedPower = (screenOnPower + lastBrightnessPower + curBrightnessPower) / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_011
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent with invalid value
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_011, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t invalidBrightness1 = -1;
    int32_t invalidBrightness2 = 300;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness2);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 3 * POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsDisplayTest_012
 * @tc.desc: test GetTotalTimeSecond function(CURRENT_SCREEN_ON & CURRENT_SCREEN_BRIGHTNESS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDisplayTest, StatsDisplayTest_012, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsDisplayTest_012 is start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t brightnessBegin = 0;
    int32_t brightness = 0;
    int32_t count = 50;
    int32_t step = 3;

    SetLastBrightness(lastBrightness);
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    for (int32_t i = 0; i < count; i++) {
        brightness = brightnessBegin + step * i;
        auto ret = HiSysEvent::Write("DISPLAY", "BRIGHTNESS_NIT", HiSysEvent::EventType::STATISTIC,
            "BRIGHTNESS", brightness);
        STATS_HILOGD(LABEL_TEST, "HiSysEventWrite return: %{public}d", ret);
        usleep(POWER_CONSUMPTION_DURATION_US);
    }
    HiSysEvent::Write("DISPLAY", "SCREEN_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    long expectScreenOnTime = POWER_CONSUMPTION_DURATION_US * (count + 1) / US_PER_SECOND;
    long screenOnTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SCREEN_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected screen on time = " << expectScreenOnTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual screen on time = " <<  screenOnTime << " seconds";
    EXPECT_EQ(expectScreenOnTime, screenOnTime);

    long expectBrightnessTime = POWER_CONSUMPTION_DURATION_US * (count + 1) / US_PER_SECOND;
    long brightnessTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS);
    GTEST_LOG_(INFO) << __func__ << ": expected screen on time = " << expectBrightnessTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual screen on time = " <<  brightnessTime << " seconds";
    EXPECT_EQ(expectBrightnessTime, brightnessTime);
    STATS_HILOGD(LABEL_TEST, "StatsDisplayTest_012 is end");
}
}