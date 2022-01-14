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

#include "entities/camera_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

CameraEntity::CameraEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA;
}

long CameraEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        auto iter = cameraTimerMap_.find(uid);
        if (iter != cameraTimerMap_.end()) {
            activeTimeMs = iter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got camera on time: %{public}ldms for uid: %{public}d", activeTimeMs,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void CameraEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto cameraOnAverageMa = statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    auto cameraOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_CAMERA_ON);
    auto cameraOnPowerMah = cameraOnAverageMa * cameraOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = cameraPowerMap_.find(uid);
    if (iter != cameraPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update camera on power consumption: %{public}lfmAh for uid: %{public}d",
            cameraOnAverageMa, uid);
        iter->second = cameraOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create camera on power consumption: %{public}lfmAh for uid: %{public}d",
            cameraOnAverageMa, uid);
        cameraPowerMap_.insert(std::pair<int32_t, double>(uid, cameraOnPowerMah));
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera on average power: %{public}lfma", cameraOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera on time: %{public}ldms for uid: %{public}d", cameraOnTimeMs,
        uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera on power consumption: %{public}lfmAh for uid: %{public}d",
        cameraOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double CameraEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = cameraPowerMap_.find(uidOrUserId);
    if (iter != cameraPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app camera power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app camera power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double CameraEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        auto cameraOnIter = cameraPowerMap_.find(uid);
        if (cameraOnIter != cameraPowerMap_.end()) {
            power = cameraOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got camera on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No camera on power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> CameraEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        auto cameraOnIter = cameraTimerMap_.find(uid);
        if (cameraOnIter != cameraTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got camera on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return cameraOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create camera on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            cameraTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void CameraEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Camera on total power consumption
    for (auto &iter : cameraPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Camera on timer
    for (auto &iter : cameraTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS