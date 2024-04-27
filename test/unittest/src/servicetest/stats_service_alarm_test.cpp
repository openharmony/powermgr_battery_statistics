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

#include "stats_service_alarm_test.h"

#include <hisysevent.h>

#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "stats_hisysevent.h"
#include "stats_service_test_proxy.h"
#include "stats_service_write_event.h"

using namespace OHOS;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;
using namespace testing::ext;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
} // namespace

void StatsServiceAlarmTest::SetUpTestCase()
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

void StatsServiceAlarmTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceAlarmTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceAlarmTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceAlarmTest_001
 * @tc.desc: test Reset function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceAlarmTest_002
 * @tc.desc: test GetPartStatsMah function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceAlarmTest_003
 * @tc.desc: test GetAppStatsPercent function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;
    double fullPercent = 1;
    double zeroPercent = 0;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceAlarmTest_004
 * @tc.desc: test SetOnBattery function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();
    g_statsServiceProxy->SetOnBattery(false);

    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    g_statsServiceProxy->SetOnBattery(true);
}

/**
 * @tc.name: StatsServiceAlarmTest_005
 * @tc.desc: test alarm entity GetPartStatsMah function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    auto statsCore = statsService->GetBatteryStatsCore();
    auto alarmEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM);
    statsCore->ComputePower();

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = alarmEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_ALARM, uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, alarmEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_ALARM));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, alarmEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID, uid));
}

/**
 * @tc.name: StatsServiceAlarmTest_006
 * @tc.desc: test uid entity GetPartStatsMah function(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double alarmOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC,
            "CALLER_PID", pid, "CALLER_UID", uid);
    }

    auto statsCore = statsService->GetBatteryStatsCore();
    auto uidEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    statsCore->ComputePower();

    double expectedPower = count * alarmOnAverageMa;
    double actualPower = uidEntity->GetEntityPowerMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceAlarmTest_007
 * @tc.desc: test send hisysevent with missing information(Alarm)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceAlarmTest, StatsServiceAlarmTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::TIME, StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT, HiSysEvent::EventType::STATISTIC);
    }

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}
}