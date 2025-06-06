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
class StatsServiceConfigParseTestThree : public Test {
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
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number)
{
    return nullptr;
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item)
{
    return false;
}

namespace {
constexpr size_t NUMBER_UID = 100;
HWTEST_F(StatsServiceConfigParseTestThree, StatsServiceConfigParseTestThree001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree001 function start!");
    ASSERT_TRUE(root_);
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);
    statsService->OnStart();
    auto statsCore = statsService->GetBatteryStatsCore();
    EXPECT_TRUE(statsCore != nullptr);
    statsCore->SaveForHardware(root_);
    cJSON* hardwareObj = cJSON_GetObjectItemCaseSensitive(root_, "Hardware");
    EXPECT_TRUE(hardwareObj != nullptr);
    statsCore->SaveForHardwareInternal(hardwareObj);
    cJSON* item = cJSON_GetObjectItemCaseSensitive(root_, "bluetooth_br_on");
    EXPECT_TRUE(item == nullptr);
    statsService->OnStop();
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree001 function end!");
}

HWTEST_F(StatsServiceConfigParseTestThree, StatsServiceConfigParseTestThree002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree002 function start!");
    ASSERT_TRUE(root_);
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);
    statsService->OnStart();
    auto statsCore = statsService->GetBatteryStatsCore();
    EXPECT_TRUE(statsCore != nullptr);
    statsCore->SaveForSoftwareCommon(root_, NUMBER_UID);
    cJSON* softwareObj = cJSON_GetObjectItemCaseSensitive(root_, "Software");
    EXPECT_TRUE(softwareObj != nullptr);
    cJSON* uidObj = cJSON_CreateObject();
    statsCore->SaveForSoftwareCommonInternal(uidObj, NUMBER_UID);
    cJSON* item = cJSON_GetObjectItemCaseSensitive(uidObj, "audio_on");
    EXPECT_TRUE(item == nullptr);
    cJSON_Delete(uidObj);
    statsService->OnStop();
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree002 function end!");
}

HWTEST_F(StatsServiceConfigParseTestThree, StatsServiceConfigParseTestThree003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree003 function start!");
    ASSERT_TRUE(root_);
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);
    statsService->OnStart();
    auto statsCore = statsService->GetBatteryStatsCore();
    EXPECT_TRUE(statsCore != nullptr);
    statsCore->SaveForSoftwareConnectivity(root_, NUMBER_UID);
    cJSON* softwareObj = cJSON_GetObjectItemCaseSensitive(root_, "Software");
    EXPECT_TRUE(softwareObj != nullptr);
    statsService->OnStop();
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree003 function end!");
}

HWTEST_F(StatsServiceConfigParseTestThree, StatsServiceConfigParseTestThree004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree004 function start!");
    ASSERT_TRUE(root_);
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);
    statsService->OnStart();
    auto statsCore = statsService->GetBatteryStatsCore();
    EXPECT_TRUE(statsCore != nullptr);
    statsCore->SaveForPower(root_);
    cJSON* powerObj = cJSON_GetObjectItemCaseSensitive(root_, "Power");
    EXPECT_TRUE(powerObj != nullptr);
    statsService->OnStop();
    STATS_HILOGI(LABEL_TEST, "StatsServiceConfigParseTestThree004 function end!");
}
} // namespace
