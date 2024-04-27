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

#include "stats_service_camera_test.h"

#include <hisysevent.h>

#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "stats_hisysevent.h"
#include "stats_service_test_proxy.h"
#include "stats_service_write_event.h"

using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
} // namespace

void StatsServiceCameraTest::SetUpTestCase()
{
    ParserAveragePowerFile();
    g_statsService = BatteryStatsService::GetInstance();
    g_statsService->OnStart();

    if (g_statsService->listenerPtr_ == nullptr) {
        g_statsService->listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }

    if (g_statsServiceProxy == nullptr) {
        g_statsServiceProxy = std::make_shared<StatsServiceTestProxy>(g_statsService);
    }
}

void StatsServiceCameraTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceCameraTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceCameraTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceCameraTest_001
 * @tc.desc: test Reset function(Camera)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceCameraTest_002
 * @tc.desc: test GetAppStatsMah function(Camera)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_003
 * @tc.desc: test GetAppStatsPercent function(Camera)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_004
 * @tc.desc: test Reset function(Camera with Flashlight)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceCameraTest_005
 * @tc.desc: test GetAppStatsMah function(Camera with Flashlight)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = (3 * SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR) +
        (SERVICE_POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR);
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_006
 * @tc.desc: test GetAppStatsPercent function(Camera with Flashlight)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_007
 * @tc.desc: test Reset function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceCameraTest_008
 * @tc.desc: test GetAppStatsMah function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_008, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_009
 * @tc.desc: test GetAppStatsPercent function, The same Uid triggers the camera(different id) continuously
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_009, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);

    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_010
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_010, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId0 = "Camera0";
    std::string cameraId1 = "Camera1";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId0);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId1);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId0);

    double expectedPower = 3 * SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_011
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_011, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid1 = 10003;
    int32_t pid1 = 3458;
    int32_t uid2 = 10004;
    int32_t pid2 = 3459;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid1,
        "UID", uid1, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid2,
        "UID", uid2, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid1);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_012
 * @tc.desc: test GetAppStatsMah function, Camera-Flashlight is opened when camera closed
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_012, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_OFF, HiSysEvent::EventType::STATISTIC);

    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_TRUE(actualPower >= StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceCameraTest_013
 * @tc.desc: test GetAppStatsMah function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_013, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::FLASHLIGHT_ON, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT,
        HiSysEvent::EventType::STATISTIC, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);

    double expectedPower = (2 * SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR) +
        (SERVICE_POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR);
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_014
 * @tc.desc: test Reset function(Torch)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_014, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceCameraTest_015
 * @tc.desc: test GetAppStatsMah function(Torch)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_015, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceCameraTest_016
 * @tc.desc: test GetAppStatsPercent function(Torch)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_016, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);
    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_017
 * @tc.desc: test GetAppStatsMah(Torch) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_017, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    uid = 10004;
    pid = 3459;
    std::string deviceId = "Camera0";
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);
    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_018
 * @tc.desc: test GetAppStatsMah(Camera) and GetAppStatsPercent(Audio) function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_018, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string deviceId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", deviceId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", deviceId);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
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

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE, HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::AUDIO, StatsHiSysEvent::STREAM_CHANGE, HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceCameraTest_019
 * @tc.desc: test camera entity GetPartStatsMah function(Camera)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_019, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double cameraOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string cameraId = "Camera0";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "ID", cameraId);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC,
        "ID", cameraId);

    auto statsCore = statsService->GetBatteryStatsCore();
    auto cameraEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    statsCore->ComputePower();

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * cameraOnAverageMa / US_PER_HOUR;
    double actualPower = cameraEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CAMERA_ON, uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cameraEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CAMERA_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cameraEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID, uid));
}

/**
 * @tc.name: StatsServiceCameraTest_020
 * @tc.desc: test flashlight entity GetPartStatsMah function(Torch)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_020, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double flashlightOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateOn = 1;
    int32_t stateOff = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE, HiSysEvent::EventType::STATISTIC, "PID", pid,
        "UID", uid, "STATE", stateOff);

    auto statsCore = statsService->GetBatteryStatsCore();
    auto flashlightEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    statsCore->ComputePower();

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * flashlightOnAverageMa / US_PER_HOUR;
    double actualPower = flashlightEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_FLASHLIGHT_ON, uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, flashlightEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_FLASHLIGHT_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, flashlightEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID, uid));
}

/**
 * @tc.name: StatsServiceCameraTest_021
 * @tc.desc: test send hisysevent with missing information(Camera & Flashlight)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCameraTest, StatsServiceCameraTest_021, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_CONNECT,
        HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::CAMERA, StatsHiSysEvent::CAMERA_DISCONNECT, HiSysEvent::EventType::STATISTIC);

    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::CAMERA, StatsHiSysEvent::TORCH_STATE,
        HiSysEvent::EventType::STATISTIC);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}
}