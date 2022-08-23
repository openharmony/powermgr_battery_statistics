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

#include <call_manager_inner_type.h>
#include <hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_parser.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS::Telephony;
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
 * @tc.name: StatsPhoneTest_001
 * @tc.desc: test Reset function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsPhoneTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPhoneTest_001: test end";
}

/**
 * @tc.name: StatsPhoneTest_002
 * @tc.desc: test GetPartStatsMah function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_002: test end";
}

/**
 * @tc.name: StatsPhoneTest_003
 * @tc.desc: test GetPartStatsPercent function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);
    
    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPhoneTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_003: test end";
}

/**
 * @tc.name: StatsPhoneTest_004
 * @tc.desc: test GetBatteryStats function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
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
        <<" StatsPhoneTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_004: test end";
}

/**
 * @tc.name: StatsPhoneTest_005
 * @tc.desc: test CALL_STATE event are sent repeatedly, phone power consumption(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_005: test end";
}

/**
 * @tc.name: StatsPhoneTest_006
 * @tc.desc: test GetAppStatsMah function, Phone call on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t stateHolding = static_cast<int32_t>(TelCallState::CALL_STATUS_HOLDING);
    int32_t stateDisconnecting = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTING);
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateHolding);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateDisconnecting);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsPhoneTest_006 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_006: test end";
}

/**
 * @tc.name: StatsPhoneTest_007
 * @tc.desc: test test GetAppStatsMah function, Phone call on abnormal state test(Phone call is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) << " StatsPhoneTest_007 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_007: test end";
}

/**
 * @tc.name: StatsPhoneTest_008
 * @tc.desc: test test GetAppStatsMah function, Phone call on abnormal state test(Phone call is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsPhoneTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_008: test end";
}

/**
 * @tc.name: StatsPhoneTest_009
 * @tc.desc: test Reset function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsPhoneTest_009 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsPhoneTest_009: test end";
}

/**
 * @tc.name: StatsPhoneTest_010
 * @tc.desc: test GetPartStatsMah function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_010: test end";
}

/**
 * @tc.name: StatsPhoneTest_011
 * @tc.desc: test GetPartStatsPercent function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);
    
    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsPhoneTest_011 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_011: test end";
}

/**
 * @tc.name: StatsPhoneTest_012
 * @tc.desc: test GetBatteryStats function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
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
        <<" StatsPhoneTest_012 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_012: test end";
}

/**
 * @tc.name: StatsPhoneTest_013
 * @tc.desc: test CALL_STATE event are sent repeatedly, phone power consumption(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_013: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_013 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_013: test end";
}

/**
 * @tc.name: StatsPhoneTest_014
 * @tc.desc: test test GetAppStatsMah function, Phone data on abnormal state test(Phone data is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_014: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 5;
    int32_t stateOff = 0;

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) << " StatsPhoneTest_014 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_014: test end";
}

/**
 * @tc.name: StatsPhoneTest_015
 * @tc.desc: test test GetAppStatsMah function, Phone data on abnormal state test(Phone data is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", stateInvaildOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", stateInvaildOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsPhoneTest_015 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_015: test end";
}

/**
 * @tc.name: StatsPhoneTest_016
 * @tc.desc: test CALL_STATE and CELLULAR_DATA event are sent repeatedly, power consumption(Func:GetPartStatsMah)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_016: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOff);
    sleep(testWaitTimeSec);

    double phoneOnPower = 3 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double phoneDataPower = 4 * testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double expectedPower = phoneOnPower + phoneDataPower;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_016 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_016: test end";
}

/**
 * @tc.name: StatsPhoneTest_017
 * @tc.desc: test CALL_STATE and CELLULAR_DATA event are sent repeatedly, power consumption(Func:GetBatteryStats)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOff);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOff);
    sleep(testWaitTimeSec);

    double phoneOnPower = 3 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double phoneDataPower = 4 * testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double expectedPower = phoneOnPower + phoneDataPower;
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
        <<" StatsPhoneTest_017 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_017: test end";
}

/**
 * @tc.name: StatsPhoneTest_018
 * @tc.desc: test SetOnBattery function(Phone Call & Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", callStateOff);
    sleep(testWaitTimeSec);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR,
        "STATE", dataStateOff);
    sleep(testWaitTimeSec);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_EQ(expectedPower, actualPower) <<" StatsPhoneTest_018 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_018: test end";
    statsClient.SetOnBattery(true);
}

/**
 * @tc.name: StatsPhoneTest_019
 * @tc.desc: test GetPartStatsMah function with battery changed(Phone call on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_019, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_019: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double deviation = 0.01;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * phoneOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation) << " StatsPhoneTest_019 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_019: test end";
}

/**
 * @tc.name: StatsPhoneTest_020
 * @tc.desc: test GetPartStatsMah(Phone Data) and GetAppStatsMah(Audio) function
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_020, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_020: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * phoneDataAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_020 fail due to power mismatch";

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsPhoneTest_020 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_020: test end";
}

/**
 * @tc.name: StatsPhoneTest_021
 * @tc.desc: test GetTotalTimeSecond function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_021, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_021: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double deviation = 0.01;

    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CALL_MANAGER", "CALL_STATE", HiSysEvent::EventType::STATISTIC, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsPhoneTest_021 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_021: test end";
}

/**
 * @tc.name: StatsPhoneTest_022
 * @tc.desc: test GetTotalTimeSecond function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsClientTest, StatsPhoneTest_022, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsPhoneTest_022: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CELLULAR_DATA", "DATA_CONNECTION_STATE", HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_DATA);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation) << " StatsPhoneTest_022 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsPhoneTest_022: test end";
}
}