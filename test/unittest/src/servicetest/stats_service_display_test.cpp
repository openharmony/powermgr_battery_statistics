/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "stats_service_display_test.h"

#include <display_power_info.h>
#include <hisysevent.h>

#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "power_mgr_client.h"
#include "stats_hisysevent.h"
#include "stats_service_test_proxy.h"
#include "stats_service_write_event.h"

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
} // namespace

static void SetLastBrightness(int32_t lastBrightness)
{
    auto statsService = BatteryStatsService::GetInstance();
    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);

    GTEST_LOG_(INFO) << __func__ << ": Set last screen brightness value = " << lastBrightness;
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", lastBrightness);
    usleep(StatsTest::SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    GTEST_LOG_(INFO) << __func__ << ": Battery stats client reset";
    g_statsServiceProxy->Reset();
}

void StatsServiceDisplayTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    g_statsService = BatteryStatsService::GetInstance();
    g_statsService->OnStart();

    if (g_statsService->listenerPtr_ == nullptr) {
        g_statsService->listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }

    if (g_statsServiceProxy == nullptr) {
        g_statsServiceProxy = std::make_shared<StatsServiceTestProxy>(g_statsService);
    }

    auto& pms = PowerMgrClient::GetInstance();
    pms.SuspendDevice();
}

void StatsServiceDisplayTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceDisplayTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceDisplayTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceDisplayTest_001
 * @tc.desc: test Reset function(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double powerMahBefore = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceDisplayTest_002
 * @tc.desc: test SetOnBattery function(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();
    g_statsServiceProxy->SetOnBattery(false);

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 120;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    g_statsServiceProxy->SetOnBattery(true);
}

/**
 * @tc.name: StatsServiceDisplayTest_003
 * @tc.desc: test GetPartStatsMah function(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

    double expectedPower = average * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_004
 * @tc.desc: test GetPartStatsPercent function(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double actualPercent = g_statsServiceProxy->GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceDisplayTest_005
 * @tc.desc: test GetBatteryStats function(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * brightness + screenOnAverage;

    double expectedPower = average * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = g_statsServiceProxy->GetBatteryStats();
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
 * @tc.name: StatsServiceDisplayTest_006
 * @tc.desc: test Last brightness value exists, but no BRIGHTNESS_NIT event notification, screen power consumption
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_007
 * @tc.desc: test SCREEN_STATE event are sent repeatedly, screen power consumption
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 2 * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_008
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_008, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t brightness = 100;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", brightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsServiceDisplayTest_009
 * @tc.desc: test Screen is off, BRIGHTNESS_NIT event is invalid
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_009, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_010
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent delay
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_010, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t currentBrightness = 200;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", currentBrightness);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double screenOnPower = screenOnAverage * 2 * SERVICE_POWER_CONSUMPTION_DURATION_US;
    double lastBrightnessPower = screenBrightnessAverage * lastBrightness * SERVICE_POWER_CONSUMPTION_DURATION_US;
    double curBrightnessPower = screenBrightnessAverage * currentBrightness * SERVICE_POWER_CONSUMPTION_DURATION_US;

    double expectedPower = (screenOnPower + lastBrightnessPower + curBrightnessPower) / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_011
 * @tc.desc: test Screen is on, BRIGHTNESS_NIT event are sent with invalid value
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_011, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int32_t>(OHOS::DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t lastBrightness = 100;
    int32_t invalidBrightness1 = -1;
    int32_t invalidBrightness2 = 300;
    double screenOnAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    SetLastBrightness(lastBrightness);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BRIGHTNESS_NIT,
        HiSysEvent::EventType::STATISTIC, "BRIGHTNESS", invalidBrightness2);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::SCREEN_STATE,
        HiSysEvent::EventType::STATISTIC, "STATE", stateOff);

    double average = screenBrightnessAverage * lastBrightness + screenOnAverage;

    double expectedPower = average * 3 * SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceDisplayTest_012
 * @tc.desc: test send hisysevent with missing information(Screen)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDisplayTest, StatsServiceDisplayTest_012, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY,
    StatsHiSysEvent::SCREEN_STATE, HiSysEvent::EventType::STATISTIC);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY,
    StatsHiSysEvent::BRIGHTNESS_NIT, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::DISPLAY,
    StatsHiSysEvent::SCREEN_STATE, HiSysEvent::EventType::STATISTIC);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}
}