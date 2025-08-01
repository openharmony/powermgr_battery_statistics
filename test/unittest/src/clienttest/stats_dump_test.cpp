/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "stats_log.h"

#include <display_power_info.h>
#include <hisysevent.h>
#include <running_lock_info.h>
#include <string_ex.h>

#include "battery_stats_client.h"
#include "stats_hisysevent.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

static std::vector<std::string> dumpArgs;

void StatsDumpTest::SetUpTestCase()
{
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}

void StatsDumpTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsDumpTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsDumpTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsDumpTest_001
 * @tc.desc: test Dump function(BATTERY_CHANGED)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_001 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t batteryLevel = 60;
    int32_t batteryChargerType = 2;

    HiSysEventWrite(HiSysEvent::Domain::BATTERY, StatsHiSysEvent::BATTERY_CHANGED,
        HiSysEvent::EventType::STATISTIC, "LEVEL", batteryLevel, "CHARGER", batteryChargerType);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Battery level = ")
        .append(ToString(batteryLevel))
        .append(", Charger type = ")
        .append(ToString(batteryChargerType));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_001 end");
}

/**
 * @tc.name: StatsDumpTest_002
 * @tc.desc: test Dump function(POWER_RUNNINGLOCK)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_002 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10001;
    int32_t pid = 3456;
    int32_t stateLock = 1;
    int32_t stateUnlock = 0;
    int32_t type = static_cast<int32_t>(RunningLockType::RUNNINGLOCK_SCREEN);
    std::string name = " StatsDumpTest_002";

    HiSysEventWrite(HiSysEvent::Domain::POWER, StatsHiSysEvent::POWER_RUNNINGLOCK,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateLock, "TYPE", type, "NAME", name);
    usleep(US_PER_MS);
    HiSysEventWrite(HiSysEvent::Domain::POWER, StatsHiSysEvent::POWER_RUNNINGLOCK,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateUnlock, "TYPE", type, "NAME", name);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("UID = ")
        .append(ToString(uid))
        .append(", PID = ")
        .append(ToString(pid))
        .append(", wakelock type = ")
        .append(ToString(type))
        .append(", wakelock name = ")
        .append(name);

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_002 end");
}

/**
 * @tc.name: StatsDumpTest_003
 * @tc.desc: test Dump function(BACKLIGHT_DISCOUNT)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_003 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t ratio = 100;

    HiSysEventWrite(HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BACKLIGHT_DISCOUNT,
        HiSysEvent::EventType::STATISTIC, "RATIO", ratio);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append(StatsHiSysEvent::BACKLIGHT_DISCOUNT)
        .append(" Ratio = ")
        .append(ToString(ratio));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_003 end");
}

/**
 * @tc.name: StatsDumpTest_004
 * @tc.desc: test Dump function(POWER_WORKSCHEDULER)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_004 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t pid = 3457;
    int32_t uid = 10002;
    int32_t type = 1;
    int32_t interval = 30000;
    int32_t state = 5;

    HiSysEventWrite(HiSysEvent::Domain::STATS, StatsHiSysEvent::POWER_WORKSCHEDULER,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "TYPE", type, "INTERVAL", interval, "STATE", state);
    usleep(POWER_CONSUMPTION_DURATION_US);
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
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_004 end");
}

/**
 * @tc.name: StatsDumpTest_005
 * @tc.desc: test Dump function(POWER_TEMPERATURE)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_005, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_005 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    std::string partName = "Battery";
    int32_t temperature = 40;

    HiSysEventWrite(HiSysEvent::Domain::THERMAL, StatsHiSysEvent::POWER_TEMPERATURE,
        HiSysEvent::EventType::STATISTIC, "NAME", partName, "TEMPERATURE", temperature);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = POWER_TEMPERATURE")
        .append(" Name = ")
        .append(partName);

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_005 end");
}

/**
 * @tc.name: StatsDumpTest_006
 * @tc.desc: test Dump function(START_REMOTE_ABILITY)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_006, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_006 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    std::string callType = "DUBAI_TAG_DIST_SCHED_TO_REMOTE";
    int32_t callUid = 10003;
    int32_t callPid = 3458;
    std::string targetBundle = "TargetBundleName";
    std::string targetAbility = "TargetAbilityName";
    int32_t callAppUid = 9568;
    int32_t result = 1;

    HiSysEventWrite(HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        StatsHiSysEvent::START_REMOTE_ABILITY, HiSysEvent::EventType::BEHAVIOR,
        "CALLING_TYPE", callType, "CALLING_UID", callUid, "CALLING_PID", callPid, "TARGET_BUNDLE", targetBundle,
        "TARGET_ABILITY", targetAbility, "CALLING_APP_UID", callAppUid, "RESULT", result);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = START_REMOTE_ABILITY")
        .append(" Calling Type = ")
        .append(callType);

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_006 end");
}

/**
 * @tc.name: StatsDumpTest_007
 * @tc.desc: test Dump function(THERMAL_ACTION_TRIGGERED)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_007, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_007 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    std::string actionName = "thermallevel";
    int32_t value = 3;
    float ratio = 0.60;
    int32_t beginPos = 0;
    int32_t ratioLen = 4;

    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::THERMAL, StatsHiSysEvent::THERMAL_ACTION_TRIGGERED,
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", actionName, "VALUE", value, "RATIO", ratio);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ACTION_TRIGGERED")
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
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_007 end");
}

/**
 * @tc.name: StatsDumpTest_008
 * @tc.desc: test Dump function(BACKLIGHT_DISCOUNT)
 * @tc.type: FUNC
 */
