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

#include <map>
#include <cinttypes>
#include <singleton.h>
#include <gtest/gtest.h>

#include "ohos.batteryStatistics.proj.hpp"
#include "ohos.batteryStatistics.impl.hpp"
#include "ohos.batteryStatistics.user.hpp"
#include "taihe/runtime.hpp"
#include "battery_stats_client.h"
#include "battery_stats_info.h"
#include "battery_stats_errors.h"
#include "stats_log.h"

using namespace taihe;
using namespace ohos::batteryStatistics;
using namespace OHOS::PowerMgr;

using namespace testing;
using namespace testing::ext;

namespace {
BatteryStatsInfoList g_list{};
StatsError g_error = StatsError::ERR_OK;
}

namespace taihe {
void set_business_error(int32_t err_code, taihe::string_view msg)
{
    (void)err_code;
    (void)msg;
}
}

namespace OHOS::PowerMgr {
BatteryStatsInfoList BatteryStatsClient::GetBatteryStats()
{
    STATS_HILOGI(LABEL_TEST, "enter mock GetBatteryStats");
    return g_list;
}

double BatteryStatsClient::GetAppStatsMah(const int32_t& uid)
{
    return 0.0;
}

double BatteryStatsClient::GetAppStatsPercent(const int32_t& uid)
{
    return 0.0;
}

double BatteryStatsClient::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& uid)
{
    return 0.0;
}

double BatteryStatsClient::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& uid)
{
    return 0.0;
}

StatsError BatteryStatsClient::GetLastError()
{
    STATS_HILOGI(LABEL_TEST, "enter mock GetLastError, %{public}d", static_cast<int32_t>(g_error));
    return g_error;
}
} // OHOS::PowerMgr

namespace {
class StatsTaiheNativeTest : public ::testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

static bool IsEqual(double a, double b, double epsilon = 1e-6)
{
    return std::abs(a - b) < epsilon;
}

/**
 * @tc.name: StatsTaiheNativeTest_001
 * @tc.desc: test stats taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(StatsTaiheNativeTest, StatsTaiheNativeTest_001, TestSize.Level1)
{
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_001 start");
    std::shared_ptr<OHOS::PowerMgr::BatteryStatsInfo> info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    g_list.push_back(info);
    auto result = GetBatteryStatsSync();
    EXPECT_TRUE(result.empty());

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_IDLE);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_USER);
    g_list.push_back(info);

    info = std::make_shared<OHOS::PowerMgr::BatteryStatsInfo>();
    info->SetConsumptioType(OHOS::PowerMgr::BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    g_list.push_back(info);

    result = GetBatteryStatsSync();
    EXPECT_FALSE(result.empty());

    g_error = StatsError::ERR_PARAM_INVALID;
    result = GetBatteryStatsSync();
    EXPECT_TRUE(result.empty());
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_001 end");
}

/**
 * @tc.name: StatsTaiheNativeTest_002
 * @tc.desc: test stats taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(StatsTaiheNativeTest, StatsTaiheNativeTest_002, TestSize.Level1)
{
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_002 start");
    int32_t uid = -1;
    g_error = StatsError::ERR_OK;
    auto result = GetAppPowerValue(uid);
    EXPECT_TRUE(IsEqual(result, 0.0));

    uid = 10021;
    g_error = StatsError::ERR_SYSTEM_API_DENIED;
    result = GetAppPowerValue(uid);
    EXPECT_TRUE(IsEqual(result, 0.0));
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_002 end");
}

/**
 * @tc.name: StatsTaiheNativeTest_003
 * @tc.desc: test stats taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(StatsTaiheNativeTest, StatsTaiheNativeTest_003, TestSize.Level1)
{
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_003 start");
    int32_t uid = -1;
    g_error = StatsError::ERR_OK;
    auto result = GetAppPowerPercent(uid);
    EXPECT_TRUE(IsEqual(result, 0.0));

    uid = 10021;
    g_error = StatsError::ERR_SYSTEM_API_DENIED;
    result = GetAppPowerPercent(uid);
    EXPECT_TRUE(IsEqual(result, 0.0));
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_003 end");
}

/**
 * @tc.name: StatsTaiheNativeTest_004
 * @tc.desc: test stats taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(StatsTaiheNativeTest, StatsTaiheNativeTest_004, TestSize.Level1)
{
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_004 start");
    ohos::batteryStatistics::ConsumptionType type =
        ohos::batteryStatistics::ConsumptionType::key_t::CONSUMPTION_TYPE_APP;
    g_error = StatsError::ERR_OK;
    auto result = GetHardwareUnitPowerValue(type);
    EXPECT_TRUE(IsEqual(result, 0.0));

    type = ohos::batteryStatistics::ConsumptionType::key_t::CONSUMPTION_TYPE_INVALID;
    g_error = StatsError::ERR_SYSTEM_API_DENIED;
    result = GetHardwareUnitPowerValue(type);
    EXPECT_TRUE(IsEqual(result, 0.0));
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_004 end");
}

/**
 * @tc.name: StatsTaiheNativeTest_005
 * @tc.desc: test stats taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(StatsTaiheNativeTest, StatsTaiheNativeTest_005, TestSize.Level1)
{
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_005 start");
    ohos::batteryStatistics::ConsumptionType type =
        ohos::batteryStatistics::ConsumptionType::key_t::CONSUMPTION_TYPE_APP;
    g_error = StatsError::ERR_OK;
    auto result = GetHardwareUnitPowerPercent(type);
    EXPECT_TRUE(IsEqual(result, 0.0));

    type = ohos::batteryStatistics::ConsumptionType::key_t::CONSUMPTION_TYPE_INVALID;
    g_error = StatsError::ERR_SYSTEM_API_DENIED;
    result = GetHardwareUnitPowerPercent(type);
    EXPECT_TRUE(IsEqual(result, 0.0));
    STATS_HILOGI(LABEL_TEST, "StatsTaiheNativeTest_005 end");
}
}
