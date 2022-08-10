/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "entities/sensor_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

SensorEntity::SensorEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR;
}

int64_t SensorEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON: {
            auto gravityIter = gravityTimerMap_.find(uid);
            if (gravityIter != gravityTimerMap_.end()) {
                activeTimeMs = gravityIter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get gravity on time: %{public}" PRId64 "ms for uid: %{public}d",
                    activeTimeMs, uid);
                break;
            }
            STATS_HILOGD(COMP_SVC, "Didn't find related timer for uid: %{public}d, return 0", uid);
            break;
        }
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON: {
            auto proximityIter = proximityTimerMap_.find(uid);
            if (proximityIter != proximityTimerMap_.end()) {
                activeTimeMs = proximityIter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get proximity on time: %{public}" PRId64 "ms for uid: %{public}d",
                    activeTimeMs, uid);
                break;
            }
            STATS_HILOGD(COMP_SVC, "Didn't find related timer for uid: %{public}d, return 0", uid);
            break;
        }
        default:
            break;
    }
    return activeTimeMs;
}

double SensorEntity::CalculateGravity(int32_t uid)
{
    auto gravityOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    auto gravityOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);
    auto gravityOnPowerMah = gravityOnAverageMa * gravityOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto gravityIter = gravityPowerMap_.find(uid);
    if (gravityIter != gravityPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update gravity on power consumption: %{public}lfmAh for uid: %{public}d",
            gravityOnAverageMa, uid);
        gravityIter->second = gravityOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create gravity on power consumption: %{public}lfmAh for uid: %{public}d",
            gravityOnAverageMa, uid);
        gravityPowerMap_.insert(std::pair<int32_t, double>(uid, gravityOnPowerMah));
    }
    return gravityOnPowerMah;
}

double SensorEntity::CalculateProximity(int32_t uid)
{
    auto proximityOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    auto proximityOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON);
    auto proximityOnPowerMah = proximityOnAverageMa * proximityOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto proximityIter = proximityPowerMap_.find(uid);
    if (proximityIter != proximityPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update proximity on power consumption: %{public}lfmAh for uid: %{public}d",
            proximityOnAverageMa, uid);
        proximityIter->second = proximityOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create proximity on power consumption: %{public}lfmAh for uid: %{public}d",
            proximityOnAverageMa, uid);
        proximityPowerMap_.insert(std::pair<int32_t, double>(uid, proximityOnPowerMah));
    }
    return proximityOnPowerMah;
}

void SensorEntity::Calculate(int32_t uid)
{
    auto gravityOnPowerMah = CalculateGravity(uid);
    auto proximityOnPowerMah = CalculateProximity(uid);

    double sensorTotalPowerMah = gravityOnPowerMah + proximityOnPowerMah;
    auto sensorIter = sensorTotalPowerMap_.find(uid);
    if (sensorIter != sensorTotalPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update sensor total power consumption: %{public}lfmAh for uid: %{public}d",
            sensorTotalPowerMah, uid);
        sensorIter->second = sensorTotalPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create sensor total power consumption: %{public}lfmAh for uid: %{public}d",
            sensorTotalPowerMah, uid);
        sensorTotalPowerMap_.insert(std::pair<int32_t, double>(uid, sensorTotalPowerMah));
    }
}

double SensorEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = sensorTotalPowerMap_.find(uidOrUserId);
    if (iter != sensorTotalPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app sensor power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app sensor power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double SensorEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON) {
        auto gravityOnIter = gravityPowerMap_.find(uid);
        if (gravityOnIter != gravityPowerMap_.end()) {
            power = gravityOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get gravity on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No gravity on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON) {
        auto proximityOnIter = proximityPowerMap_.find(uid);
        if (proximityOnIter != proximityPowerMap_.end()) {
            power = proximityOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get proximity on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No proximity on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> SensorEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON: {
            auto gravityOnIter = gravityTimerMap_.find(uid);
            if (gravityOnIter != gravityTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get gravity on timer for uid: %{public}d", uid);
                timer = gravityOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create gravity on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> gravityTimer = std::make_shared<StatsHelper::ActiveTimer>();
            gravityTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, gravityTimer));
            timer = gravityTimer;
            break;
        }
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON: {
            auto proximityOnIter = proximityTimerMap_.find(uid);
            if (proximityOnIter != proximityTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get proximity on timer for uid: %{public}d", uid);
                timer = proximityOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create proximity on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> proximityTimer = std::make_shared<StatsHelper::ActiveTimer>();
            proximityTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, proximityTimer));
            timer = proximityTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void SensorEntity::Reset()
{
    // Reset app sensor total power consumption
    for (auto& iter : sensorTotalPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app gravity on total power consumption
    for (auto& iter : gravityPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app proximity on total power consumption
    for (auto& iter : proximityPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset gravity on timer
    for (auto& iter : gravityTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset proximity on timer
    for (auto& iter : proximityTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS