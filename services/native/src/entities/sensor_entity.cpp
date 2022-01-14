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

#include "entities/sensor_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

SensorEntity::SensorEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR;
}

long SensorEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON) {
        auto gravityIter = gravityTimerMap_.find(uid);
        if (gravityIter != gravityTimerMap_.end()) {
            activeTimeMs = gravityIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got gravity on time: %{public}ldms for uid: %{public}d", activeTimeMs,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON) {
        auto proximityIter = proximityTimerMap_.find(uid);
        if (proximityIter != proximityTimerMap_.end()) {
            activeTimeMs = proximityIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got proximity on time: %{public}ldms for uid: %{public}d", activeTimeMs,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void SensorEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    auto gravityOnAverageMa =
        statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_GRAVITY);
    auto gravityOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);
    auto gravityOnPowerMah = gravityOnAverageMa * gravityOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto gravityIter = gravityPowerMap_.find(uid);
    if (gravityIter != gravityPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update gravity on power consumption: %{public}lfmAh for uid: %{public}d",
            gravityOnAverageMa, uid);
        gravityIter->second = gravityOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create gravity on power consumption: %{public}lfmAh for uid: %{public}d",
            gravityOnAverageMa, uid);
        gravityPowerMap_.insert(std::pair<int32_t, double>(uid, gravityOnPowerMah));
    }

    auto proximityOnAverageMa =
        statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SENSOR_PROXIMITY);
    auto proximityOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON);
    auto proximityOnPowerMah = proximityOnAverageMa * proximityOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto proximityIter = proximityPowerMap_.find(uid);
    if (proximityIter != proximityPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update proximity on power consumption: %{public}lfmAh for uid: %{public}d",
            proximityOnAverageMa, uid);
        proximityIter->second = proximityOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create proximity on power consumption: %{public}lfmAh for uid: %{public}d",
            proximityOnAverageMa, uid);
        proximityPowerMap_.insert(std::pair<int32_t, double>(uid, proximityOnPowerMah));
    }

    double sensorTotalPowerMah = gravityOnPowerMah + proximityOnPowerMah;
    auto sensorIter = sensorTotalPowerMap_.find(uid);
    if (sensorIter != sensorTotalPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update sensor total power consumption: %{public}lfmAh for uid: %{public}d",
            sensorTotalPowerMah, uid);
        sensorIter->second = sensorTotalPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create sensor total power consumption: %{public}lfmAh for uid: %{public}d",
            sensorTotalPowerMah, uid);
        proximityPowerMap_.insert(std::pair<int32_t, double>(uid, sensorTotalPowerMah));
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity on average power: %{public}lfma", gravityOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity on time: %{public}ldms for uid: %{public}d", gravityOnTimeMs,
        uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity on power consumption: %{public}lfmAh for uid: %{public}d",
        gravityOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity on average power: %{public}lfma", proximityOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity on time: %{public}ldms for uid: %{public}d",
        proximityOnTimeMs, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity on power consumption: %{public}lfmAh for uid: %{public}d",
        proximityOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate sensor total power consumption: %{public}lfmAh for uid: %{public}d",
        sensorTotalPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double SensorEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = sensorTotalPowerMap_.find(uidOrUserId);
    if (iter != sensorTotalPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app sensor power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app sensor power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double SensorEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON) {
        auto gravityOnIter = gravityPowerMap_.find(uid);
        if (gravityOnIter != gravityPowerMap_.end()) {
            power = gravityOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got gravity on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No gravity on power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON) {
        auto proximityOnIter = proximityPowerMap_.find(uid);
        if (proximityOnIter != proximityPowerMap_.end()) {
            power = proximityOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got proximity on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No proximity on power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> SensorEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON) {
        auto gravityOnIter = gravityTimerMap_.find(uid);
        if (gravityOnIter != gravityTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got gravity on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return gravityOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create gravity on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            gravityTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON) {
        auto proximityOnIter = proximityTimerMap_.find(uid);
        if (proximityOnIter != proximityTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got proximity on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return proximityOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create proximity on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            proximityTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void SensorEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app sensor total power consumption
    for (auto &iter : sensorTotalPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app gravity on total power consumption
    for (auto &iter : gravityPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app proximity on total power consumption
    for (auto &iter : proximityPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset gravity on timer
    for (auto &iter : gravityTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset proximity on timer
    for (auto &iter : proximityTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS