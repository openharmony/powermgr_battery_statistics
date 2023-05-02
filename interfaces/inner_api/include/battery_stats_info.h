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

#ifndef BATTERY_STATS_INFO_H
#define BATTERY_STATS_INFO_H

#include <list>
#include <memory>
#include <parcel.h>
#include <string>

#include "battery_stats_info.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsInfo : Parcelable {
public:
    enum ConsumptionType {
        CONSUMPTION_TYPE_INVALID = -17,
        CONSUMPTION_TYPE_APP,
        CONSUMPTION_TYPE_BLUETOOTH,
        CONSUMPTION_TYPE_IDLE,
        CONSUMPTION_TYPE_PHONE,
        CONSUMPTION_TYPE_RADIO,
        CONSUMPTION_TYPE_SCREEN,
        CONSUMPTION_TYPE_USER,
        CONSUMPTION_TYPE_WIFI,
        CONSUMPTION_TYPE_CAMERA,
        CONSUMPTION_TYPE_FLASHLIGHT,
        CONSUMPTION_TYPE_AUDIO,
        CONSUMPTION_TYPE_SENSOR,
        CONSUMPTION_TYPE_GNSS,
        CONSUMPTION_TYPE_CPU,
        CONSUMPTION_TYPE_WAKELOCK,
        CONSUMPTION_TYPE_ALARM
    };

    bool Marshalling(Parcel &parcel) const override;
    static std::shared_ptr<BatteryStatsInfo> Unmarshalling(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
    void SetUid(int32_t uid);
    void SetUserId(int32_t userId);
    void SetConsumptioType(ConsumptionType type);
    void SetPower(double power);
    int32_t GetUid();
    int32_t GetUserId();
    ConsumptionType GetConsumptionType();
    double GetPower();
    static std::string ConvertConsumptionType(ConsumptionType type);
private:
    int32_t uid_ = StatsUtils::INVALID_VALUE;
    int32_t userId_ = StatsUtils::INVALID_VALUE;
    ConsumptionType type_ = CONSUMPTION_TYPE_INVALID;
    double totalPowerMah_ = StatsUtils::DEFAULT_VALUE;
    static std::string ConvertTypeForPart(ConsumptionType type);
    static std::string ConvertTypeForApp(ConsumptionType type);
};
using BatteryStatsInfoList = std::list<std::shared_ptr<BatteryStatsInfo>>;
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_INFO_H