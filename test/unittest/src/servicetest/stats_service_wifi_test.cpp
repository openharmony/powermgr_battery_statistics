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

#include "stats_service_wifi_test.h"

#include <hisysevent.h>
#include "wifi_msg.h"

#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "stats_hisysevent.h"
#include "stats_service_test_proxy.h"
#include "stats_service_write_event.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
static sptr<BatteryStatsService> g_statsService = nullptr;
static std::shared_ptr<StatsServiceTestProxy> g_statsServiceProxy = nullptr;
} // namespace

void StatsServiceWifiTest::SetUpTestCase()
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

void StatsServiceWifiTest::TearDownTestCase()
{
    g_statsService->listenerPtr_ = nullptr;
    g_statsService->OnStop();
}

void StatsServiceWifiTest::SetUp()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(true);
}

void StatsServiceWifiTest::TearDown()
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsServiceWifiTest_001
 * @tc.desc: test Reset function(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_001, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double powerMahBefore = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceWifiTest_002
 * @tc.desc: test GetPartStatsMah function(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_002, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_003
 * @tc.desc: test GetPartStatsPercent function(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_003, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double actualPercent = g_statsServiceProxy->GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceWifiTest_004
 * @tc.desc: test GetBatteryStats function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_004, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = g_statsServiceProxy->GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_WIFI) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_005
 * @tc.desc: test WIFI_CONNECTION event are sent repeatedly, wifi on power consumption(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_005, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_006
 * @tc.desc: test test GetAppStatsMah function, Wifi connection abnormal state test(Wifi is off)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_006, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = 3;
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}

/**
 * @tc.name: StatsServiceWifiTest_007
 * @tc.desc: test test GetAppStatsMah function, Wifi connection abnormal state test(Wifi is on)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_007, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    int32_t stateInvaildOn = 5;
    int32_t stateInvaildOff = -1;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateInvaildOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService, HiSysEvent::Domain::COMMUNICATION,
    StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateInvaildOff);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = 3 * SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_008
 * @tc.desc: test GetTotalTimeSecond function(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_008, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    long expectedTime = round(SERVICE_POWER_CONSUMPTION_DURATION_US / US_PER_SECOND);
    long actualTime = g_statsServiceProxy->GetTotalTimeSecond(StatsUtils::STATS_TYPE_WIFI_ON);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << expectedTime << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  actualTime << " seconds";
    EXPECT_EQ(expectedTime, actualTime);
}

/**
 * @tc.name: StatsServiceWifiTest_09
 * @tc.desc: test Reset function(Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_09, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double powerMahBefore = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    g_statsServiceProxy->Reset();
    double powerMahAfter = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore >= StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsServiceWifiTest_010
 * @tc.desc: test GetPartStatsMah function(Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_010, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double expectedPower = count * wifiScanAverageMa;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_011
 * @tc.desc: test GetPartStatsPercent function(Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_011, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double fullPercent = 1;
    double zeroPercent = 0;
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double actualPercent = g_statsServiceProxy->GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceWifiTest_012
 * @tc.desc: test GetBatteryStats function(Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_012, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    int16_t count = 10;

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double expectedPower = count * wifiScanAverageMa;
    double actualPower = StatsUtils::DEFAULT_VALUE;
    auto list = g_statsServiceProxy->GetBatteryStats();
    for (auto it : list) {
        if ((*it).GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_WIFI) {
            actualPower = (*it).GetPower();
        }
    }
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_013
 * @tc.desc: test GetTotalDataBytes function (Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_013, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int16_t count = 10;
    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    long expectValue = StatsUtils::DEFAULT_VALUE;
    long data = g_statsServiceProxy->GetTotalDataBytes(StatsUtils::STATS_TYPE_WIFI_SCAN);
    EXPECT_EQ(data, expectValue);
}

/**
 * @tc.name: StatsServiceWifiTest_014
 * @tc.desc: test GetPartStatsMah function(Wifi connection & Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_014, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    int16_t count = 10;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double wifiOnPower = SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double wifiScanPower = count * wifiScanAverageMa;

    double expectedPower = wifiOnPower + wifiScanPower;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_015
 * @tc.desc: test SetOnBattery function(Wifi connection & Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_015, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();
    g_statsServiceProxy->SetOnBattery(false);

    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    int16_t count = 10;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    for (int16_t i = 0; i < count; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
    }

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
    g_statsServiceProxy->SetOnBattery(true);
}

/**
 * @tc.name: StatsServiceWifiTest_016
 * @tc.desc: test GetPartStatsMah function with battery changed(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_016, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    g_statsServiceProxy->SetOnBattery(false);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    g_statsServiceProxy->SetOnBattery(true);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double expectedPower = 2 * SERVICE_POWER_CONSUMPTION_DURATION_US * wifiOnAverageMa / US_PER_HOUR;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_017
 * @tc.desc: test GetPartStatsMah function with battery changed(Wifi scan)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_017, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    double wifiScanAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    int16_t totalCount = 20;
    int16_t delayCount = 10;
    int16_t startDelayPos = 5;

    for (int16_t i = 0; i < totalCount; i++) {
        StatsWriteHiSysEvent(statsService,
            HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_SCAN, HiSysEvent::EventType::STATISTIC);
        if (i == startDelayPos) {
            g_statsServiceProxy->SetOnBattery(false);
        } else if (i == startDelayPos + delayCount)
        {
            g_statsServiceProxy->SetOnBattery(true);
        }
    }

    double expectedPower = (totalCount - delayCount) * wifiScanAverageMa;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    double devPrecent = abs(expectedPower - actualPower) / expectedPower;
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(devPrecent, DEVIATION_PERCENT_THRESHOLD);
}

/**
 * @tc.name: StatsServiceWifiTest_018
 * @tc.desc: test GetPartStatsPercent(Wifi on) and GetAppStatsPercent(Camera) function
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_018, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    int32_t stateOn = static_cast<int32_t>(Wifi::ConnState::CONNECTED);
    int32_t stateOff = static_cast<int32_t>(Wifi::ConnState::DISCONNECTED);
    double fullPercent = 1;
    double zeroPercent = 0;

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOn);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION,
        HiSysEvent::EventType::STATISTIC, "TYPE", stateOff);

    double actualPercent = g_statsServiceProxy->GetPartStatsPercent(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);

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
    actualPercent = g_statsServiceProxy->GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent);
}

/**
 * @tc.name: StatsServiceWifiTest_019
 * @tc.desc: test send hisysevent with missing information(Wifi connection)
 * @tc.type: FUNC
 * @tc.require: issueI663DX
 */
HWTEST_F (StatsServiceWifiTest, StatsServiceWifiTest_019, TestSize.Level0)
{
    ASSERT_NE(g_statsServiceProxy, nullptr);
    auto statsService = BatteryStatsService::GetInstance();
    g_statsServiceProxy->Reset();

    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION, HiSysEvent::EventType::STATISTIC);
    usleep(SERVICE_POWER_CONSUMPTION_DURATION_US);
    StatsWriteHiSysEvent(statsService,
        HiSysEvent::Domain::COMMUNICATION, StatsHiSysEvent::WIFI_CONNECTION, HiSysEvent::EventType::STATISTIC);

    double expectedPower = StatsUtils::DEFAULT_VALUE;
    double actualPower = g_statsServiceProxy->GetPartStatsMah(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_EQ(expectedPower, actualPower);
}
}