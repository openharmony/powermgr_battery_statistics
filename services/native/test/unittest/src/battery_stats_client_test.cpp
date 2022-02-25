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

#include "battery_stats_client_test.h"

#include <csignal>
#include <iostream>

#include <hisysevent.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <string_ex.h>
#include <system_ability_definition.h>

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
}

void BatteryStatsClientTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << __func__;
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
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
 * @tc.desc: Test GetTotalDataBytes function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long data = StatsUtils::INVALID_VALUE;
    data = statsClient.GetTotalDataBytes(StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    EXPECT_EQ(data, StatsUtils::DEFAULT_VALUE) << " BatteryStatsClientTest_002 fail due to reset failed";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_002: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_003
 * @tc.desc: Test GetTotalTimeSecond function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_003: test start";
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
        <<" BatteryStatsClientTest_003 fail due to time mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_003: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_004
 * @tc.desc: Test GetPartStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_004: test start";
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
        <<" BatteryStatsClientTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_004: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_005
 * @tc.desc: Test GetPartStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_005: test start";
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
        <<" BatteryStatsClientTest_005 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_005: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_006
 * @tc.desc: Test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_006: test start";
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
        <<" BatteryStatsClientTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_006: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_007
 * @tc.desc: Test GetAppStatsPercent function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_007: test start";
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
        <<" BatteryStatsClientTest_007 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_007: test end";
}

/**
 * @tc.name: BatteryStatsClientTest_008
 * @tc.desc: Test GetBatteryStats function
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsClientTest, BatteryStatsClientTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_008: test start";
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
        <<" BatteryStatsClientTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " BatteryStatsClientTest_008: test end";
}