/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "battery_stats_info.h"

#include <parcel.h>
#include <string_ex.h>

#include "stats_common.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
bool BatteryStatsInfo::Marshalling(Parcel& parcel) const
{
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, uid_, false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, static_cast<int32_t>(type_), false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", totalPowerMah_);
    return true;
}

std::shared_ptr<BatteryStatsInfo> BatteryStatsInfo::Unmarshalling(Parcel& parcel)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    if (statsInfo == nullptr) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "BatteryStatsInfo object creating is failed.");
        statsInfo = nullptr;
    }
    if (!statsInfo->ReadFromParcel(parcel)) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "ReadFromParcel failed.");
        statsInfo = nullptr;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return statsInfo;
}

bool BatteryStatsInfo::ReadFromParcel(Parcel &parcel)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_READ_PARCEL_WITH_RET(parcel, Int32, uid_, false);
    int32_t type = static_cast<int32_t>(0);
    STATS_READ_PARCEL_WITH_RET(parcel, Int32, type, false);
    type_ = static_cast<ConsumptionType>(type);
    STATS_READ_PARCEL_WITH_RET(parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", totalPowerMah_);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return true;
}

void BatteryStatsInfo::SetUid(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (uid > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set uid: %{public}d", uid);
        uid_ = uid;
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Got illegal uid: %{public}d, ignore", uid);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetUserId(int32_t userId)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (userId > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set uid: %{public}d", userId);
        userId_ = userId;
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Got illegal user id: %{public}d, ignore", userId);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetConsumptioType(ConsumptionType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Set type: %{public}d", type);
    type_ = type;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetPower(double power)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (uid_ > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set APP power: %{public}lfmAh for uid: %{public}d", totalPowerMah_, uid_);
    } else {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set power: %{public}lfmAh for part: %{public}s", totalPowerMah_,
            ConvertConsumptionType(type_).c_str());
    }
    totalPowerMah_ = power;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

int32_t BatteryStatsInfo::GetUid()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got uid: %{public}d", uid_);
    return uid_;
}

int32_t BatteryStatsInfo::GetUserId()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got user id: %{public}d", userId_);
    return userId_;
}

BatteryStatsInfo::ConsumptionType BatteryStatsInfo::GetConsumptionType()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got type: %{public}d", type_);
    return type_;
}

double BatteryStatsInfo::GetPower()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (uid_ > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Got APP power: %{public}lfmAh for uid: %{public}d", totalPowerMah_, uid_);
    } else {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Got power: %{public}lfmAh for part: %{public}s", totalPowerMah_,
            ConvertConsumptionType(type_).c_str());
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return totalPowerMah_;
}

std::string BatteryStatsInfo::ConvertTypeForPart(ConsumptionType type)
{
    std::string result = "";
    switch (type) {
        case CONSUMPTION_TYPE_BLUETOOTH:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_BLUETOOTH);
            break;
        case CONSUMPTION_TYPE_IDLE:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_IDLE);
            break;
        case CONSUMPTION_TYPE_PHONE:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_PHONE);
            break;
        case CONSUMPTION_TYPE_RADIO:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_RADIO);
            break;
        case CONSUMPTION_TYPE_SCREEN:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_SCREEN);
            break;
        case CONSUMPTION_TYPE_WIFI:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_WIFI);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
    return result;
}

std::string BatteryStatsInfo::ConvertTypeForApp(ConsumptionType type)
{
    std::string result = "";
    switch (type) {
        case CONSUMPTION_TYPE_USER:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_USER);
            break;
        case CONSUMPTION_TYPE_APP:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_APP);
            break;
        case CONSUMPTION_TYPE_CAMERA:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_CAMERA);
            break;
        case CONSUMPTION_TYPE_FLASHLIGHT:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_FLASHLIGHT);
            break;
        case CONSUMPTION_TYPE_AUDIO:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_AUDIO);
            break;
        case CONSUMPTION_TYPE_SENSOR:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_SENSOR);
            break;
        case CONSUMPTION_TYPE_GPS:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_GPS);
            break;
        case CONSUMPTION_TYPE_CPU:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_CPU);
            break;
        case CONSUMPTION_TYPE_WAKELOCK:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_WAKELOCK);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
    return result;
}

std::string BatteryStatsInfo::ConvertConsumptionType(ConsumptionType type)
{
    std::string result = "";
    switch (type) {
        case CONSUMPTION_TYPE_BLUETOOTH:
        case CONSUMPTION_TYPE_IDLE:
        case CONSUMPTION_TYPE_PHONE:
        case CONSUMPTION_TYPE_RADIO:
        case CONSUMPTION_TYPE_SCREEN:
        case CONSUMPTION_TYPE_WIFI:
            result = ConvertTypeForPart(type);
            break;
        case CONSUMPTION_TYPE_USER:
        case CONSUMPTION_TYPE_APP:
        case CONSUMPTION_TYPE_CAMERA:
        case CONSUMPTION_TYPE_FLASHLIGHT:
        case CONSUMPTION_TYPE_AUDIO:
        case CONSUMPTION_TYPE_SENSOR:
        case CONSUMPTION_TYPE_GPS:
        case CONSUMPTION_TYPE_CPU:
        case CONSUMPTION_TYPE_WAKELOCK:
            result = ConvertTypeForApp(type);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Illegal ConsumptionType got");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
