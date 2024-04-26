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

#include "stats_service_dump_test.h"

#include <display_power_info.h>
#include <hisysevent.h>
#include <running_lock_info.h>
#include <string_ex.h>

#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "stats_hisysevent.h"
#include "stats_service_test_proxy.h"
#include "stats_service_write_event.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
static std::vector<std::string> dumpArgs;
} // namespace

void StatsServiceDumpTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    g_statsService = BatteryStatsService::GetInstance();
    g_statsService->OnStart();
    g_statsService->isBootCompleted_ = true;

    if (g_statsService->listenerPtr_ == nullptr) {
        g_statsService->listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }

    if (g_statsServiceProxy == nullptr) {
        g_statsServiceProxy = std::make_shared<StatsServiceTestProxy>(g_statsService);
    }
    dumpArgs.push_back("-batterystats");
}

void StatsServiceDumpTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceDumpTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceDumpTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceDumpTest_001
 * @tc.desc: test Dump function(BATTERY_CHANGED)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t batteryLevel = 60;
    int32_t batteryChargerType = 2;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::BATTERY, StatsHiSysEvent::BATTERY_CHANGED, HiSysEvent::EventType::STATISTIC, "LEVEL",
        batteryLevel, "CHARGER", batteryChargerType);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Battery level = ")
        .append(ToString(batteryLevel))
        .append(", Charger type = ")
        .append(ToString(batteryChargerType));

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::BATTERY, StatsHiSysEvent::BATTERY_CHANGED, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append("Battery level = ").append(ToString(-1));
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_002
 * @tc.desc: test Dump function(POWER_RUNNINGLOCK)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10001;
    int32_t pid = 3456;
    int32_t stateLock = 1;
    int32_t stateUnlock = 0;
    int32_t type = static_cast<int32_t>(RunningLockType::RUNNINGLOCK_SCREEN);
    std::string name = " StatsServiceDumpTest_002";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::POWER, StatsHiSysEvent::POWER_RUNNINGLOCK, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateLock, "TYPE", type, "NAME", name);
    usleep(US_PER_MS);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::POWER, StatsHiSysEvent::POWER_RUNNINGLOCK, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateUnlock, "TYPE", type, "NAME", name);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("UID = ")
        .append(ToString(uid))
        .append(", PID = ")
        .append(ToString(pid))
        .append(", wakelock type = ")
        .append(ToString(type))
        .append(", wakelock name = ")
        .append(name);

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::POWER, StatsHiSysEvent::POWER_RUNNINGLOCK, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append(", wakelock name = ").append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_003
 * @tc.desc: test Dump function(BACKLIGHT_DISCOUNT)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t ratio = 100;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BACKLIGHT_DISCOUNT,
        HiSysEvent::EventType::STATISTIC, "RATIO", ratio);
    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append(StatsHiSysEvent::BACKLIGHT_DISCOUNT)
        .append(" Ratio = ")
        .append(ToString(ratio));

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::BACKLIGHT_DISCOUNT, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append(" Ratio = ").append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_004
 * @tc.desc: test Dump function(POWER_WORKSCHEDULER)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t pid = 3457;
    int32_t uid = 10002;
    int32_t type = 1;
    int32_t interval = 30000;
    int32_t state = 5;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::STATS, StatsHiSysEvent::POWER_WORKSCHEDULER,
        HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "TYPE", type, "INTERVAL", interval, "STATE", state);

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

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::STATS, StatsHiSysEvent::POWER_WORKSCHEDULER, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append(", work interval = ").append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_005
 * @tc.desc: test Dump function(POWER_TEMPERATURE)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    std::string partName = "Battery";
    int32_t temperature = 40;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::THERMAL, StatsHiSysEvent::POWER_TEMPERATURE, HiSysEvent::EventType::STATISTIC, "NAME",
        partName, "TEMPERATURE", temperature);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = POWER_TEMPERATURE")
        .append(" Name = ")
        .append(partName);

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::THERMAL, StatsHiSysEvent::POWER_TEMPERATURE, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append("Additional debug info: ")
        .append("Event name = POWER_TEMPERATURE")
        .append(" Name = ")
        .append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_006
 * @tc.desc: test Dump function(START_REMOTE_ABILITY)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    std::string callType = "DUBAI_TAG_DIST_SCHED_TO_REMOTE";
    int32_t callUid = 10003;
    int32_t callPid = 3458;
    std::string targetBundle = "TargetBundleName";
    std::string targetAbility = "TargetAbilityName";
    int32_t callAppUid = 9568;
    int32_t result = 1;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        StatsHiSysEvent::START_REMOTE_ABILITY, HiSysEvent::EventType::BEHAVIOR,
        "CALLING_TYPE", callType, "CALLING_UID", callUid, "CALLING_PID", callPid, "TARGET_BUNDLE", targetBundle,
        "TARGET_ABILITY", targetAbility, "CALLING_APP_UID", callAppUid, "RESULT", result);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = START_REMOTE_ABILITY")
        .append(" Calling Type = ")
        .append(callType);

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISTRIBUTED_SCHEDULE,
        StatsHiSysEvent::START_REMOTE_ABILITY, HiSysEvent::EventType::BEHAVIOR);
    std::string expectedMissInfo;
    expectedMissInfo.append("Additional debug info: ")
        .append("Event name = START_REMOTE_ABILITY")
        .append(" Calling Type = ")
        .append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_007
 * @tc.desc: test Dump function(THERMAL_ACTION_TRIGGERED)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    std::string actionName = "thermallevel";
    int32_t value = 3;
    float ratio = 0.60;
    int32_t beginPos = 0;
    int32_t ratioLen = 4;

    StatsWriteHiSysEvent(statsService,
        HiviewDFX::HiSysEvent::Domain::THERMAL, StatsHiSysEvent::THERMAL_ACTION_TRIGGERED,
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "ACTION", actionName, "VALUE", value, "RATIO", ratio);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ACTION_TRIGGERED")
        .append(" Action name = ")
        .append(actionName)
        .append(" Value = ")
        .append(ToString(value))
        .append(" Ratio = ")
        .append(std::to_string(ratio).substr(beginPos, ratioLen));

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::THERMAL, StatsHiSysEvent::THERMAL_ACTION_TRIGGERED, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append("Additional debug info: ")
        .append("Event name = ACTION_TRIGGERED")
        .append(" Action name = ")
        .append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_008
 * @tc.desc: test Dump function(AMBIENT_LIGHT)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_008, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t type = 100;
    int32_t level = 101;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::AMBIENT_LIGHT,
        HiSysEvent::EventType::STATISTIC, "TYPE", type, "LEVEL", level);

    std::string expectedDebugInfo;
    expectedDebugInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append(StatsHiSysEvent::AMBIENT_LIGHT)
        .append(" Ambient type = ")
        .append(ToString(type))
        .append(" Ambient brightness = ")
        .append(ToString(level));

    std::string actualDebugInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    GTEST_LOG_(INFO) << __func__ << ": expected debug info: " << expectedDebugInfo;
    GTEST_LOG_(INFO) << __func__ << ": actual debug info: " << actualDebugInfo;
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::DISPLAY, StatsHiSysEvent::AMBIENT_LIGHT, HiSysEvent::EventType::STATISTIC);
    std::string expectedMissInfo;
    expectedMissInfo.append("Additional debug info: ")
        .append("Event name = ")
        .append(StatsHiSysEvent::AMBIENT_LIGHT)
        .append(" Ambient type = ")
        .append("");
    std::string actualMissInfo = g_statsServiceProxy->ShellDump(dumpArgs, dumpArgs.size());
    auto missIndex = actualMissInfo.find(expectedMissInfo);
    EXPECT_TRUE(missIndex != string::npos);
}

/**
 * @tc.name: StatsServiceDumpTest_009
 * @tc.desc: test Dump function(args is none or help)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceDumpTest, StatsServiceDumpTest_009, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    std::vector<std::string> dumpArgsNone {};
    std::vector<std::string> dumpArgsHelp {};
    dumpArgsHelp.push_back("-h");

    std::string expectedDebugInfo;
    expectedDebugInfo.append("usage: statistics <command> [<options>]\n");

    std::string noneDebugInfo = g_statsServiceProxy->ShellDump(dumpArgsNone, dumpArgsNone.size());
    auto noneIndex = noneDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(noneIndex != string::npos);

    std::string helpDebugInfo = g_statsServiceProxy->ShellDump(dumpArgsHelp, dumpArgsHelp.size());
    auto helpIndex = helpDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(helpIndex != string::npos);
}
}