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

#include "stats_bluetooth_test.h"
#include "stats_log.h"

#include <bluetooth_def.h>
#include <hisysevent.h>

#include "battery_stats_client.h"
#include "stats_hisysevent.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

static void WriteBluetoothEvent(int32_t pid, int32_t uid, long time)
{
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateScanOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateScanOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(time * StatsTest::US_PER_MS);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(time * StatsTest::US_PER_MS);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(time * StatsTest::US_PER_MS);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateScanOn);
    usleep(time * StatsTest::US_PER_MS);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateScanOff);
}

void StatsBluetoothTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsBluetoothTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsBluetoothTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsBluetoothTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsBluetoothTest_001
 * @tc.desc: test Reset function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_001 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_001 end");
}

/**
 * @tc.name: StatsBluetoothTest_002
 * @tc.desc: test GetPartStatsMah function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_002 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_002 end");
}

/**
 * @tc.name: StatsBluetoothTest_003
 * @tc.desc: test GetPartStatsPercent function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_003 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_003 end");
}

/**
 * @tc.name: StatsBluetoothTest_004
 * @tc.desc: test GetBatteryStats function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_004 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_004 end");
}

/**
 * @tc.name: StatsBluetoothTest_005
 * @tc.desc: test BR_SWITCH_STATE event are sent repeatedly, Bluetooth BR power consumption(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_005, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_005 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_005 end");
}

/**
 * @tc.name: StatsBluetoothTest_006
 * @tc.desc: test GetAppStatsMah function, Bluetooth BR on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_006, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_006 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateTurningOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURNING_ON);
    int32_t stateTurningOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURNING_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateTurningOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateTurningOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_006 end");
}

/**
 * @tc.name: StatsBluetoothTest_007
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR on abnormal state test(Bluetooth BR is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_007, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_007 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_007 end");
}

/**
 * @tc.name: StatsBluetoothTest_008
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR on abnormal state test(Bluetooth BR is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_008, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_008 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_008 end");
}

/**
 * @tc.name: StatsBluetoothTest_009
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_009, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_009 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_009 end");
}

/**
 * @tc.name: StatsBluetoothTest_010
 * @tc.desc: test Reset function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_010, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_010 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_010 end");
}

/**
 * @tc.name: StatsBluetoothTest_011
 * @tc.desc: test GetPartStatsMah function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_011, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_011 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_011 end");
}

/**
 * @tc.name: StatsBluetoothTest_012
 * @tc.desc: test GetPartStatsPercent function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_012, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_012 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_012 end");
}

/**
 * @tc.name: StatsBluetoothTest_013
 * @tc.desc: test GetBatteryStats function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_013, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_013 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_013 end");
}

/**
 * @tc.name: StatsBluetoothTest_014
 * @tc.desc: test BLE_SWITCH_STATE event are sent repeatedly, Bluetooth BLE power consumption(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_014, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_014 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_014 end");
}

/**
 * @tc.name: StatsBluetoothTest_015
 * @tc.desc: test GetAppStatsMah function, Bluetooth BLE on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_015, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_015 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateTurningOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURNING_ON);
    int32_t stateTurningOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURNING_OFF);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateTurningOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateTurningOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_015 end");
}

/**
 * @tc.name: StatsBluetoothTest_016
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BLE on abnormal state test(Bluetooth BLE is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_016, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_016 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_016 end");
}

/**
 * @tc.name: StatsBluetoothTest_017
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BLE on abnormal state test(Bluetooth BLE is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_017, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_017 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_017 end");
}

/**
 * @tc.name: StatsBluetoothTest_018
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BLE on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_018, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_018 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_018 end");
}

/**
 * @tc.name: StatsBluetoothTest_019
 * @tc.desc: test Reset function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_019, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_019 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_019 end");
}

/**
 * @tc.name: StatsBluetoothTest_020
 * @tc.desc: test GetAppStatsMah function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_020, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_020 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBrScanAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_020 end");
}

/**
 * @tc.name: StatsBluetoothTest_021
 * @tc.desc: test GetAppStatsPercent function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_021, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_021 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_021 end");
}

/**
 * @tc.name: StatsBluetoothTest_022
 * @tc.desc: test DISCOVERY_STATE event are sent repeatedly, BR scan power consumption(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_022, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_022 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBrScanAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_022 end");
}

/**
 * @tc.name: StatsBluetoothTest_023
 * @tc.desc: test GetAppStatsMah function, Bluetooth BR scan state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_023, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_023 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);
    int32_t stateScan = static_cast<int32_t>(Bluetooth::DISCOVERYING);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateScan);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateScan);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBrScanAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_023 end");
}

/**
 * @tc.name: StatsBluetoothTest_024
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR scan abnormal state test(Bluetooth BR scan is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_024, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_024 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_024 end");
}

/**
 * @tc.name: StatsBluetoothTest_025
 * @tc.desc: test test GetAppStatsMah function, Bluetooth BR scan abnormal state test(Bluetooth BR scan is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_025, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_025 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * bluetoothBleOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_025 end");
}

/**
 * @tc.name: StatsBluetoothTest_026
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_026, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_026 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_026 end");
}

/**
 * @tc.name: StatsBluetoothTest_027
 * @tc.desc: test GetAppStatsMah function with different UID(Bluetooth BR scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_027, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_027 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::DISCOVERY_STARTED);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::DISCOVERY_STOPED);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::DISCOVERY_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne, "STATE", stateOff);

    double expectedPowerOne = 3 * POWER_CONSUMPTION_DURATION_US * bluetoothBrScanAverageMa / US_PER_HOUR;
    double actualPowerOne = statsClient.GetAppStatsMah(uidOne);
    double devPrecentOne = abs(expectedPowerOne - actualPowerOne) / expectedPowerOne;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption One = " << expectedPowerOne << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption One = " << actualPowerOne << " mAh";
    EXPECT_LE(devPrecentOne, DEVIATION_PERCENT_THRESHOLD);

    double expectedPowerTwo = POWER_CONSUMPTION_DURATION_US * bluetoothBrScanAverageMa / US_PER_HOUR;
    double actualPowerTwo = statsClient.GetAppStatsMah(uidTwo);
    double devPrecentTwo = abs(expectedPowerTwo - actualPowerTwo) / expectedPowerTwo;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption Two = " << expectedPowerTwo << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption Two = " << actualPowerTwo << " mAh";
    EXPECT_LE(devPrecentTwo, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_027 end");
}

/**
 * @tc.name: StatsBluetoothTest_028
 * @tc.desc: test Reset function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_028, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_028 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_028 end");
}

/**
 * @tc.name: StatsBluetoothTest_029
 * @tc.desc: test GetAppStatsMah function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_029, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_029 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * bluetoothBleScanAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_029 end");
}

/**
 * @tc.name: StatsBluetoothTest_030
 * @tc.desc: test GetAppStatsPercent function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_030, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_030 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_030 end");
}

/**
 * @tc.name: StatsBluetoothTest_031
 * @tc.desc: test Bluetooth BLE scan event are sent repeatedly, Bluetooth BLE power consumption(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_031, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_031 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBleScanAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_031 end");
}

/**
 * @tc.name: StatsBluetoothTest_032
 * @tc.desc: test GetTotalTimeSecond function(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_032, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_032 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_BLUETOOTH_BLE_SCAN, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_032 end");
}

/**
 * @tc.name: StatsBluetoothTest_033
 * @tc.desc: test GetAppStatsMah function with different UID(Bluetooth BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_033, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_033 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_START,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BLE_SCAN_STOP,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne);

    double expectedPowerOne = 3 * POWER_CONSUMPTION_DURATION_US * bluetoothBleScanAverageMa / US_PER_HOUR;
    double actualPowerOne = statsClient.GetAppStatsMah(uidOne);
    double devPrecentOne = abs(expectedPowerOne - actualPowerOne) / expectedPowerOne;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption One = " << expectedPowerOne << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption One = " << actualPowerOne << " mAh";
    EXPECT_LE(devPrecentOne, DEVIATION_PERCENT_THRESHOLD);

    double expectedPowerTwo = POWER_CONSUMPTION_DURATION_US * bluetoothBleScanAverageMa / US_PER_HOUR;
    double actualPowerTwo = statsClient.GetAppStatsMah(uidTwo);
    double devPrecentTwo = abs(expectedPowerTwo - actualPowerTwo) / expectedPowerTwo;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption Two = " << expectedPowerTwo << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption Two = " << actualPowerTwo << " mAh";
    EXPECT_LE(devPrecentTwo, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_033 end");
}

/**
 * @tc.name: StatsBluetoothTest_034
 * @tc.desc: test GetPartStatsMah function(Bluetooth BR on & BLE on & BR scan & BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_034, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_034 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    double bluetoothBleOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_ON);
    double bluetoothBrScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_SCAN);
    double bluetoothBleScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BLE_SCAN);
    long testTimeMs = 200;
    int32_t uid = 10003;
    int32_t pid = 3458;

    WriteBluetoothEvent(pid, uid, testTimeMs);

    double expectedPartPower = testTimeMs * (bluetoothBrOnAverageMa + bluetoothBleOnAverageMa) / MS_PER_HOUR;
    double actualPartPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecentPart = abs(expectedPartPower - actualPartPower) / expectedPartPower;
    GTEST_LOG_(INFO) << __func__ << ": expected part consumption = " << expectedPartPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual part consumption = " << actualPartPower << " mAh";
    EXPECT_LE(devPrecentPart, DEVIATION_PERCENT_THRESHOLD);

    double expectedSoftPower = testTimeMs * (bluetoothBrScanAverageMa + bluetoothBleScanAverageMa) / MS_PER_HOUR;
    double actualSoftPower = statsClient.GetAppStatsMah(uid);
    double devPrecentSoft = abs(expectedSoftPower - actualSoftPower) / expectedSoftPower;
    GTEST_LOG_(INFO) << __func__ << ": expected soft consumption = " << expectedSoftPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual soft consumption = " << actualSoftPower << " mAh";
    EXPECT_LE(devPrecentSoft, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_034 end");
}

/**
 * @tc.name: StatsBluetoothTest_035
 * @tc.desc: test SetOnBattery function(Bluetooth BR on & BLE on & BR scan & BLE scan)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_035, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_035 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeMs = 200;
    int32_t uid = 10003;
    int32_t pid = 3458;

    WriteBluetoothEvent(pid, uid, testTimeMs);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPartPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    GTEST_LOG_(INFO) << __func__ << ": expected part consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual part consumption = " << actualPartPower << " mAh";
    EXPECT_EQ(expectedPower, actualPartPower);

    double actualSoftPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected soft consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual soft consumption = " << actualSoftPower << " mAh";
    EXPECT_EQ(expectedPower, actualSoftPower);
    statsClient.SetOnBattery(true);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_035 end");
}

/**
 * @tc.name: StatsBluetoothTest_036
 * @tc.desc: test GetPartStatsMah function with battery changed(Bluetooth BR on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJK
 */
HWTEST_F (StatsBluetoothTest, StatsBluetoothTest_036, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_036 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double bluetoothBrOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_BR_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_ON);
    int32_t stateOff = static_cast<int32_t>(Bluetooth::BTStateID::STATE_TURN_OFF);

    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(false);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(true);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::BT_SERVICE, StatsHiSysEvent::BR_SWITCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * bluetoothBrOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsBluetoothTest_036 end");
}
}