/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "batterystats_sys_test.h"

#include <cmath>
#include <unistd.h>

#include "display_info.h"
#include "hisysevent.h"
#include "running_lock_info.h"
#include "string_ex.h"

#include "battery_stats_service.h"
#include "stats_common.h"
#include "stats_helper.h"
#include "stats_hilog_wrapper.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;

namespace {
static sptr<BatteryStatsService> g_service;
static const int32_t SECOND_PER_HOUR = 3600;
static const int32_t MS_PER_SECOND = 1000;
}

void BatterystatsSysTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    g_service = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    g_service->OnStart();
}

void BatterystatsSysTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    g_service->OnStop();
    DelayedStatsSpSingleton<BatteryStatsService>::DestroyInstance();
    delete g_service;
}

void BatterystatsSysTest::SetUp(void)
{
    GTEST_LOG_(INFO) << __func__;
}

void BatterystatsSysTest::TearDown(void)
{
    GTEST_LOG_(INFO) << __func__;
}

/**
 *
 * @tc.name:  BatteryStatsSysTest_001
 * @tc.desc: test Wakelock consumption
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest,  BatteryStatsSysTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_001: test start";

    g_service->Reset();
    StatsHelper::SetOnBattery(true);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10001;
    int32_t pid = 3456;
    int32_t stateLock = 1;
    int32_t stateUnlock = 0;
    int32_t type = static_cast<int>(RunningLockType::RUNNINGLOCK_SCREEN);
    std::string name = " BatteryStatsSysTest_001";
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateLock, "TYPE", type, "NAME", name);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateUnlock, "TYPE", type, "NAME", name);
    sleep(testWaitTimeSec);
    double wakelockAverage = g_service->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_AWAKE);

    double expectedPowerMah = wakelockAverage * testTimeSec / SECOND_PER_HOUR;
    double actualPowerMah = g_service->GetAppStatsMah(uid);
    long  actualTimeSec = g_service->GetTotalTimeSecond(StatsUtils::STATS_TYPE_WAKELOCK_HOLD, uid);

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

    std::string actualDebugInfo;
    g_service->GetBatteryStatsCore()->GetDebugInfo(actualDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPowerMah << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPowerMah << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTimeSec << " seconds";

    EXPECT_LE(abs(expectedPowerMah - actualPowerMah), deviation)
        << " BatteryStatsSysTest_001 fail due to consumption deviation larger than 0.01";
    EXPECT_EQ(testTimeSec,  actualTimeSec) << " BatteryStatsSysTest_001 fail due to wakelock time";
    EXPECT_TRUE(index != string::npos) << " BatteryStatsSysTest_001 fail due to not found Wakelock related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_001: test end";
}

/**
 *
 * @tc.name:  BatteryStatsSysTest_002
 * @tc.desc: test Screen consumption
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest,  BatteryStatsSysTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_002: test start";

    g_service->Reset();
    StatsHelper::SetOnBattery(true);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_ON);
    int32_t stateOff = static_cast<int>(DisplayPowerMgr::DisplayState::DISPLAY_OFF);
    int32_t brightnessBefore = 100;
    int32_t brightnessAfter = 120;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOn,
        "BRIGHTNESS", brightnessBefore);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SCREEN", HiSysEvent::EventType::STATISTIC, "STATE", stateOff,
        "BRIGHTNESS", brightnessAfter);
    sleep(testWaitTimeSec);
    double screenOnAverage = g_service->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    double screenBrightnessAverage =
        g_service->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);

    double average = screenBrightnessAverage * brightnessBefore + screenOnAverage;

    double expectedPowerMah = average * testTimeSec / SECOND_PER_HOUR;
    double actualPowerMah = g_service->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    long  actualTimeMs = g_service->GetBatteryStatsCore()->GetTotalTimeMs(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
        brightnessBefore);
    long actualTimeSec = round(actualTimeMs / MS_PER_SECOND);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Screen is in off state, brigntness level = ")
        .append(ToString(brightnessAfter));

    std::string actualDebugInfo;
    g_service->GetBatteryStatsCore()->GetDebugInfo(actualDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPowerMah << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPowerMah << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTimeSec << " seconds";

    EXPECT_LE(abs(expectedPowerMah - actualPowerMah), deviation)
        << " BatteryStatsSysTest_002 fail due to consumption deviation larger than 0.01";
    EXPECT_EQ(testTimeSec,  actualTimeSec) << " BatteryStatsSysTest_002 fail due to wakelock time";
    EXPECT_TRUE(index != string::npos) << " BatteryStatsSysTest_003 fail due to not found battery related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_002: test end";
}

/**
 *
 * @tc.name:  BatteryStatsSysTest_003
 * @tc.desc: test Battery stats event
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest,  BatteryStatsSysTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_003: test start";

    g_service->Reset();

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

    std::string actualDebugInfo;
    g_service->GetBatteryStatsCore()->GetDebugInfo(actualDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsSysTest_003 fail due to not found battery related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_003: test end";
}

/**
 *
 * @tc.name:  BatteryStatsSysTest_004
 * @tc.desc: test Thermal stats event
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest,  BatteryStatsSysTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_004: test start";

    g_service->Reset();

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

    std::string actualDebugInfo;
    g_service->GetBatteryStatsCore()->GetDebugInfo(actualDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsSysTest_004 fail due to not found thermal related debug info";
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_004: test end";
}

/**
 *
 * @tc.name:  BatteryStatsSysTest_005
 * @tc.desc: test WorkScheduler stats event
 * @tc.type: FUNC
 */
HWTEST_F (BatterystatsSysTest,  BatteryStatsSysTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_005: test start";

    g_service->Reset();

    long testWaitTimeSec = 1;
    int32_t pid = 3457;
    int32_t uid = 10002;
    int32_t type = 1;
    int32_t interval = 30000;
    int32_t state = 5;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_WORkSCHEDULER", HiSysEvent::EventType::STATISTIC, "PID", pid,
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

    std::string actualDebugInfo;
    g_service->GetBatteryStatsCore()->GetDebugInfo(actualDebugInfo);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " BatteryStatsSysTest_005 fail due to no WorkScheduler debug info found";
    GTEST_LOG_(INFO) << " BatteryStatsSysTest_005: test end";
}