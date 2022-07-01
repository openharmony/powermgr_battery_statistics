/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef SENSOR_ENTITY_H
#define SENSOR_ENTITY_H

#include <map>

#include "entities/battery_stats_entity.h"
#include "stats_helper.h"

namespace OHOS {
namespace PowerMgr {
class SensorEntity : public BatteryStatsEntity {
public:
    SensorEntity();
    ~SensorEntity() = default;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    int64_t GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    double GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::ActiveTimer> GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    void Reset() override;
private:
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> gravityTimerMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> proximityTimerMap_;
    std::map<int32_t, double> sensorTotalPowerMap_;
    std::map<int32_t, double> gravityPowerMap_;
    std::map<int32_t, double> proximityPowerMap_;
    double CalculateGravity(int32_t uid);
    double CalculateProximity(int32_t uid);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // SENSOR_ENTITY_H