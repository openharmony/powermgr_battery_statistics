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
constexpr int8_t NUMBER_0 = 0;
constexpr int8_t NUMBER_1 = 1;
constexpr int8_t NUMBER_2 = 2;
constexpr int8_t NUMBER_4 = 4;

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest001 function start!");
    ASSERT_TRUE(root_);
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();

    statsCore->SaveForPower(root_);

    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_TRUE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest001 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest002 function start!");
    ASSERT_TRUE(root_);
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();

    statsCore->UpdateStatsEntity(root_);
    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_FALSE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest002 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest003 function start!");
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
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
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest023 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest024, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest024 function start!");
    ASSERT_TRUE(root_);
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

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest025, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest025 function start!");
    ASSERT_TRUE(root_);
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToObject(root_, "points", array);
    statsCore->UpdateStatsEntity(array);
    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_FALSE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest025 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest026, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest026 function start!");
    ASSERT_TRUE(root_);
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();
    cJSON* powerObj = cJSON_CreateObject();
    cJSON_AddStringToObject(powerObj, "10", "test");
    cJSON_AddItemToObject(root_, "Power", powerObj);
    statsCore->UpdateStatsEntity(root_);
    powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_TRUE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest026 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest027, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest027 function start!");
    ASSERT_TRUE(root_);
    std::shared_ptr<BatteryStatsCore> statsCore = std::make_shared<BatteryStatsCore>();
    cJSON* powerObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(powerObj, "asb", 123);
    cJSON_AddItemToObject(root_, "Power", powerObj);
    statsCore->UpdateStatsEntity(root_);
    powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    ASSERT_TRUE(powerObj && cJSON_IsObject(powerObj));
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest027 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest028, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest028 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "STATE", "test");
    std::string eventName = StatsHiSysEvent::DISCOVERY_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBrEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest028 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest029, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest029 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "STATE", "test");
    std::string eventName = StatsHiSysEvent::BR_SWITCH_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBrEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest029 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest030, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest030 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    cJSON_AddStringToObject(root_, "STATE", "test");
    std::string eventName = StatsHiSysEvent::BLE_SWITCH_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBluetoothBleEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest030 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest031, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest031 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "TYPE", "test");
    std::string eventName = StatsHiSysEvent::WIFI_CONNECTION;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWifiEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_WIFI_ON);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest031 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest032, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest032 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "STATE", "test");
    std::string eventName = StatsHiSysEvent::CALL_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest032 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest033, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest033 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "STATE", "test");
    std::string eventName = StatsHiSysEvent::DATA_CONNECTION_STATE;
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPhoneEvent(data, root_, eventName);
    EXPECT_EQ(data.type, StatsUtils::STATS_TYPE_PHONE_DATA);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest033 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest034, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest034 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "STATE", "test");
    cJSON_AddStringToObject(root_, "UID", "UID");
    cJSON_AddStringToObject(root_, "PID", "PID");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessFlashlightEvent(data, root_);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest034 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest035, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest035 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "STATE", "test");
    cJSON_AddStringToObject(root_, "UID", "UID");
    cJSON_AddStringToObject(root_, "PID", "PID");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEvent(data, root_);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest035 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest036, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest036 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_0);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEvent(data, root_);
    EXPECT_EQ(data.uid, NUMBER_UID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest036 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest037, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest037 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_1);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEvent(data, root_);
    EXPECT_EQ(data.uid, NUMBER_UID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest037 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest038, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest038 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_2);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEvent(data, root_);
    EXPECT_EQ(data.uid, NUMBER_UID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest038 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest039, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest039 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_4);
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_PID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEvent(data, root_);
    EXPECT_EQ(data.uid, NUMBER_UID);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest039 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest040, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest040 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddStringToObject(root_, "TYPE", "type");
    cJSON_AddNumberToObject(root_, "NAME", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "LOG_LEVEL", NUMBER_PID);
    cJSON_AddNumberToObject(root_, "TAG", NUMBER_PID);
    cJSON_AddNumberToObject(root_, "MESSAGE", NUMBER_PID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEventInternal(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest040 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest041, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest041 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "TYPE", NUMBER_UID);
    cJSON_AddStringToObject(root_, "NAME", "name");
    cJSON_AddStringToObject(root_, "LOG_LEVEL", "log_level");
    cJSON_AddStringToObject(root_, "TAG", "tag");
    cJSON_AddStringToObject(root_, "MESSAGE", "message");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWakelockEventInternal(data, root_);
    EXPECT_EQ(data.eventDataType, NUMBER_UID);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.eventDebugInfo, "INVALID TAG = tag MESSAGE = message");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest041 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest042, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest042 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "STATE", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "BRIGHTNESS", NUMBER_UID);
    cJSON_AddStringToObject(root_, "REASON", "reason");
    cJSON_AddStringToObject(root_, "name", "name");
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDisplayDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.eventDebugInfo, "INVALID Screen state = 100 Screen brightness = 100 Brightness reason = reason");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest042 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest043, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest043 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDisplayDebugInfo(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest043 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest044, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest044 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "NIT", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "RATIO", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "TYPE", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "LEVEL", NUMBER_UID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDisplayDebugInfoInternal(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.eventDebugInfo,
        "INVALID Brightness nit = 100 Ratio = 100 Ambient type = 100 Ambient brightness = 100");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest044 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest045, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest045 function start!");
    ASSERT_TRUE(root_);
    cJSON_AddNumberToObject(root_, "LEVEL", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "CHARGER", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "VOLTAGE", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "HEALTH", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "TEMPERATURE", NUMBER_UID);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBatteryEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.level, NUMBER_UID);
    EXPECT_EQ(data.eventDataExtra, NUMBER_UID);
    EXPECT_EQ(data.eventDebugInfo, "INVALID Voltage = 100 Health = 100 Temperature = 100");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest045 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest046, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest046 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessBatteryEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest046 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest047, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest047 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessThermalEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest047 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest048, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest048 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "name_", "name");
    cJSON_AddStringToObject(root_, "NAME", "NAME");
    cJSON_AddNumberToObject(root_, "LEVEL", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "TEMPERATURE", NUMBER_UID);
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessThermalEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.eventDebugInfo, "INVALIDEvent name = name Name = NAME Temperature = 100 Temperature level = 100");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest048 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest049, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest049 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "ACTION", "ACTION");
    cJSON_AddNumberToObject(root_, "VALUE", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "RATIO", NUMBER_UID);
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessThermalEventInternal(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest049 function end!");
}


HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest050, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest050 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessPowerWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest050 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest051, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest051 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessOthersWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest051 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest052, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest052 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "name_", "name");
    cJSON_AddStringToObject(root_, "NAME", "NAME");
    cJSON_AddNumberToObject(root_, "UID", NUMBER_UID);
    cJSON_AddNumberToObject(root_, "PID", NUMBER_UID);
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessOthersWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.uid, NUMBER_UID);
    EXPECT_EQ(data.pid, NUMBER_UID);
    EXPECT_EQ(data.eventDebugInfo, "INVALIDname: Bundle name = NAME");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest052 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest053, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest053 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "WORKID", "WORKID");
    cJSON_AddStringToObject(root_, "TRIGGER", "TRIGGER");
    cJSON_AddStringToObject(root_, "TYPE", "TYPE");
    cJSON_AddNumberToObject(root_, "INTERVAL", NUMBER_UID);
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessOthersWorkschedulerEventInternal(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    EXPECT_EQ(data.eventDebugInfo,
        "INVALID Work ID = WORKID Trigger conditions = TRIGGER Work type = TYPE Interval = 100");
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest053 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest054, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest054 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest054 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest055, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest055 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "name_", "WORK_ADD");
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest055 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest056, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest056 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    cJSON_AddStringToObject(root_, "name_", "WORK_ADD");
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessWorkschedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest056 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest057, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest057 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDistributedSchedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest057 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest058, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest058 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDistributedSchedulerEvent(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest058 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest059, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest059 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessDistributedSchedulerEventInternal(data, root_);
    ASSERT_FALSE(data.eventDebugInfo.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest059 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest060, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest060 function start!");
    ASSERT_TRUE(root_);
    StatsUtils::StatsData data;
    std::shared_ptr<BatteryStatsListener> listener = std::make_shared<BatteryStatsListener>();
    listener->ProcessAlarmEvent(data, root_);
    EXPECT_EQ(data.uid, INVALID_VALUE);
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest060 function end!");
}

HWTEST_F(StatsServiceConfigParseTest, StatsServiceConfigParseTest061, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest061 function start!");
    ASSERT_TRUE(root_);
    BatteryStatsParser batteryStatsParser;
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToObject(root_, "points", array);
    cJSON_AddStringToObject(array, "TYPE", "TYPE");
    batteryStatsParser.ParsingArray("type", array);
    ASSERT_FALSE(batteryStatsParser.averageVecMap_.empty());
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTest061 function end!");
}
} // namespace
