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

#include "stats_camera_test.h"

#include <hisysevent.h>

#include "battery_stats_client.h"
#include "battery_stats_parser.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
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

void StatsCameraTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}

void StatsCameraTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsCameraTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsCameraTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsCameraTest_001
 * @tc.desc: test Reset function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsClientTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsCameraTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCameraTest_001: test end";
}

/**
 * @tc.name: StatsCameraTest_002
 * @tc.desc: test GetAppStatsMah function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_002: test end";
}

/**
 * @tc.name: StatsCameraTest_003
 * @tc.desc: test GetAppStatsPercent function(Camera)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_003: test end";
}

/**
 * @tc.name: StatsCameraTest_004
 * @tc.desc: test Reset function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsClientTest_066: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Camera sleep 2 seconds";
    sleep(testTimeSec);
    GTEST_LOG_(INFO) << __func__ << ": Notify camera flashfight";
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_ON", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Camera flashlight sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_OFF", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsCameraTest_004 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCameraTest_004: test end";
}

/**
 * @tc.name: StatsCameraTest_005
 * @tc.desc: test GetAppStatsMah function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_ON", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Camera flashlight sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_OFF", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double expectedPower = (3 * testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR) +
        (testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR);
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_005: test end";
}

/**
 * @tc.name: StatsCameraTest_006
 * @tc.desc: test GetAppStatsPercent function(Camera with Flashlight)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_ON", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Camera flashlight sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_OFF", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_006 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_006: test end";
}

/**
 * @tc.name: StatsCameraTest_007
 * @tc.desc: test Reset function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_007: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    sleep(testWaitTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsCameraTest_007 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCameraTest_007: test end";
}

/**
 * @tc.name: StatsCameraTest_008
 * @tc.desc: test GetAppStatsMah function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_008: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    sleep(testWaitTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_008 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_008: test end";
}

/**
 * @tc.name: StatsCameraTest_009
 * @tc.desc: test GetAppStatsPercent function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_009: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    sleep(testWaitTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    sleep(testWaitTimeSec);

    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_009 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_009: test end";
}

/**
 * @tc.name: StatsCameraTest_010
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_010: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_010 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_010: test end";
}

/**
 * @tc.name: StatsCameraTest_011
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_011: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid1 = 10003;
    int32_t pid1 = 3458;
    int32_t uid2 = 10004;
    int32_t pid2 = 3459;
    std::string cameraId = "Camera0";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid1,
        "UID", uid1, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid2,
        "UID", uid2, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid1);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_011 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_011: test end";
}

/**
 * @tc.name: StatsCameraTest_012
 * @tc.desc: test GetAppStatsMah function, Camera-Flashlight is opened when camera closed
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_012: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;

    HiSysEvent::Write("CAMERA", "FLASHLIGHT_ON", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_OFF", HiSysEvent::EventType::STATISTIC);
    sleep(testWaitTimeSec);

    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_TRUE(actualPower == StatsUtils::DEFAULT_VALUE)
        <<" StatsCameraTest_012 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_012: test end";
}

/**
 * @tc.name: StatsCameraTest_013
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_013: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "FLASHLIGHT_ON", HiSysEvent::EventType::STATISTIC);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC, "ID", cameraId);
    sleep(testTimeSec);
    sleep(testWaitTimeSec);

    double expectedPower = (2 * testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR) +
        (testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR);
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_013 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_013: test end";
}

/**
 * @tc.name: StatsCameraTest_014
 * @tc.desc: test Reset function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_014: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsCameraTest_014 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsCameraTest_014: test end";
}

/**
 * @tc.name: StatsCameraTest_015
 * @tc.desc: test GetAppStatsMah function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_015: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_015 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_015: test end";
}

/**
 * @tc.name: StatsCameraTest_016
 * @tc.desc: test GetAppStatsPercent function(Torch)
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_016: test start";
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

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_016 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_016: test end";
}

/**
 * @tc.name: StatsCameraTest_017
 * @tc.desc: test GetAppStatsMah(Torch) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_017: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "TORCH_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * flashlightOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_017 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_017 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_017: test end";
}

/**
 * @tc.name: StatsCameraTest_018
 * @tc.desc: test GetAppStatsMah(Camera) and GetAppStatsPercent(Audio) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsCameraTest, StatsCameraTest_018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsCameraTest_018: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";
    double deviation = 0.01;

    HiSysEvent::Write("CAMERA", "CAMERA_CONNECT", HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("CAMERA", "CAMERA_DISCONNECT", HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * cameraOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsCameraTest_018 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsCameraTest_018 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsCameraTest_018: test end";
}
}