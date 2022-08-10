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

#include "entities/audio_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

AudioEntity::AudioEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO;
}

int64_t AudioEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_AUDIO_ON: {
            auto iter = audioTimerMap_.find(uid);
            if (iter != audioTimerMap_.end()) {
                activeTimeMs = iter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get audio on time: %{public}" PRId64 "ms for uid: %{public}d",
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

void AudioEntity::Calculate(int32_t uid)
{
    auto audioOnAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_AUDIO_ON);
    auto audioOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_AUDIO_ON);
    auto audioOnPowerMah = audioOnAverageMa * audioOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = audioPowerMap_.find(uid);
    if (iter != audioPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update audio on power consumption: %{public}lfmAh for uid: %{public}d",
            audioOnAverageMa, uid);
        iter->second = audioOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create audio on power consumption: %{public}lfmAh for uid: %{public}d",
            audioOnPowerMah, uid);
        audioPowerMap_.insert(std::pair<int32_t, double>(uid, audioOnPowerMah));
    }
}

double AudioEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = audioPowerMap_.find(uidOrUserId);
    if (iter != audioPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app audio power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app audio power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double AudioEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_AUDIO_ON) {
        auto audioOnIter = audioPowerMap_.find(uid);
        if (audioOnIter != audioPowerMap_.end()) {
            power = audioOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get audio on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No audio on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> AudioEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_AUDIO_ON: {
            auto audioOnIter = audioTimerMap_.find(uid);
            if (audioOnIter != audioTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get audio on timer for uid: %{public}d", uid);
                timer = audioOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create audio on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> audioTimer = std::make_shared<StatsHelper::ActiveTimer>();
            audioTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, audioTimer));
            timer = audioTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void AudioEntity::Reset()
{
    // Reset app Audio on total power consumption
    for (auto& iter : audioPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Audio on timer
    for (auto& iter : audioTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS