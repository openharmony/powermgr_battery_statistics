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

#include "stats_dump_test.h"

#include <display_power_info.h>
#include <hisysevent.h>
#include <running_lock_info.h>
#include <string_ex.h>

#include "battery_stats_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

static std::vector<std::string> dumpArgs;

void StatsDumpTest::SetUpTestCase(void)
{
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}

void StatsDumpTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsDumpTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsDumpTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsDumpTest_001
 * @tc.desc: test Dump function(BATTERY_CHANGED)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_001: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t batteryLevel = 60;
    int32_t batteryChargerType = 2;

    HiSysEvent::Write("BATTERY", "BATTERY_CHANGED", HiSysEvent::EventType::STATISTIC, "LEVEL",
        batteryLevel, "CHARGER", batteryChargerType);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Battery level = ")
        .append(ToString(batteryLevel))
        .append(", Charger type = ")
        .append(ToString(batteryChargerType));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_001 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_001: test end";
}

/**
 * @tc.name: StatsDumpTest_002
 * @tc.desc: test Dump function(POWER_RUNNINGLOCK)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_002: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10001;
    int32_t pid = 3456;
    int32_t stateLock = 1;
    int32_t stateUnlock = 0;
    int32_t type = static_cast<int32_t>(RunningLockType::RUNNINGLOCK_SCREEN);
    std::string name = " StatsDumpTest_002";

    HiSysEvent::Write("POWER", "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateLock, "TYPE", type, "NAME", name);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("POWER", "POWER_RUNNINGLOCK", HiSysEvent::EventType::STATISTIC, "PID", pid,
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

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_002 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_002: test end";
}

/**
 * @tc.name: StatsDumpTest_003
 * @tc.desc: test Dump function(BACKLIGHT_DISCOUNT)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_003: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t ratio = 100;

    HiSysEvent::Write("DISPLAY", "BACKLIGHT_DISCOUNT", HiSysEvent::EventType::STATISTIC, "RATIO", ratio);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append("BACKLIGHT_DISCOUNT")
        .append(" Ratio = ")
        .append(ToString(ratio));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_003 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_003: test end";
}

/**
 * @tc.name: StatsDumpTest_004
 * @tc.desc: test Dump function(POWER_WORKSCHEDULER)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_004: test start";

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

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_004 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_004: test end";
}

/**
 * @tc.name: StatsDumpTest_005
 * @tc.desc: test Dump function(POWER_TEMPERATURE)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_005: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    std::string partName = "Battery";
    int32_t temperature = 40;

    HiSysEvent::Write("THERMAL", "POWER_TEMPERATURE", HiSysEvent::EventType::STATISTIC, "NAME",
        partName, "TEMPERATURE", temperature);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = POWER_TEMPERATURE")
        .append(" Name = ")
        .append(partName);

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_005 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_005: test end";
}

/**
 * @tc.name: StatsDumpTest_006
 * @tc.desc: test Dump function(START_REMOTE_ABILITY)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_006: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    std::string callType = "DUBAI_TAG_DIST_SCHED_TO_REMOTE";
    int32_t callUid = 10003;
    int32_t callPid = 3458;
    std::string targetBundle = "TargetBundleName";
    std::string targetAbility = "TargetAbilityName";
    int32_t callAppUid = 9568;
    int32_t result = 1;


    HiSysEvent::Write("DISTSCHEDULE", "START_REMOTE_ABILITY", HiSysEvent::EventType::BEHAVIOR,
        "CALLING_TYPE", callType, "CALLING_UID", callUid, "CALLING_PID", callPid, "TARGET_BUNDLE", targetBundle,
        "TARGET_ABILITY", targetAbility, "CALLING_APP_UID", callAppUid, "RESULT", result);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = START_REMOTE_ABILITY")
        .append(" Calling Type = ")
        .append(callType);

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_006 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_006: test end";
}

/**
 * @tc.name: StatsDumpTest_007
 * @tc.desc: test Dump function(THERMAL_ACTION_TRIGGERED)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_007: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    std::string actionName = "thermallevel";
    int32_t value = 3;
    float ratio = 0.60;
    int32_t beginPos = 0;
    int32_t ratioLen = 4;

    HiviewDFX::HiSysEvent::Write("THERMAL", "THERMAL_ACTION_TRIGGERED",
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", actionName, "VALUE", value, "RATIO", ratio);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = THERMAL_ACTION_TRIGGERED")
        .append(" Action name = ")
        .append(actionName)
        .append(" Value = ")
        .append(ToString(value))
        .append(" Ratio = ")
        .append(std::to_string(ratio).substr(beginPos, ratioLen));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_007 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_007: test end";
}

/**
 * @tc.name: StatsDumpTest_008
 * @tc.desc: test Dump function(BACKLIGHT_DISCOUNT)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsDumpTest_008: test start";

    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testWaitTimeSec = 1;
    int32_t type = 100;
    int32_t level = 101;

    HiSysEvent::Write("DISPLAY", "AMBIENT_LIGHT", HiSysEvent::EventType::STATISTIC, "TYPE", type, "LEVEL", level);
    sleep(testWaitTimeSec);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append("AMBIENT_LIGHT")
        .append(" Ambient type = ")
        .append(ToString(type))
        .append(" Ambient brightness = ")
        .append(ToString(level));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);

    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;

    auto index = actualDebugInfo.find(expectedDebugInfo);

    EXPECT_TRUE(index != string::npos) << " StatsDumpTest_008 fail due to not found related debug info";
    GTEST_LOG_(INFO) << " StatsDumpTest_008: test end";
}
}