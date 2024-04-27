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

#include "stats_service_location_test.h"

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

void StatsServiceLocationTest::SetUpTestCase()
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

void StatsServiceLocationTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceLocationTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceLocationTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceLocationTest_001
 * @tc.desc: test Reset function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double powerMahBefore = g_statsServiceProxy->GetAppStatsMah(uid);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceLocationTest_002
 * @tc.desc: test GetAppStatsMah function(Gnss)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceLocationTest_003
 * @tc.desc: test GetAppStatsPercent function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceLocationTest_004
 * @tc.desc: test GNSS_STATE event are sent repeatedly, gnss power consumption(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceLocationTest_005
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is off)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateInvaildOn = "star";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsServiceLocationTest_006
 * @tc.desc: test test GetAppStatsMah function, GNSS abnormal state test(GNSS is On)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    std::string stateInvaildOn = "star";
    std::string stateInvaildOff = "stp";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateInvaildOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 3 * SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceLocationTest_007
 * @tc.desc: test GetTotalTimeSecond function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    long expectedTime = round(SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = g_statsServiceProxy->GetTotalTimeSecond(StatsUtils::STATS_TYPE_GNSS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
}

/**
 * @tc.name: StatsServiceLocationTest_008
 * @tc.desc:  test test GetAppStatsMah function with different UID(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_008, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uidOne = 10003;
    int32_t pidOne = 3458;
    int32_t uidTwo = 10004;
    int32_t pidTwo = 3459;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidTwo, "UID", uidTwo, "STATE", stateOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pidOne, "UID", uidOne, "STATE", stateOff);

    double expectedPower = 3 * SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uidOne);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected first uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual first uid consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    actualPower = g_statsServiceProxy->GetAppStatsMah(uidTwo);
    devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected second uid consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual second uid consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceLocationTest_009
 * @tc.desc: test SetOnBattery function(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_009, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();
    g_statsServiceProxy->SetOnBattery(false);

    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    g_statsServiceProxy->SetOnBattery(true);
}

/**
 * @tc.name: StatsServiceLocationTest_010
 * @tc.desc: test GetPartStatsMah function with battery changed(GNSS)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_010, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    g_statsServiceProxy->SetOnBattery(false);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    g_statsServiceProxy->SetOnBattery(true);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceLocationTest_011
 * @tc.desc: test gnss entity GetPartStatsMah function(Gnss)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_011, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double gnssOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    int32_t uid = 10003;
    int32_t pid = 3458;
    std::string stateOn = "start";
    std::string stateOff = "stop";

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE,
        HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", stateOff);

    auto statsCore = statsService->GetBatteryStatsCore();
    auto gnssEntity = statsCore->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GNSS);
    statsCore->ComputePower();

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * gnssOnAverageMa / US_PER_HOUR;
    double actualPower = gnssEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_GNSS_ON, uid);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);

    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, gnssEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_GNSS_ON));
    EXPECT_EQ(StatsUtils::DEFAULT_VALUE, gnssEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_INVALID, uid));
}

/**
 * @tc.name: StatsServiceLocationTest_012
 * @tc.desc: test send hisysevent with missing information(Gnss)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceLocationTest, StatsServiceLocationTest_012, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t uid = 10003;
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::LOCATION, StatsHiSysEvent::GNSS_STATE, HiSysEvent::EventType::STATISTIC);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}
}