HWTEST_F (StatsDumpTest, StatsDumpTest_008, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_008 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t type = 100;
    int32_t level = 101;

    HiSysEventWrite(HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::AMBIENT_LIGHT,
        HiSysEvent::EventType::STATISTIC, "TYPE", type, "LEVEL", level);
    usleep(POWER_CONSUMPTION_DURATION_US);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append(StatsHiSysEvent::AMBIENT_LIGHT)
        .append(" Ambient type = ")
        .append(ToString(type))
        .append(" Ambient brightness = ")
        .append(ToString(level));

    std::string actualDebugInfo = statsClient.Dump(dumpArgs);
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_008 end");
}

/**
 * @tc.name: StatsDumpTest_009
 * @tc.desc: Test Dump exceed limit.
 * @tc.type: FUNC
 */
HWTEST_F(StatsDumpTest, StatsDumpTest_009, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_009 function start!");
    size_t size = 1000;
    std::vector<std::string> cmdsList;
    for (size_t i = 0; i < size; i++) {
        std::string cmd = "test_cmd" + std::to_string(i);
        cmdsList.push_back(cmd);
    }
    auto& statsClient = BatteryStatsClient::GetInstance();
    std::string result = statsClient.Dump(cmdsList);

    STATS_HILOGI(LABEL_TEST, "result : %{public}s", result.c_str());
    EXPECT_EQ(true, result == "remote error");
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_009 function end!");
}

/**
 * @tc.name: StatsDumpTest_010
 * @tc.desc: Test Dump limit.
 * @tc.type: FUNC
 */
HWTEST_F(StatsDumpTest, StatsDumpTest_010, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_010 function start!");
    size_t size = 5;
    std::vector<std::string> cmdsList;
    for (size_t i = 0; i < size; i++) {
        std::string cmd = "test_cmd" + std::to_string(i);
        cmdsList.push_back(cmd);
    }
    auto& statsClient = BatteryStatsClient::GetInstance();
    std::string result = statsClient.Dump(cmdsList);

    STATS_HILOGI(LABEL_TEST, "result : %{public}s", result.c_str());
    EXPECT_EQ(true, result.empty());
    STATS_HILOGI(LABEL_TEST, "StatsDumpTest_010 function end!");
}
}