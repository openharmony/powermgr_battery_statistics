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

#include "entities/audio_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

AudioEntity::AudioEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO;
}

long AudioEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_AUDIO_ON) {
        auto iter = audioTimerMap_.find(uid);
        if (iter != audioTimerMap_.end()) {
            activeTimeMs = iter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got audio on time: %{public}ldms for uid: %{public}d", activeTimeMs,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void AudioEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto audioOnAverageMa = statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    auto audioOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_AUDIO_ON);
    auto audioOnPowerMah = audioOnAverageMa * audioOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = audioPowerMap_.find(uid);
    if (iter != audioPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update audio on power consumption: %{public}lfmAh for uid: %{public}d",
            audioOnAverageMa, uid);
        iter->second = audioOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create audio on power consumption: %{public}lfmAh for uid: %{public}d",
            audioOnAverageMa, uid);
        audioPowerMap_.insert(std::pair<int32_t, double>(uid, audioOnPowerMah));
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate audio on average power: %{public}lfma", audioOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate audio on time: %{public}ldms for uid: %{public}d", audioOnTimeMs,
        uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate audio on power consumption: %{public}lfmAh for uid: %{public}d",
        audioOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double AudioEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = audioPowerMap_.find(uidOrUserId);
    if (iter != audioPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app audio power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app audio power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double AudioEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_AUDIO_ON) {
        auto audioOnIter = audioPowerMap_.find(uid);
        if (audioOnIter != audioPowerMap_.end()) {
            power = audioOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got audio on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No audio on power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> AudioEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_AUDIO_ON) {
        auto audioOnIter = audioTimerMap_.find(uid);
        if (audioOnIter != audioTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got audio on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return audioOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create audio on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            audioTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void AudioEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Audio on total power consumption
    for (auto &iter : audioPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Audio on timer
    for (auto &iter : audioTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS