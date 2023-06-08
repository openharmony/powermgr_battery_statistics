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

#include "stats_powermgr_test.h"

#include <bt_def.h>
#include <call_manager_inner_type.h>
#include <hisysevent.h>
#include <wifi_hisysevent.h>

#include "battery_stats_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS::Telephony;
using namespace OHOS;
using namespace std;


void StatsPowerMgrTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsPowerMgrTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsPowerMgrTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsPowerMgrTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsPowerMgrTest_009
 * @tc.desc: test GetTotalTimeSecond function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_009, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
}

/**
 * @tc.name: StatsPowerMgrTest_010
 * @tc.desc: test GetAppStatsMah function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_010, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorGravityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsPowerMgrTest_011
 * @tc.desc: test GetAppStatsPercent function(Sensor Gravity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_011, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_012
 * @tc.desc: test GetAppStatsMah(Sensor Gravity) and GetAppStatsPercent(Sensor Proximity) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_012, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorGravityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorGravityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_013
 * @tc.desc: test GetTotalTimeSecond function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_013, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t stateOn = 1;
    int32_t stateOff = 0;
    int32_t uid = 10003;
    int32_t pid = 3458;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsPowerMgrTest_014
 * @tc.desc: test GetAppStatsMah function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_014, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorProximityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsPowerMgrTest_015
 * @tc.desc: test GetAppStatsPercent function(Sensor Proximity)
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_015, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsPowerMgrTest_016
 * @tc.desc: test GetAppStatsMah(Sensor Proximity) and GetAppStatsPercent(Torch) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsPowerMgrTest, StatsPowerMgrTest_016, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double sensorProximityOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_PROXIMITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * sensorProximityOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    stateOn = 1;
    stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: BatteryStatsInfo_001
 * @tc.desc: test class BatteryStatsInfo function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsPowerMgrTest, BatteryStatsInfo_001, TestSize.Level0)
{
    std::shared_ptr<BatteryStatsInfo> sptrStatsInfo = std::make_shared<BatteryStatsInfo>();
    EXPECT_NE(sptrStatsInfo, nullptr);
    int32_t uid = 1004;
    int32_t userId = 10005;
    BatteryStatsInfo::ConsumptionType consumptionType = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
    double totalPowerMah = 1000.0;
    double partPowerMah = 200.0;

    EXPECT_EQ(StatsUtils::INVALID_VALUE, sptrStatsInfo->GetUid());
    sptrStatsInfo->SetPower(partPowerMah);
    EXPECT_EQ(partPowerMah, sptrStatsInfo->GetPower());

    sptrStatsInfo->SetUid(uid);
    sptrStatsInfo->SetUserId(userId);
    sptrStatsInfo->SetConsumptioType(consumptionType);
    sptrStatsInfo->SetPower(totalPowerMah);
    EXPECT_EQ(uid, sptrStatsInfo->GetUid());
    EXPECT_EQ(userId, sptrStatsInfo->GetUserId());
    EXPECT_EQ(consumptionType, sptrStatsInfo->GetConsumptionType());
    EXPECT_EQ(totalPowerMah, sptrStatsInfo->GetPower());

    sptrStatsInfo->SetUid(StatsUtils::INVALID_VALUE);
    EXPECT_EQ(uid, sptrStatsInfo->GetUid());
    sptrStatsInfo->SetUserId(StatsUtils::INVALID_VALUE);
    EXPECT_EQ(userId, sptrStatsInfo->GetUserId());
    
    Parcel infoParcel = {};
    EXPECT_EQ(nullptr, sptrStatsInfo->Unmarshalling(infoParcel));

    sptrStatsInfo->Marshalling(infoParcel);
    auto unmarshalInfo = sptrStatsInfo->Unmarshalling(infoParcel);
    if (unmarshalInfo != nullptr) {
        EXPECT_EQ(uid, unmarshalInfo->GetUid());
        EXPECT_EQ(consumptionType, unmarshalInfo->GetConsumptionType());
        EXPECT_EQ(totalPowerMah, unmarshalInfo->GetPower());
    }
}

/**
 * @tc.name: BatteryStatsInfo_002
 * @tc.desc: test class ConvertConsumptionType function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsPowerMgrTest, BatteryStatsInfo_002, TestSize.Level0)
{
    EXPECT_EQ("", BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_INVALID));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_APP),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_APP));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_BLUETOOTH),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_IDLE),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_IDLE));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_PHONE),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_SCREEN),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_USER),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_USER));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_WIFI),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_CAMERA),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_FLASHLIGHT),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_AUDIO),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_SENSOR),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_GNSS),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_GNSS));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_CPU),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_CPU));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_WAKELOCK),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK));
    EXPECT_EQ(GET_VARIABLE_NAME(CONSUMPTION_TYPE_ALARM),
        BatteryStatsInfo::ConvertConsumptionType(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM));
}

/**
 * @tc.name: BatteryStatsParser_001
 * @tc.desc: test class BatteryStatsParser function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsPowerMgrTest, BatteryStatsParser_001, TestSize.Level0)
{
    uint16_t normalLevel = 0;
    uint16_t errorRadioOnLevel = 10;
    EXPECT_EQ(0.0, g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_INVALID));
    EXPECT_EQ(0.0, g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_INVALID, normalLevel));
    EXPECT_EQ(0.0, g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, errorRadioOnLevel));

    uint16_t expectClusterNum = 3;
    EXPECT_EQ(expectClusterNum, g_statsParser->GetClusterNum());

    uint16_t speedClusterZero = 0;
    uint16_t expectSpeedCluster0Num = 14;
    uint16_t errorSpeedCluster = 3;
    EXPECT_EQ(expectSpeedCluster0Num, g_statsParser->GetSpeedNum(speedClusterZero));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, g_statsParser->GetSpeedNum(errorSpeedCluster));
}

/**
 * @tc.name: BatteryStatsRadio_001
 * @tc.desc: test class BatteryStatsClient function with radio type
 * @tc.type: FUNC
 * @tc.require: issueI6ZT17
 */
HWTEST_F (StatsPowerMgrTest, BatteryStatsRadio_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double actualPower = statsClient.GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    double actualPercent = statsClient.GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    EXPECT_TRUE(actualPower == StatsUtils::DEFAULT_VALUE && actualPercent == StatsUtils::DEFAULT_VALUE);
}
}