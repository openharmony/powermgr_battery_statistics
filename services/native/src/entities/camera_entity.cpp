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

#include "entities/camera_entity.h"

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

CameraEntity::CameraEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA;
}

int64_t CameraEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        for (auto& cameraIter : cameraTimerMap_) {
            auto uidIter = cameraIter.second.find(uid);
            if (uidIter != cameraIter.second.end()) {
                activeTimeMs += uidIter->second->GetRunningTimeMs();
            } else {
                STATS_HILOGD(COMP_SVC, "Didn't find related timer for uid: %{public}d in camera id: %{private}s",
                    uid, cameraIter.first.c_str());
            }
        }
    }
    return activeTimeMs;
}

void CameraEntity::Calculate(int32_t uid)
{
    auto cameraOnAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CAMERA_ON);
    auto cameraOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_CAMERA_ON);
    auto cameraOnPowerMah = cameraOnAverageMa * cameraOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = cameraPowerMap_.find(uid);
    if (iter != cameraPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update camera on power consumption: %{public}lfmAh for uid: %{public}d",
            cameraOnAverageMa, uid);
        iter->second = cameraOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create camera on power consumption: %{public}lfmAh for uid: %{public}d",
            cameraOnAverageMa, uid);
        cameraPowerMap_.insert(std::pair<int32_t, double>(uid, cameraOnPowerMah));
    }
}

double CameraEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = cameraPowerMap_.find(uidOrUserId);
    if (iter != cameraPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app camera power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app camera power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double CameraEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        auto cameraOnIter = cameraPowerMap_.find(uid);
        if (cameraOnIter != cameraPowerMap_.end()) {
            power = cameraOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get camera on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No camera on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> CameraEntity::GetOrCreateTimer(const std::string& deviceId, int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_CAMERA_ON: {
            auto cameraIter = cameraTimerMap_.find(deviceId);
            if (cameraIter != cameraTimerMap_.end()) {
                auto uidIter = cameraIter->second.find(uid);
                if (uidIter != cameraIter->second.end()) {
                    timer = uidIter->second;
                    break;
                }
                std::shared_ptr<StatsHelper::ActiveTimer> cmrTimer = std::make_shared<StatsHelper::ActiveTimer>();
                cameraIter->second.insert(
                    std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, cmrTimer));
                timer = cmrTimer;
                break;
            }
            std::shared_ptr<StatsHelper::ActiveTimer> cmrTimer = std::make_shared<StatsHelper::ActiveTimer>();
            std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> uidTimerMap;
            uidTimerMap.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, cmrTimer));
            cameraTimerMap_.insert(std::pair<std::string, std::map<int32_t,
                std::shared_ptr<StatsHelper::ActiveTimer>>>(deviceId, uidTimerMap));
            timer = cmrTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void CameraEntity::Reset()
{
    // Reset app Camera on total power consumption
    for (auto& iter : cameraPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Camera on timer
    for (auto& cameraIter : cameraTimerMap_) {
        for (auto& uidIter : cameraIter.second) {
            if (uidIter.second) {
                uidIter.second->Reset();
            }
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS