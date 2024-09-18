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

#include "stats_camera_test.h"
#include "stats_log.h"

#include <hisysevent.h>

#include "battery_stats_client.h"
#include "stats_hisysevent.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;


void StatsCameraTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    system("hidumper -s 3302 -a -u");
}

void StatsCameraTest::TearDownTestCase()
{
    system("hidumper -s 3302 -a -r");
}

void StatsCameraTest::SetUp()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
}

void StatsCameraTest::TearDown()
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsCameraTest_001
 * @tc.desc: test Reset function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_001, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_001 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT,
        HiSysEvent::EventType::STATISTIC, "ID", cameraId);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_001 end");
}

/**
 * @tc.name: StatsCameraTest_002
 * @tc.desc: test GetAppStatsMah function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_002, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_002 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_002 end");
}

/**
 * @tc.name: StatsCameraTest_003
 * @tc.desc: test GetAppStatsPercent function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_003, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_003 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", deviceId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_003 end");
}

/**
 * @tc.name: StatsCameraTest_004
 * @tc.desc: test Reset function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_004, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_004 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_004 end");
}

/**
 * @tc.name: StatsCameraTest_005
 * @tc.desc: test GetAppStatsMah function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_005, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_005 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = (3 * POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR) +
        (POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR);
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_005 end");
}

/**
 * @tc.name: StatsCameraTest_006
 * @tc.desc: test GetAppStatsPercent function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_006, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_006 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_006 end");
}

/**
 * @tc.name: StatsCameraTest_007
 * @tc.desc: test Reset function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_007, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_007 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId0);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_007 end");
}

/**
 * @tc.name: StatsCameraTest_008
 * @tc.desc: test GetAppStatsMah function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_008, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_008 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId0);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_008 end");
}

/**
 * @tc.name: StatsCameraTest_009
 * @tc.desc: test GetAppStatsPercent function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_009, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_009 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId0);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_009 end");
}

/**
 * @tc.name: StatsCameraTest_010
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_010, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_010 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId0);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);

    double expectedPower = 3 * POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_010 end");
}

/**
 * @tc.name: StatsCameraTest_011
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_011, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_011 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid1 = 10003;
    int32_t pid1 = 3458;
    int32_t uid2 = 10004;
    int32_t pid2 = 3459;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid1, "UID", uid1, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid2, "UID", uid2, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = 2 * POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid1);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_011 end");
}

/**
 * @tc.name: StatsCameraTest_012
 * @tc.desc: test GetAppStatsMah function, Camera-Flashlight is opened when camera closed
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_012, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_012 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);

    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_TRUE(actualPower >= StatsUtils::DEFAULT_VALUE);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_012 end");
}

/**
 * @tc.name: StatsCameraTest_013
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_013, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_013 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT,
        HiSysEvent::EventType::STATISTIC, "ID", cameraId);
    usleep(POWER_CONSUMPTION_DURATION_US);

    double expectedPower = (2 * POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR) +
        (POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR);
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_013 end");
}

/**
 * @tc.name: StatsCameraTest_014
 * @tc.desc: test Reset function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_014, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_014 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_014 end");
}

/**
 * @tc.name: StatsCameraTest_015
 * @tc.desc: test GetAppStatsMah function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_015, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_015 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_015 end");
}

/**
 * @tc.name: StatsCameraTest_016
 * @tc.desc: test GetAppStatsPercent function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_016, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_016 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_016 end");
}

/**
 * @tc.name: StatsCameraTest_017
 * @tc.desc: test GetAppStatsMah(Torch) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_017, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_017 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", deviceId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_017 end");
}

/**
 * @tc.name: StatsCameraTest_018
 * @tc.desc: test GetAppStatsMah(Camera) and GetAppStatsPercent(Audio) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_018, TestSize.Level0)
{
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_018 start");
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";

    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "ID", deviceId);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);

    double expectedPower = POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEventWrite(HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE,
        HiSysEvent::EventType::BEHAVIOR, "PID", pid, "UID", uid, "STATE", stateRunning);
    usleep(POWER_CONSUMPTION_DURATION_US);
    HiSysEventWrite(HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE,
        HiSysEvent::EventType::BEHAVIOR, "PID", pid, "UID", uid, "STATE", stateStopped);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
    STATS_HILOGD(LABEL_TEST, "StatsCameraTest_018 end");
}
}