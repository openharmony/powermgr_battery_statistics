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

static void WriteBluetoothEvent(int32_t pid, int32_t uid, long time)
{
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateScanOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateScanOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(time);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(time);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(time);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateScanOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(time);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateScanOff);
    sleep(testWaitTimeSec);
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
 * @tc.name: StatsBluetoothTest_001
 * @tc.desc: test Reset function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsBluetoothTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_001: test end";
}

/**
 * @tc.name: StatsBluetoothTest_002
 * @tc.desc: test GetPartStatsMah function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    double deviation = 0.01;
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_002: test end";
}

/**
 * @tc.name: StatsBluetoothTest_003
 * @tc.desc: test GetPartStatsPercent function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsBluetoothTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_003: test end";
}

/**
 * @tc.name: StatsBluetoothTest_004
 * @tc.desc: test GetBatteryStats function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_004: test end";
}

/**
 * @tc.name: StatsBluetoothTest_005
 * @tc.desc: test BLUETOOTH_BR_SWITCH_STATE event are sent repeatedly, Bluetooth BR power consumption(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_005: test end";
}

/**
 * @tc.name: StatsBluetoothTest_006
 * @tc.desc: test GetAppStatsMah function, Bluetooth BR on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateTurningOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURNING_ON);
    int32_t stateTurningOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURNING_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateTurningOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateTurningOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_006: test end";
}

/**
 * @tc.name: StatsBluetoothTest_007
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR on abnormal state test(Bluetooth BR is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) << " StatsBluetoothTest_007 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_007: test end";
}

/**
 * @tc.name: StatsBluetoothTest_008
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR on abnormal state test(Bluetooth BR is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_008: test end";
}

/**
 * @tc.name: StatsBluetoothTest_009
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsBluetoothTest_009 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_009: test end";
}

/**
 * @tc.name: StatsBluetoothTest_010
 * @tc.desc: test Reset function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsBluetoothTest_010 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_010: test end";
}

/**
 * @tc.name: StatsBluetoothTest_011
 * @tc.desc: test GetPartStatsMah function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    double deviation = 0.01;
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_011 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_011: test end";
}

/**
 * @tc.name: StatsBluetoothTest_012
 * @tc.desc: test GetPartStatsPercent function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsBluetoothTest_012 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_012: test end";
}

/**
 * @tc.name: StatsBluetoothTest_013
 * @tc.desc: test GetBatteryStats function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_013: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH) {
            actualPower = (*it).GetPower();
        }
    }
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_013 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_013: test end";
}

/**
 * @tc.name: StatsBluetoothTest_014
 * @tc.desc: test BLUETOOTH_BLE_STATE event are sent repeatedly, Bluetooth BLE power consumption(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_014: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_014 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_014: test end";
}

/**
 * @tc.name: StatsBluetoothTest_015
 * @tc.desc: test GetAppStatsMah function, Bluetooth BLE on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateTurningOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURNING_ON);
    int32_t stateTurningOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURNING_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateTurningOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateTurningOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_015 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_015: test end";
}

/**
 * @tc.name: StatsBluetoothTest_016
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BLE on abnormal state test(Bluetooth BLE is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_016: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) << " StatsBluetoothTest_016 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_016: test end";
}

/**
 * @tc.name: StatsBluetoothTest_017
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BLE on abnormal state test(Bluetooth BLE is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_017 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_017: test end";
}

/**
 * @tc.name: StatsBluetoothTest_018
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsBluetoothTest_018 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_018: test end";
}

/**
 * @tc.name: StatsBluetoothTest_019
 * @tc.desc: test Reset function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_019, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_019: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsBluetoothTest_019 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_019: test end";
}

/**
 * @tc.name: StatsBluetoothTest_020
 * @tc.desc: test GetAppStatsMah function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_020, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_020: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBrScanAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsBluetoothTest_020 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_020: test end";
}

/**
 * @tc.name: StatsBluetoothTest_021
 * @tc.desc: test GetAppStatsPercent function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_021, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_021: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsBluetoothTest_021 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_021: test end";
}

/**
 * @tc.name: StatsBluetoothTest_022
 * @tc.desc: test BLUETOOTH_DISCOVERY_STATE event are sent repeatedly, BR scan power consumption(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_022, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_022: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBrScanAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_022 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_022: test end";
}

/**
 * @tc.name: StatsBluetoothTest_023
 * @tc.desc: test GetAppStatsMah function, Bluetooth BR scan state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_023, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_023: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    int32_t stateScan = static_cast<int32_t>(bluetooth::DISCOVERYING);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateScan);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateScan);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBrScanAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_023 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_023: test end";
}

/**
 * @tc.name: StatsBluetoothTest_024
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR scan abnormal state test(Bluetooth BR scan is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_024, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_024: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) << " StatsBluetoothTest_024 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_024: test end";
}

/**
 * @tc.name: StatsBluetoothTest_025
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR scan abnormal state test(Bluetooth BR scan is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_025, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_025: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * bluetoothBleOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_025 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_025: test end";
}

/**
 * @tc.name: StatsBluetoothTest_026
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_026, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_026: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsBluetoothTest_026 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_026: test end";
}

/**
 * @tc.name: StatsBluetoothTest_027
 * @tc.desc: test GetAppStatsMah function with different UID(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_027, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_027: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;
    int32_t stateOn = static_cast<int32_t>(bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(bluetooth::DISCOVERY_STOPED);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pidOne, "UID", uidOne, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pidTwo, "UID", uidTwo, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pidTwo, "UID", uidTwo, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_DISCOVERY_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pidOne, "UID", uidOne, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPowerOne = 3 * testTimeSec * bluetoothBrScanAverageMa / SECOND_PER_HOUR;
    double actualPowerOne = statsClient.GetAppStatsMah(uidOne);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption One = " << expectedPowerOne << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption One = " << actualPowerOne << " mAh";
    EXPECT_LE(abs(expectedPowerOne - actualPowerOne), deviation)
        <<" StatsBluetoothTest_027 fail due to power mismatch";

    double expectedPowerTwo = testTimeSec * bluetoothBrScanAverageMa / SECOND_PER_HOUR;
    double actualPowerTwo = statsClient.GetAppStatsMah(uidTwo);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption Two = " << expectedPowerTwo << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption Two = " << actualPowerTwo << " mAh";
    EXPECT_LE(abs(expectedPowerTwo - actualPowerTwo), deviation)
        <<" StatsBluetoothTest_027 fail due to power mismatch";

    GTEST_LOG_(INFO) << " StatsBluetoothTest_027: test end";
}

/**
 * @tc.name: StatsBluetoothTest_028
 * @tc.desc: test Reset function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_028, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_028: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsBluetoothTest_028 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_028: test end";
}

/**
 * @tc.name: StatsBluetoothTest_029
 * @tc.desc: test GetAppStatsMah function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_029, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_029: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * bluetoothBleScanAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsBluetoothTest_029 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_029: test end";
}

/**
 * @tc.name: StatsBluetoothTest_030
 * @tc.desc: test GetAppStatsPercent function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_030, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_030: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsBluetoothTest_030 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_030: test end";
}

/**
 * @tc.name: StatsBluetoothTest_031
 * @tc.desc: test Bluetooth BLE scan event are sent repeatedly, Bluetooth BLE power consumption(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_031, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_031: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBleScanAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_031 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_031: test end";
}

/**
 * @tc.name: StatsBluetoothTest_032
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_032, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_032: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BLE_SCAN, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsBluetoothTest_032 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_032: test end";
}

/**
 * @tc.name: StatsBluetoothTest_033
 * @tc.desc: test GetAppStatsMah function with different UID(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_033, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_033: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pidOne, "UID", uidOne);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_START", HiSysEvent::EventType::STATISTIC,
        "PID", pidTwo, "UID", uidTwo);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pidTwo, "UID", uidTwo);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BLE_SCAN_STOP", HiSysEvent::EventType::STATISTIC,
        "PID", pidOne, "UID", uidOne);
    sleep(testWaitTimeSec);

    double expectedPowerOne = 3 * testTimeSec * bluetoothBleScanAverageMa / SECOND_PER_HOUR;
    double actualPowerOne = statsClient.GetAppStatsMah(uidOne);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption One = " << expectedPowerOne << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption One = " << actualPowerOne << " mAh";
    EXPECT_LE(abs(expectedPowerOne - actualPowerOne), deviation)
        <<" StatsBluetoothTest_033 fail due to power mismatch";

    double expectedPowerTwo = testTimeSec * bluetoothBleScanAverageMa / SECOND_PER_HOUR;
    double actualPowerTwo = statsClient.GetAppStatsMah(uidTwo);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption Two = " << expectedPowerTwo << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption Two = " << actualPowerTwo << " mAh";
    EXPECT_LE(abs(expectedPowerTwo - actualPowerTwo), deviation)
        <<" StatsBluetoothTest_033 fail due to power mismatch";

    GTEST_LOG_(INFO) << " StatsBluetoothTest_033: test end";
}

/**
 * @tc.name: StatsBluetoothTest_034
 * @tc.desc: test GetPartStatsMah function(Bluetooth BR on & BLE on & BR scan & BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_034, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_034: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    long testTimeSec = 2;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    WriteBluetoothEvent(pid, uid, testTimeSec);

    double expectedPartPower = testTimeSec * (bluetoothBrOnAverageMa + bluetoothBleOnAverageMa) / SECOND_PER_HOUR;
    double actualPartPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected part consumption = " << expectedPartPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual part consumption = " << actualPartPower << " mAh";

    EXPECT_LE(abs(expectedPartPower - actualPartPower), deviation)
        << " StatsBluetoothTest_034 fail due to power mismatch";

    double expectedSoftPower = testTimeSec * (bluetoothBrScanAverageMa + bluetoothBleScanAverageMa) / SECOND_PER_HOUR;
    double actualSoftPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected soft consumption = " << expectedSoftPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual soft consumption = " << actualSoftPower << " mAh";

    EXPECT_LE(abs(expectedSoftPower - actualSoftPower), deviation)
        << " StatsBluetoothTest_034 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_034: test end";
}

/**
 * @tc.name: StatsBluetoothTest_035
 * @tc.desc: test SetOnBattery function(Bluetooth BR on & BLE on & BR scan & BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_035, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_035: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    WriteBluetoothEvent(pid, uid, testTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPartPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected part consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual part consumption = " << actualPartPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPartPower), deviation)
        << " StatsBluetoothTest_035 fail due to power mismatch";

    double actualSoftPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected soft consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual soft consumption = " << actualSoftPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualSoftPower), deviation)
        << " StatsBluetoothTest_035 fail due to power mismatch";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsBluetoothTest_036
 * @tc.desc: test GetPartStatsMah function with battery changed(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsClientTest, StatsBluetoothTest_036, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsBluetoothTest_036: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(bluetooth::BTStateID::STATE_TURN_OFF);
    double deviation = 0.01;

    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("BLUETOOTH", "BLUETOOTH_BR_SWITCH_STATE", HiSysEvent::EventType::STATISTIC,
        "PID", pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * bluetoothBrOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsBluetoothTest_036 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsBluetoothTest_036: test end";
}
}