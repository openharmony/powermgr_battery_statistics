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

#include "stats_audio_test.h"

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

void StatsAudioTest::SetUpTestCase(void)
{
    ParserAveragePowerFile();
    dumpArgs.push_back("-batterystats");
    system("hidumper -s 3302 -a -u");
}

void StatsAudioTest::TearDownTestCase(void)
{
    system("hidumper -s 3302 -a -r");
}

void StatsAudioTest::SetUp(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(true);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 1 seconds";
    sleep(WAIT_TIME);
}

void StatsAudioTest::TearDown(void)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.SetOnBattery(false);
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsAudioTest_001
 * @tc.desc: test Reset function(Audio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_001: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    double powerMahBefore = statsClient.GetAppStatsMah(uid);
    statsClient.Reset();
    double powerMahAfter = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": before consumption = " << powerMahBefore << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": after consumption = " << powerMahAfter << " mAh";
    EXPECT_TRUE(powerMahBefore > StatsUtils::DEFAULT_VALUE && powerMahAfter == StatsUtils::DEFAULT_VALUE)
        << " StatsAudioTest_001 fail due to reset failed";
    GTEST_LOG_(INFO) << " StatsAudioTest_001: test end";
}

/**
 * @tc.name: StatsAudioTest_002
 * @tc.desc: test GetAppStatsMah function(Audio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_002: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    double deviation = 0.01;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsAudioTest_002 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsAudioTest_002: test end";
}

/**
 * @tc.name: StatsAudioTest_003
 * @tc.desc: test GetAppStatsPercent function(Audio)
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_003: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
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
        <<" StatsAudioTest_003 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsAudioTest_003: test end";
}

/**
 * @tc.name: StatsAudioTest_004
 * @tc.desc: test GetAppStatsMah function, Audio state composite test
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_004: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    int32_t stateReleased = 4;
    int32_t statePaused = 5;
    double deviation = 0.01;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateReleased);
    sleep(testWaitTimeSec);

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", statePaused);
    sleep(testWaitTimeSec);

    double expectedPower = 3 * testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsAudioTest_004 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsAudioTest_004: test end";
}

/**
 * @tc.name: StatsAudioTest_005
 * @tc.desc: test GetAppStatsMah function, Audio abnormal state test
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_005: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    int32_t stateInvalid = -1;
    int32_t stateAbnormal = 101;
    double deviation = 0.01;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateInvalid);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateAbnormal);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    double expectedPower = 2 * testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsAudioTest_005 fail due to power mismatch";
    GTEST_LOG_(INFO) << " StatsAudioTest_005: test end";
}

/**
 * @tc.name: StatsAudioTest_006
 * @tc.desc: test GetAppStatsMah(Audio) and GetAppStatsPercent(Sensor) function
 * @tc.type: FUNC
 */
HWTEST_F (StatsAudioTest, StatsAudioTest_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " StatsAudioTest_006: test start";
    auto& statsClient = BatteryStatsClient::GetInstance();
    statsClient.Reset();

    double audioOnAverageMa = g_statsParser->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    long testTimeSec = 2;
    long testWaitTimeSec = 1;
    int32_t uid = 10003;
    int32_t pid = 3458;
    int32_t stateRunning = 2;
    int32_t stateStopped = 3;
    double deviation = 0.01;

    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateRunning);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write("AUDIO", "AUDIO_STREAM_CHANGE", HiSysEvent::EventType::BEHAVIOR, "PID", pid,
        "UID", uid, "STATE", stateStopped);
    sleep(testWaitTimeSec);

    double expectedPower = testTimeSec * audioOnAverageMa / SECOND_PER_HOUR;
    double actualPower = statsClient.GetAppStatsMah(uid);
    GTEST_LOG_(INFO) << __func__ << ": expected consumption = " << expectedPower << " mAh";
    GTEST_LOG_(INFO) << __func__ << ": actual consumption = " << actualPower << " mAh";
    EXPECT_LE(abs(expectedPower - actualPower), deviation)
        <<" StatsAudioTest_006 fail due to power mismatch";

    uid = 10004;
    pid = 3459;
    int32_t stateOn = 1;
    int32_t stateOff = 0;
    double fullPercent = 1;
    double zeroPercent = 0;

    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOn);
    GTEST_LOG_(INFO) << __func__ << ": Sleep 2 seconds";
    sleep(testTimeSec);
    HiSysEvent::Write(HiSysEvent::Domain::POWERMGR, "POWER_SENSOR_GRAVITY", HiSysEvent::EventType::STATISTIC, "PID",
        pid, "UID", uid, "STATE", stateOff);
    sleep(testWaitTimeSec);
    double actualPercent = statsClient.GetAppStatsPercent(uid);
    GTEST_LOG_(INFO) << __func__ << ": actual percent = " << actualPercent;
    EXPECT_TRUE(actualPercent >= zeroPercent && actualPercent <= fullPercent)
        <<" StatsAudioTest_006 fail due to percent mismatch";
    GTEST_LOG_(INFO) << " StatsAudioTest_006: test end";
}
}