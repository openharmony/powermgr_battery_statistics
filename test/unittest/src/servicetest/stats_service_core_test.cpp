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

#include "stats_service_core_test.h"

#include "battery_stats_core.h"
#include "battery_stats_service.h"

using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace std;
using namespace testing::ext;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
} // namespace

void StatsServiceCoreTest::SetUpTestCase()
{
    g_statsService = BatteryStatsService::GetInstance();
    g_statsService->OnStart();
}

void StatsServiceCoreTest::TearDownTestCase()
{
    g_statsService->OnStop();
}

void StatsServiceCoreTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    statsCore->Reset();
}

void StatsServiceCoreTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    statsCore->Reset();
}

namespace {
/**
 * @tc.name: StatsServiceCoreTest_001
 * @tc.desc: test BatteryStatsCore function GetEntity
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_001, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    EXPECT_EQ(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_INVALID));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_IDLE));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_USER));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GNSS));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK));
    EXPECT_NE(nullptr, statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM));
}

/**
 * @tc.name: StatsServiceCoreTest_002
 * @tc.desc: test BatteryStatsCore function update
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_002, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    statsService->SetOnBattery(true);
    int64_t time = 100;
    int64_t data = 10;
    statsCore->UpdateStats(StatsUtils::STATS_TYPE_WIFI_SCAN, time, data);
    statsCore->UpdateStats(StatsUtils::STATS_TYPE_WAKELOCK_HOLD, time, data);
    std::shared_ptr<BatteryStatsEntity> wifiEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    EXPECT_EQ(data, wifiEntity->GetConsumptionCount(StatsUtils::STATS_TYPE_WIFI_SCAN));

    statsCore->UpdateStats(StatsUtils::STATS_TYPE_WIFI_ON, StatsUtils::STATS_STATE_ACTIVATED);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    statsCore->UpdateStats(StatsUtils::STATS_TYPE_WIFI_SCAN, StatsUtils::STATS_STATE_DEACTIVATED);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    statsCore->UpdateStats(StatsUtils::STATS_TYPE_WIFI_ON, StatsUtils::STATS_STATE_DEACTIVATED);
    EXPECT_GT(wifiEntity->GetActiveTimeMs(StatsUtils::STATS_TYPE_WIFI_ON),
        ((2 * SERVICE_POWER_CONSUMPTION_DURATION_US) / 3) * 2 / US_PER_MS);
    statsService->SetOnBattery(false);
}

/**
 * @tc.name: StatsServiceCoreTest_003
 * @tc.desc: test BatteryStatsCore with entity is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_003, TestSize.Level0)
{
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();
    std::string expectedDebugInfo;
    expectedDebugInfo.append("BATTERY STATS DUMP:\n").append("\n");
    std::string actualDebugInfo {};
    statsCore->DumpInfo(actualDebugInfo);
    auto index = actualDebugInfo.find(expectedDebugInfo);
    EXPECT_TRUE(index != string::npos);
}

/**
 * @tc.name: StatsServiceCoreTest_004
 * @tc.desc: test Entity Function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_004, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    int32_t uid = 1003;

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_ALARM)->GetConsumptionCount(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_ALARM)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_ALARM)->GetOrCreateCounter(StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH)->GetActiveTimeMs(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH)->GetOrCreateTimer(StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_GNSS)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_GNSS)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_GNSS)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));
}

/**
 * @tc.name: StatsServiceCoreTest_005
 * @tc.desc: test Entity Function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_005, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    int32_t uid = 1003;

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_PHONE)->GetActiveTimeMs(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_PHONE)->GetOrCreateTimer(StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN)->GetActiveTimeMs(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN)->GetOrCreateTimer(StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_USER)->GetEntityPowerMah());

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK)->GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK)->GetEntityPowerMah());
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK)->GetOrCreateTimer(uid, StatsUtils::STATS_TYPE_INVALID));

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WIFI)->GetActiveTimeMs(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WIFI)->GetOrCreateTimer(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WIFI)->GetConsumptionCount(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(nullptr, statsCore->GetEntity(
        BatteryStatsInfo::CONSUMPTION_TYPE_WIFI)->GetOrCreateCounter(StatsUtils::STATS_TYPE_INVALID));
}

/**
 * @tc.name: StatsServiceCoreTest_006
 * @tc.desc: test Cpu and Idle Entity Function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_006, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    auto cpuEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    auto idleEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_IDLE);
    int32_t invalidUid = -1;
    cpuEntity->Reset();
    cpuEntity->UpdateCpuTime();
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetCpuTimeMs(invalidUid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetEntityPowerMah(invalidUid));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_ACTIVE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_CLUSTER));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_SPEED));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID));

    idleEntity->Reset();
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, idleEntity->GetActiveTimeMs(StatsUtils::STATS_TYPE_INVALID));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, idleEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_PHONE_IDLE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, idleEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_SUSPEND));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, idleEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID));
}

/**
 * @tc.name: StatsServiceCoreTest_007
 * @tc.desc: test Uid Entity Function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceCoreTest, StatsServiceCoreTest_007, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    auto statsCore = statsService->GetBatteryStatsCore();
    auto uidEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    int32_t invalidUid = -1;
    uidEntity->Reset();
    uidEntity->UpdateUidMap(invalidUid);
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_BLUETOOTH_BLE_SCAN));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CAMERA_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_FLASHLIGHT_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_GNSS_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_AUDIO_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_WAKELOCK_HOLD));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_CLUSTER));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_SPEED));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_ACTIVE));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_ALARM));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, uidEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID));
}
}