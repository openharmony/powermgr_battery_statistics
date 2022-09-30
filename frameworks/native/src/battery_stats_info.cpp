/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "stats_common.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
bool BatteryStatsInfo::Marshalling(Parcel& parcel) const
{
    STATS_WRITE_PARCEL_WITH_RET(COMP_FWK, parcel, Int32, uid_, false);
    STATS_WRITE_PARCEL_WITH_RET(COMP_FWK, parcel, Int32, static_cast<int32_t>(type_), false);
    STATS_WRITE_PARCEL_WITH_RET(COMP_FWK, parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(COMP_FWK, "uid: %{public}d, type: %{public}d, power: %{public}lf", uid_, type_, totalPowerMah_);
    return true;
}

std::shared_ptr<BatteryStatsInfo> BatteryStatsInfo::Unmarshalling(Parcel& parcel)
{
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    if (statsInfo == nullptr) {
        STATS_HILOGW(COMP_FWK, "BatteryStatsInfo object creation failed");
        return statsInfo;
    }
    if (!statsInfo->ReadFromParcel(parcel)) {
        STATS_HILOGW(COMP_FWK, "Read from parcel failed");
        statsInfo = nullptr;
    }
    return statsInfo;
}

bool BatteryStatsInfo::ReadFromParcel(Parcel &parcel)
{
    STATS_READ_PARCEL_WITH_RET(COMP_FWK, parcel, Int32, uid_, false);
    int32_t type = static_cast<int32_t>(0);
    STATS_READ_PARCEL_WITH_RET(COMP_FWK, parcel, Int32, type, false);
    type_ = static_cast<ConsumptionType>(type);
    STATS_READ_PARCEL_WITH_RET(COMP_FWK, parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(COMP_FWK, "uid: %{public}d, type: %{public}d, power: %{public}lf", uid_, type_, totalPowerMah_);
    return true;
}

void BatteryStatsInfo::SetUid(int32_t uid)
{
    if (uid > StatsUtils::INVALID_VALUE) {
        uid_ = uid;
    }
    STATS_HILOGD(COMP_FWK, "Uid: %{public}d", uid);
}

void BatteryStatsInfo::SetUserId(int32_t userId)
{
    if (userId > StatsUtils::INVALID_VALUE) {
        userId_ = userId;
    }
    STATS_HILOGD(COMP_FWK, "UserId: %{public}d", userId);
}

void BatteryStatsInfo::SetConsumptioType(ConsumptionType type)
{
    type_ = type;
}

void BatteryStatsInfo::SetPower(double power)
{
    if (uid_ > StatsUtils::INVALID_VALUE) {
        STATS_HILOGD(COMP_FWK, "Set APP power: %{public}lfmAh for uid: %{public}d", totalPowerMah_, uid_);
    } else {
        STATS_HILOGD(COMP_FWK, "Set power: %{public}lfmAh for part: %{public}s", totalPowerMah_,
            ConvertConsumptionType(type_).c_str());
    }
    totalPowerMah_ = power;
}

int32_t BatteryStatsInfo::GetUid()
{
    return uid_;
}

int32_t BatteryStatsInfo::GetUserId()
{
    return userId_;
}

BatteryStatsInfo::ConsumptionType BatteryStatsInfo::GetConsumptionType()
{
    return type_;
}

double BatteryStatsInfo::GetPower()
{
    if (uid_ > StatsUtils::INVALID_VALUE) {
        STATS_HILOGD(COMP_FWK, "Get app power: %{public}lfmAh for uid: %{public}d", totalPowerMah_, uid_);
    } else {
        STATS_HILOGD(COMP_FWK, "Get power: %{public}lfmAh for part: %{public}s", totalPowerMah_,
            ConvertConsumptionType(type_).c_str());
    }
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
        case CONSUMPTION_TYPE_SCREEN:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_SCREEN);
            break;
        case CONSUMPTION_TYPE_WIFI:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_WIFI);
            break;
        default:
            break;
    }
    STATS_HILOGD(COMP_FWK, "Convert to %{public}s", result.c_str());
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
        case CONSUMPTION_TYPE_GNSS:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_GNSS);
            break;
        case CONSUMPTION_TYPE_CPU:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_CPU);
            break;
        case CONSUMPTION_TYPE_WAKELOCK:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_WAKELOCK);
            break;
        case CONSUMPTION_TYPE_ALARM:
            result = GET_VARIABLE_NAME(CONSUMPTION_TYPE_ALARM);
            break;
        default:
            break;
    }
    STATS_HILOGD(COMP_FWK, "Convert to %{public}s", result.c_str());
    return result;
}

std::string BatteryStatsInfo::ConvertConsumptionType(ConsumptionType type)
{
    std::string result = "";
    switch (type) {
        case CONSUMPTION_TYPE_BLUETOOTH:
        case CONSUMPTION_TYPE_IDLE:
        case CONSUMPTION_TYPE_PHONE:
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
        case CONSUMPTION_TYPE_GNSS:
        case CONSUMPTION_TYPE_CPU:
        case CONSUMPTION_TYPE_WAKELOCK:
        case CONSUMPTION_TYPE_ALARM:
            result = ConvertTypeForApp(type);
            break;
        default:
            STATS_HILOGD(COMP_FWK, "Conversion failed because the type was illegal, return empty string");
            break;
    }
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
