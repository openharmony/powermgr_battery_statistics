/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cJSON.h>
#include <gtest/gtest.h>
#include "battery_stats_core.h"
#include "battery_stats_listener.h"
#include "battery_stats_service.h"
#include "stats_hisysevent.h"
#include "stats_log.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

class StatsServiceConfigParseTest : public Test {
public:
    void SetUp() override
    {
        root_ = cJSON_CreateObject();
    }

    void TearDown() override
    {
        cJSON_Delete(root_);
    }

    cJSON* root_;
};

namespace {
constexpr size_t NUMBER_UID = 100;
constexpr size_t NUMBER_PID = 200;
constexpr size_t NUMBER_TWO = 2;
constexpr int8_t INVALID_VALUE = -1;

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest001 function start!");
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();

    statsCore->SaveForPower(root_);

    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_TRUE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest001 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest002 function start!");
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();

    statsCore->UpdateStatsEntity(root_);
    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_FALSE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest002 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest003 function start!");
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();
    statsCore->SaveForPower(root_);
    statsCore->UpdateStatsEntity(root_);
    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_TRUE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest003 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest004 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "ID", "camera0");

    std::string eventName = StatsHiSysEvent::CAMERA_CONNECT;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessCameraEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_CAMERA_ON);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_ACTIVATED);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    EXPECT_EQ(data.deviceId, "camera0");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest004 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest005, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest005 function start!");
    std::string eventName = StatsHiSysEvent::CAMERA_CONNECT;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessCameraEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_CAMERA_ON);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_ACTIVATED);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    EXPECT_EQ(data.deviceId, "");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest005 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest006, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest006 function start!");
    cJSON_AddNumberToObject(root_, "ID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "UID", "camera0");
    cJSON_AddStringToObject(root_, "PID", "camera0");

    std::string eventName = StatsHiSysEvent::CAMERA_CONNECT;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessCameraEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_CAMERA_ON);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_ACTIVATED);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    EXPECT_EQ(data.deviceId, "");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest006 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest007, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest007 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_TWO);

    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessAudioEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_AUDIO_ON);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_ACTIVATED);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest007 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest008, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest008 function start!");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessAudioEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_AUDIO_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest008 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest009, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest009 function start!");
    cJSON_AddStringToObject(root_, "UID", "test");
    cJSON_AddStringToObject(root_, "PID", "test1");
    cJSON_AddStringToObject(root_, "STATE", "test2");

    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessAudioEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_AUDIO_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest009 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest010, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest010 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddNumberToObject(root_, "STATE", 0);

    std::string eventName = StatsHiSysEvent::POWER_SENSOR_GRAVITY;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessSensorEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_DEACTIVATED);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest010 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest011, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest011 function start!");
    std::string eventName = StatsHiSysEvent::POWER_SENSOR_GRAVITY;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessSensorEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest011 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest012, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest012 function start!");
    cJSON_AddStringToObject(root_, "UID", "test");
    cJSON_AddStringToObject(root_, "PID", "test1");
    cJSON_AddStringToObject(root_, "STATE", "test2");

    std::string eventName = StatsHiSysEvent::POWER_SENSOR_GRAVITY;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessSensorEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest012 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest013, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest013 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "STATE", "start");

    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessGnssEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_GNSS_ON);
    EXPECT_EQ(data.state, StatsUtils::STATS_STATE_ACTIVATED);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest013 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest014, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest014 function start!");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessGnssEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_GNSS_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest014 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest015, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest015 function start!");
    cJSON_AddStringToObject(root_, "UID", "test");
    cJSON_AddStringToObject(root_, "PID", "test1");
    cJSON_AddNumberToObject(root_, "STATE", 0);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessGnssEvent(data, root_);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_GNSS_ON);
    EXPECT_EQ(data.state, INVALID_VALUE);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest015 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest016, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest016 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    std::string eventName = StatsHiSysEvent::DISCOVERY_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBrEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest016 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest017, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest017 function start!");
    StatsUtils::StatsData data;
    std::string eventName = StatsHiSysEvent::DISCOVERY_STATE;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBrEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest017 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest018, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest018 function start!");
    cJSON_AddStringToObject(root_, "UID", "test");
    cJSON_AddStringToObject(root_, "PID", "test1");
    cJSON_AddNumberToObject(root_, "STATE", 0);
    std::string eventName = StatsHiSysEvent::DISCOVERY_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBrEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest018 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest019, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest019 function start!");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    std::string eventName = StatsHiSysEvent::BLE_SCAN_START;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBleEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_PID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest019 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest020, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest020 function start!");
    StatsUtils::StatsData data;
    std::string eventName = StatsHiSysEvent::BLE_SCAN_START;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBleEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest020 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest021, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest021 function start!");
    cJSON_AddStringToObject(root_, "UID", "test");
    cJSON_AddStringToObject(root_, "PID", "test1");
    cJSON_AddNumberToObject(root_, "STATE", 0);
    std::string eventName = StatsHiSysEvent::BLE_SCAN_START;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBleEvent(data, root_, eventName);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    EXPECT_EQ(data.pid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest021 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest022, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest022 function start!");
    cJSON_AddNumberToObject(root_, "STATE", 0);
    cJSON_AddStringToObject(root_, "name_", "name_");
    cJSON_AddStringToObject(root_, "SLOT_ID", "SLOT_ID");
    cJSON_AddStringToObject(root_, "INDEX_ID", "INDEX_ID");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest022 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest023, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest023 function start!");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest023 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest024, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest024 function start!");
    cJSON_AddStringToObject(root_, "STATE", "state");
    cJSON_AddNumberToObject(root_, "name_", 0);
    cJSON_AddNumberToObject(root_, "SLOT_ID", 0);
    cJSON_AddNumberToObject(root_, "INDEX_ID", 0);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest024 function end!");
}
} // namespace
