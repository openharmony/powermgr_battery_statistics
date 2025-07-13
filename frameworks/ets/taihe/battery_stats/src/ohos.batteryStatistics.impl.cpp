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

#include "ohos.batteryStatistics.proj.hpp"
#include "ohos.batteryStatistics.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "battery_stats_client.h"
#include "stats_log.h"

using namespace taihe;
using namespace ohos::batteryStatistics;
using namespace OHOS::PowerMgr;

namespace {
std::map<StatsError, std::string> g_errorTable = {
    {StatsError::ERR_CONNECTION_FAIL,   "Failed to connect to the service."},
    {StatsError::ERR_PERMISSION_DENIED, "Permission is denied"             },
    {StatsError::ERR_SYSTEM_API_DENIED, "System permission is denied"      },
    {StatsError::ERR_PARAM_INVALID,     "Invalid input parameter."         }
};

using type_key_t = ohos::batteryStatistics::ConsumptionType::key_t;

ohos::batteryStatistics::ConsumptionType TaiheConsumptionTypeWrapper(
    OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType type)
{
    ohos::batteryStatistics::ConsumptionType taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_INVALID;
    switch (type) {
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_APP:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_APP;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_BLUETOOTH:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_BLUETOOTH;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_IDLE:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_IDLE;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_PHONE:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_PHONE;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_RADIO:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_RADIO;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_SCREEN:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_SCREEN;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_USER:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_USER;
            break;
        case OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_WIFI:
            taiheConsumptionType = type_key_t::CONSUMPTION_TYPE_WIFI;
            break;
        default:
            STATS_HILOGI(COMP_FWK, "Invalid return type:%{public}d", static_cast<int32_t>(type));
    }
    return taiheConsumptionType;
}

taihe::array<ohos::batteryStatistics::BatteryStatsInfo> GetBatteryStatsSync()
{
    BatteryStatsInfoList nativeStatsInfos = BatteryStatsClient::GetInstance().GetBatteryStats();
    StatsError code = BatteryStatsClient::GetInstance().GetLastError();
    std::vector<ohos::batteryStatistics::BatteryStatsInfo> tmpVector;
    if (code != StatsError::ERR_OK && g_errorTable.find(code) != g_errorTable.end()) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
        return taihe::array_view<ohos::batteryStatistics::BatteryStatsInfo>(tmpVector);
    }
    for (const auto& item : nativeStatsInfos) {
        auto type = TaiheConsumptionTypeWrapper(item->GetConsumptionType());
        if (type.get_key() == type_key_t::CONSUMPTION_TYPE_INVALID) {
            continue;
        }
        ohos::batteryStatistics::BatteryStatsInfo statsInfo = {
            .uid = item->GetUid(),
            .type = type,
            .power = item->GetPower()
        };
        tmpVector.push_back(statsInfo);
    }
    STATS_HILOGI(COMP_FWK, "GetBatteryStatsSync success, size %{public}zu", tmpVector.size());
    return taihe::array_view<ohos::batteryStatistics::BatteryStatsInfo>(tmpVector);
}

double GetAppPowerValue(int32_t uid)
{
    double appStatsMah = BatteryStatsClient::GetInstance().GetAppStatsMah(uid);
    StatsError code = BatteryStatsClient::GetInstance().GetLastError();
    STATS_HILOGI(COMP_FWK, "get app stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    if (code != StatsError::ERR_OK && g_errorTable.find(code) != g_errorTable.end()) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
    }
    return appStatsMah;
}

double GetAppPowerPercent(int32_t uid)
{
    double appStatsPercent = BatteryStatsClient::GetInstance().GetAppStatsPercent(uid);
    StatsError code = BatteryStatsClient::GetInstance().GetLastError();
    STATS_HILOGI(COMP_FWK, "get app stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    if (code != StatsError::ERR_OK && g_errorTable.find(code) != g_errorTable.end()) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
    }
    return appStatsPercent;
}

double GetHardwareUnitPowerValue(ohos::batteryStatistics::ConsumptionType type)
{
    OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType nativeType =
        OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType(static_cast<int32_t>(type));
    double partStatsMah = BatteryStatsClient::GetInstance().GetPartStatsMah(nativeType);
    StatsError code = BatteryStatsClient::GetInstance().GetLastError();
    STATS_HILOGI(COMP_FWK, "get part stats mah: %{public}lf for type: %{public}d", partStatsMah, nativeType);
    if (code != StatsError::ERR_OK && g_errorTable.find(code) != g_errorTable.end()) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
    }
    return partStatsMah;
}

double GetHardwareUnitPowerPercent(ohos::batteryStatistics::ConsumptionType type)
{
    OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType nativeType =
        OHOS::PowerMgr::BatteryStatsInfo::ConsumptionType(static_cast<int32_t>(type));
    double partStatsPercent = BatteryStatsClient::GetInstance().GetPartStatsPercent(nativeType);
    StatsError code = BatteryStatsClient::GetInstance().GetLastError();
    STATS_HILOGD(COMP_FWK, "get part stats percent: %{public}lf for type: %{public}d", partStatsPercent, nativeType);
    if (code != StatsError::ERR_OK && g_errorTable.find(code) != g_errorTable.end()) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
    }
    return partStatsPercent;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive
// NOLINTBEGIN
TH_EXPORT_CPP_API_GetBatteryStatsSync(GetBatteryStatsSync);
TH_EXPORT_CPP_API_GetAppPowerValue(GetAppPowerValue);
TH_EXPORT_CPP_API_GetAppPowerPercent(GetAppPowerPercent);
TH_EXPORT_CPP_API_GetHardwareUnitPowerValue(GetHardwareUnitPowerValue);
TH_EXPORT_CPP_API_GetHardwareUnitPowerPercent(GetHardwareUnitPowerPercent);
// NOLINTEND