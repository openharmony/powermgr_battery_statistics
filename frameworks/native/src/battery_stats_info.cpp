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
#include "stats_hilog_wrapper.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
const std::map<std::string, BatteryStatsInfo::BatteryStatsType> BatteryStatsInfo::statsTypeMap_ = {
    {BatteryStatsUtils::TYPE_BLUETOOTH_ON, BatteryStatsInfo::STATS_TYPE_BLUETOOTH},
    {BatteryStatsUtils::TYPE_WIFI_ON, BatteryStatsInfo::STATS_TYPE_WIFI},
    {BatteryStatsUtils::TYPE_RADIO_ON, BatteryStatsInfo::STATS_TYPE_RADIO},
    {BatteryStatsUtils::TYPE_RADIO_SCAN, BatteryStatsInfo::STATS_TYPE_RADIO},
    {BatteryStatsUtils::TYPE_RADIO_ACTIVE, BatteryStatsInfo::STATS_TYPE_PHONE},
    {BatteryStatsUtils::TYPE_SCREEN_ON, BatteryStatsInfo::STATS_TYPE_SCREEN},
    {BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS, BatteryStatsInfo::STATS_TYPE_SCREEN},
    {BatteryStatsUtils::TYPE_CPU_IDLE, BatteryStatsInfo::STATS_TYPE_IDLE}
};

bool BatteryStatsInfo::Marshalling(Parcel& parcel) const
{
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, uid_, false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, static_cast<int32_t>(type_), false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Double, power_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", power_);
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
    type_ = static_cast<BatteryStatsType>(type);
    STATS_READ_PARCEL_WITH_RET(parcel, Double, power_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", power_);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return true;
}

void BatteryStatsInfo::SetUid(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Set uid: %{public}d", uid);
    uid_ = uid;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetType(BatteryStatsType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Set type: %{public}d", type);
    type_ = type;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetPower(double power)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (uid_ > BatteryStatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lf to type: %{public}d for uid: %{public}d",
            power, type_, uid_);
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lf to type: %{public}d", power, type_);
    }
    power_ = power;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

int32_t BatteryStatsInfo::GetUid()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got uid: %{public}d", uid_);
    return uid_;
}

BatteryStatsInfo::BatteryStatsType BatteryStatsInfo::GetType()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got type: %{public}d", type_);
    return type_;
}

double BatteryStatsInfo::GetPower()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got power: %{public}lf", power_);
    return power_;
}

BatteryStatsInfo::BatteryStatsType BatteryStatsInfo::CovertStatsType(std::string type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    BatteryStatsType statsType = STATS_TYPE_INVALID;
    auto iter = statsTypeMap_.find(type);
    if (iter != statsTypeMap_.end()) {
        statsType = iter->second;
        STATS_HILOGD(STATS_MODULE_INNERKIT, "Covert %{public}s to %{public}d", type.c_str(), statsType);
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Covert failed for %{public}s, return %{public}d", type.c_str(), statsType);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return statsType;
}
} // namespace PowerMgr
} // namespace OHOS
