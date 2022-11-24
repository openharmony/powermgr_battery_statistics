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

#include "stats_util_test.h"

#include "stats_helper.h"
#include "stats_hisysevent.h"
#include "stats_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace std;

void StatsUtilTest::SetUpTestCase()
{
}

void StatsUtilTest::TearDownTestCase()
{
}

void StatsUtilTest::SetUp()
{
    StatsHelper::SetOnBattery(true);
}

void StatsUtilTest::TearDown()
{
    StatsHelper::SetOnBattery(false);
}

namespace {
/**
 * @tc.name: StatsHiSysEvent_001
 * @tc.desc: test StatsHiSysEvent function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHiSysEvent_001, TestSize.Level0)
{
    EXPECT_TRUE(StatsHiSysEvent::CheckHiSysEvent("POWER_RUNNINGLOCK"));
    EXPECT_FALSE(StatsHiSysEvent::CheckHiSysEvent("POWER_RUNNINGLOCK_WRONG"));
}

/**
 * @tc.name: StatsUtils_001
 * @tc.desc: test class StatsUtils ConvertStatsType function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsUtils_001, TestSize.Level0)
{
    EXPECT_EQ("", StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_INVALID));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_BR_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_BLUETOOTH_BR_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_BR_SCAN),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_BLUETOOTH_BR_SCAN));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_BLE_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_BLUETOOTH_BLE_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_BLE_SCAN),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_BLUETOOTH_BLE_SCAN));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_WIFI_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_WIFI_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_WIFI_SCAN),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_WIFI_SCAN));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_PHONE_ACTIVE),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_PHONE_ACTIVE));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_PHONE_DATA),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_PHONE_DATA));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_BATTERY),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_BATTERY));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_WORKSCHEDULER),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_WORKSCHEDULER));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_THERMAL),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_THERMAL));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_DISTRIBUTEDSCHEDULER),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_DISTRIBUTEDSCHEDULER));
}

/**
 * @tc.name: StatsUtils_002
 * @tc.desc: test class StatsUtils ConvertStatsType function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsUtils_002, TestSize.Level0)
{
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CAMERA_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CAMERA_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CAMERA_FLASHLIGHT_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CAMERA_FLASHLIGHT_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_FLASHLIGHT_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_FLASHLIGHT_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_GNSS_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_GNSS_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_SENSOR_GRAVITY_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_SENSOR_GRAVITY_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_SENSOR_PROXIMITY_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_SENSOR_PROXIMITY_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_AUDIO_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_AUDIO_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_SCREEN_ON),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_SCREEN_ON));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_SCREEN_BRIGHTNESS),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_SCREEN_BRIGHTNESS));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_ALARM),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_ALARM));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_WAKELOCK_HOLD),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_WAKELOCK_HOLD));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_PHONE_IDLE),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_PHONE_IDLE));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CPU_CLUSTER),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CPU_CLUSTER));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CPU_SPEED),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CPU_SPEED));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CPU_ACTIVE),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CPU_ACTIVE));
    EXPECT_EQ(GET_VARIABLE_NAME(STATS_TYPE_CPU_SUSPEND),
        StatsUtils::ConvertStatsType(StatsUtils::StatsType::STATS_TYPE_CPU_SUSPEND));
}

/**
 * @tc.name: StatsHelper_001
 * @tc.desc: test class ActiveTimer function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_001, TestSize.Level0)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    int64_t devTimeMs = StatsUtils::DEFAULT_VALUE;
    std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
    timer->StartRunning();
    usleep(TIMER_DURATION_MS * US_PER_MS);
    timer->StopRunning();
    activeTimeMs = timer->GetRunningTimeMs();
    devTimeMs = abs(activeTimeMs - TIMER_DURATION_MS);
    EXPECT_LE(devTimeMs, DEVIATION_TIMER_THRESHOLD);

    int64_t addTimeMs = 20;
    timer->AddRunningTimeMs(addTimeMs);
    activeTimeMs = timer->GetRunningTimeMs();
    devTimeMs = abs(activeTimeMs - TIMER_DURATION_MS - addTimeMs);
    EXPECT_LE(devTimeMs, DEVIATION_TIMER_THRESHOLD);

    timer->AddRunningTimeMs(StatsUtils::DEFAULT_VALUE);
    EXPECT_EQ(timer->GetRunningTimeMs(), activeTimeMs);

    timer->Reset();
    activeTimeMs = timer->GetRunningTimeMs();
    EXPECT_EQ(activeTimeMs, StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsHelper_002
 * @tc.desc: test class ActiveTimer function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_002, TestSize.Level0)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    int64_t devTimeMs = StatsUtils::DEFAULT_VALUE;
    std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
    timer->StartRunning();
    usleep(TIMER_DURATION_MS * US_PER_MS);
    activeTimeMs = timer->GetRunningTimeMs();
    devTimeMs = abs(activeTimeMs - TIMER_DURATION_MS);
    EXPECT_LE(devTimeMs, DEVIATION_TIMER_THRESHOLD);
    timer->StopRunning();

    timer->Reset();
    activeTimeMs = timer->GetRunningTimeMs();
    EXPECT_EQ(activeTimeMs, StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsHelper_003
 * @tc.desc: test class ActiveTimer function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_003, TestSize.Level0)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    int64_t devTimeMs = StatsUtils::DEFAULT_VALUE;
    std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
    StatsHelper::SetOnBattery(false);
    EXPECT_FALSE(StatsHelper::IsOnBattery());
    timer->StartRunning();
    usleep(TIMER_DURATION_MS * US_PER_MS);
    StatsHelper::SetOnBattery(true);
    EXPECT_TRUE(StatsHelper::IsOnBattery());
    usleep(TIMER_DURATION_MS * US_PER_MS);
    timer->StopRunning();

    activeTimeMs = timer->GetRunningTimeMs();
    devTimeMs = abs(activeTimeMs - TIMER_DURATION_MS);
    EXPECT_LE(devTimeMs, DEVIATION_TIMER_THRESHOLD);

    timer->Reset();
    activeTimeMs = timer->GetRunningTimeMs();
    EXPECT_EQ(activeTimeMs, StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsHelper_004
 * @tc.desc: test class Counter function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_004, TestSize.Level0)
{
    int64_t activeCount = StatsUtils::DEFAULT_VALUE;
    int64_t addCount = 20;
    std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
    StatsHelper::SetOnBattery(false);
    EXPECT_FALSE(StatsHelper::IsOnBattery());
    counter->AddCount(addCount);
    counter->AddCount(StatsUtils::DEFAULT_VALUE);
    StatsHelper::SetOnBattery(true);
    EXPECT_TRUE(StatsHelper::IsOnBattery());
    counter->AddCount(addCount);

    activeCount = counter->GetCount();
    EXPECT_EQ(activeCount, addCount);

    counter->Reset();
    activeCount = counter->GetCount();
    EXPECT_EQ(activeCount, StatsUtils::DEFAULT_VALUE);
}

/**
 * @tc.name: StatsHelper_005
 * @tc.desc: test class Counter function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_005, TestSize.Level0)
{
    StatsHelper::SetOnBattery(false);
    EXPECT_FALSE(StatsHelper::IsOnBattery());
    StatsHelper::SetScreenOff(false);
    EXPECT_FALSE(StatsHelper::IsOnBatteryScreenOff());

    StatsHelper::SetOnBattery(false);
    EXPECT_FALSE(StatsHelper::IsOnBattery());
    StatsHelper::SetScreenOff(true);
    EXPECT_FALSE(StatsHelper::IsOnBatteryScreenOff());

    StatsHelper::SetOnBattery(true);
    EXPECT_TRUE(StatsHelper::IsOnBattery());
    StatsHelper::SetScreenOff(false);
    EXPECT_FALSE(StatsHelper::IsOnBatteryScreenOff());

    StatsHelper::SetOnBattery(true);
    EXPECT_TRUE(StatsHelper::IsOnBattery());
    StatsHelper::SetScreenOff(true);
    EXPECT_TRUE(StatsHelper::IsOnBatteryScreenOff());

    StatsHelper::SetScreenOff(false);
}

/**
 * @tc.name: StatsHelper_006
 * @tc.desc: test class Counter function
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F (StatsUtilTest, StatsHelper_006, TestSize.Level0)
{
    int64_t timeMs = StatsUtils::DEFAULT_VALUE;
    int64_t durationTimeMs = StatsUtils::DEFAULT_VALUE;
    int64_t devTimeMs = StatsUtils::DEFAULT_VALUE;
    StatsHelper::SetOnBattery(false);
    EXPECT_FALSE(StatsHelper::IsOnBattery());
    timeMs = StatsHelper::GetOnBatteryUpTimeMs();
    usleep(TIMER_DURATION_MS * US_PER_MS);
    durationTimeMs = abs(StatsHelper::GetOnBatteryUpTimeMs() - timeMs);
    EXPECT_EQ(durationTimeMs, 0);

    StatsHelper::SetOnBattery(true);
    EXPECT_TRUE(StatsHelper::IsOnBattery());
    timeMs = StatsHelper::GetOnBatteryUpTimeMs();
    usleep(TIMER_DURATION_MS * US_PER_MS);
    durationTimeMs = abs(StatsHelper::GetOnBatteryUpTimeMs() - timeMs);
    devTimeMs = abs(durationTimeMs - TIMER_DURATION_MS);
    EXPECT_LE(devTimeMs, DEVIATION_TIMER_THRESHOLD);
}
}