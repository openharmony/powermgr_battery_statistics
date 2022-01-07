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

#include "battery_stats_utils.h"

namespace OHOS {
namespace PowerMgr{

class BatteryStatsInfo : Parcelable {
public:
    enum BatteryStatsType {
        STATS_TYPE_INVALID = -10,
        STATS_TYPE_APP,
        STATS_TYPE_BLUETOOTH,
        STATS_TYPE_IDLE,
        STATS_TYPE_PHONE,
        STATS_TYPE_RADIO,
        STATS_TYPE_SCREEN,
        STATS_TYPE_USER,
        STATS_TYPE_WIFI
    };

    bool Marshalling(Parcel &parcel) const override;
    static std::shared_ptr<BatteryStatsInfo> Unmarshalling(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
    void SetUid(int32_t uid);
    void SetType(BatteryStatsType type);
    void SetPower(double power);
    int32_t GetUid();
    BatteryStatsType GetType();
    double GetPower();
    static BatteryStatsType CovertStatsType(std::string type);
private:
    int32_t uid_ = BatteryStatsUtils::INVALID_VALUE;
    BatteryStatsType type_ = STATS_TYPE_INVALID;
    double power_ = BatteryStatsUtils::DEFAULT_VALUE;
    static const std::map<std::string, BatteryStatsType> statsTypeMap_;
};
using BatteryStatsInfoList = std::list<std::shared_ptr<BatteryStatsInfo>>;
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_INFO_H