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

#include "stats_phone_test.h"
#include "stats_log.h"

#include <call_manager_inner_type.h>
#include <hisysevent.h>

#include "battery_stats_client.h"
#include "stats_hisysevent.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS::Telephony;
using namespace std;


void StatsPhoneTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsPhoneTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsPhoneTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsPhoneTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsPhoneTest_001
 * @tc.desc: test Reset function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_001 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_001 end");
}

/**
 * @tc.name: StatsPhoneTest_002
 * @tc.desc: test GetPartStatsMah function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_002 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";

    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_002 end");
}

/**
 * @tc.name: StatsPhoneTest_003
 * @tc.desc: test GetPartStatsPercent function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_003 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_003 end");
}

/**
 * @tc.name: StatsPhoneTest_004
 * @tc.desc: test GetBatteryStats function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_004 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_PHONE) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_004 end");
}

/**
 * @tc.name: StatsPhoneTest_005
 * @tc.desc: test CALL_STATE event are sent repeatedly, phone power consumption(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_005, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_005 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_005 end");
}

/**
 * @tc.name: StatsPhoneTest_006
 * @tc.desc: test GetAppStatsMah function, Phone call on state composite test
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_006, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_006 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t stateHolding = static_cast<int32_t>(TelCallState::CALL_STATUS_HOLDING);
    int32_t stateDisconnecting = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTING);
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateHolding);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateDisconnecting);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_006 end");
}

/**
 * @tc.name: StatsPhoneTest_007
 * @tc.desc: test test GetAppStatsMah function, Phone call on abnormal state test(Phone call is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_007, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_007 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 10;
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_007 end");
}

/**
 * @tc.name: StatsPhoneTest_008
 * @tc.desc: test test GetAppStatsMah function, Phone call on abnormal state test(Phone call is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_008, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_008 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_008 end");
}

/**
 * @tc.name: StatsPhoneTest_009
 * @tc.desc: test Reset function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_009, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_009 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double powerMahBefore = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_009 end");
}

/**
 * @tc.name: StatsPhoneTest_010
 * @tc.desc: test GetPartStatsMah function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_010, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_010 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_010 end");
}

/**
 * @tc.name: StatsPhoneTest_011
 * @tc.desc: test GetPartStatsPercent function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_011, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_011 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_011 end");
}

/**
 * @tc.name: StatsPhoneTest_012
 * @tc.desc: test GetBatteryStats function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_012, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_012 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_PHONE) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_012 end");
}

/**
 * @tc.name: StatsPhoneTest_013
 * @tc.desc: test CALL_STATE event are sent repeatedly, phone power consumption(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_013, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_013 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_013 end");
}

/**
 * @tc.name: StatsPhoneTest_014
 * @tc.desc: test test GetAppStatsMah function, Phone data on abnormal state test(Phone data is off)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_014, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_014 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 5;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_014 end");
}

/**
 * @tc.name: StatsPhoneTest_015
 * @tc.desc: test test GetAppStatsMah function, Phone data on abnormal state test(Phone data is on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_015, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_015 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateInvaildOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateInvaildOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_015 end");
}

/**
 * @tc.name: StatsPhoneTest_016
 * @tc.desc: test CALL_STATE and TELEPHONY event are sent repeatedly, power consumption(Func:GetPartStatsMah)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_016, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_016 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOff);

    double phoneOnPower = 3 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double phoneDataPower = 4 * POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double expectedPower = phoneOnPower + phoneDataPower;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_016 end");
}

/**
 * @tc.name: StatsPhoneTest_017
 * @tc.desc: test CALL_STATE and TELEPHONY event are sent repeatedly, power consumption(Func:GetBatteryStats)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_017, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_017 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOff);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOff);

    double phoneOnPower = 3 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double phoneDataPower = 4 * POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double expectedPower = phoneOnPower + phoneDataPower;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = statsClient.GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_PHONE) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_017 end");
}

/**
 * @tc.name: StatsPhoneTest_018
 * @tc.desc: test SetOnBattery function(Phone Call & Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_018, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_018 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();
    statsClient.SetOnBattery(false);

    int32_t callStateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t callStateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int32_t dataStateOn = 1;
    int32_t dataStateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", callStateOff);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", dataStateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    statsClient.SetOnBattery(true);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_018 end");
}

/**
 * @tc.name: StatsPhoneTest_019
 * @tc.desc: test GetPartStatsMah function with battery changed(Phone call on)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_019, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_019 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
    int16_t level = 0;
    double phoneOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(false);
    usleep(POWER_CONSUMPTION_DURATION_US);
    statsClient.SetOnBattery(true);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * phoneOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_019 end");
}

/**
 * @tc.name: StatsPhoneTest_020
 * @tc.desc: test GetPartStatsMah(Phone Data) and GetAppStatsMah(Audio) function
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_020, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_020 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int16_t level = 0;
    double phoneDataAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_DATA, level);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * phoneDataAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;

    HiSysEventWrite(HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE,
        HiSysEvent::EventType::BEHAVIOR, "PID", pid, "UID", uid, "STATE", stateRunning);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE,
        HiSysEvent::EventType::BEHAVIOR, "PID", pid, "UID", uid, "STATE", stateStopped);

    expectedPower = POWER_CONSUMPTION_DURATION_US * audioOnAverageMa / US_PER_HOUR;
    actualPower = statsClient.GetAppStatsMah(uid);
    devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_020 end");
}

/**
 * @tc.name: StatsPhoneTest_021
 * @tc.desc: test GetTotalTimeSecond function(Phone Call)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_021, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_021 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
    int32_t stateOff = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::CALL_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_021 end");
}

/**
 * @tc.name: StatsPhoneTest_022
 * @tc.desc: test GetTotalTimeSecond function(Phone Data)
 * @tc.type: FUNC
 * @tc.require: issueI5HWJ3
 */
HWTEST_F (StatsPhoneTest, StatsPhoneTest_022, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_022 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::TELEPHONY, StatsHiSysEvent::DATA_CONNECTION_STATE,
        HiSysEvent::EventType::BEHAVIOR, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_PHONE_DATA);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
    STATS_HILOGI(LABEL_TEST, "StatsPhoneTest_022 end");
}
}