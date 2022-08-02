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

#include <hisysevent.h>

#include "battery_stats_client.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace std;

static std::vector<std::string> dumpArgs;

void StatsClientTest::SetUpTestCase(void)
{
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
 * @tc.name: StatsLocationTest_001
 * @tc.desc: test GetTotalTimeSecond function(GNSS)
 * @tc.type: FUNC
 */
HWTEST_F (StatsClientTest, StatsLocationTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsLocationTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    std::string stateOn = "start";
    std::string stateOff = "stop";
    int32_t uid = 10003;
    int32_t pid = 3458;
    double deviation = 0.01;

    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::LOCATION, "GNSS_STATE", HiSysEvent::EventType::STATISTIC, "PID", pid, "UID",
        uid, "STATE", stateOff);
    sleep(testWaitTimeSec);

    long time = statsClient.GetTotalTimeSecond(StatsUtils::STATS_TYPE_GPS_ON, uid);
    GTEST_LOG_(INFO) << __func__ << ": expected time = " << testTimeSec << " seconds";
    GTEST_LOG_(INFO) << __func__ << ": actual time = " <<  time << " seconds";
    EXPECT_LE(abs(time - testTimeSec), deviation)
        <<" StatsLocationTest_001 fail due to time mismatch";
    GTEST_LOG_(INFO) << " StatsLocationTest_001: test end";
}
}