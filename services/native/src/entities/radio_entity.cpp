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

#include "entities/radio_entity.h"

#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

RadioEntity::RadioEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_RADIO;
}

void RadioEntity::Calculate(int32_t uid)
{
    if (uid > StatsUtils::INVALID_VALUE) {
        // Calculate Radio scan and traffic power consumption caused by app
        CalculateRadioPowerForApp(uid);
    } else {
        // Calculate Radio on and Radio app power consumption caused by Radio hardware
        CalculateRadioPower();
    }
}

void RadioEntity::CalculateRadioPower()
{
    STATS_HILOGD(COMP_SVC, "Enter");
    // Calculate Radio scan power
    auto radioScanAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_SCAN);
    auto radioScanTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_RADIO_SCAN);
    auto radioScanPowerMah = radioScanAverageMa * radioScanTimeMs / StatsUtils::MS_IN_HOUR;

    // Calculate Radio on power
    double radioOnPowerMah = StatsUtils::DEFAULT_VALUE;
    long radioOnTime = StatsUtils::DEFAULT_VALUE;
    for (int i = 0; i < StatsUtils::RADIO_SIGNAL_BIN; i++) {
        double radioOnLevelPowerMah = StatsUtils::DEFAULT_VALUE;
        auto radioOnAverageMa =
            g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ON, i);
        auto radioOnLevelTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_RADIO_ON, i);
        radioOnLevelPowerMah = radioOnAverageMa * radioOnLevelTimeMs / StatsUtils::MS_IN_HOUR;
        STATS_HILOGD(COMP_SVC, "Calculate radio on power consumption: %{public}lfmAh for level: %{public}d",
            radioOnLevelPowerMah, i);
        radioOnPowerMah += radioOnLevelPowerMah;
        radioOnTime += radioOnLevelTimeMs;
    }

    // Sum Radio power
    radioOnPowerMah_ = radioScanPowerMah + radioOnPowerMah;
    totalPowerMah_ += radioOnPowerMah_;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    statsInfo->SetPower(radioOnPowerMah_);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGD(COMP_SVC, "Calculate radio, scan power consumption: %{public}lfmAh,"   \
        "on power consumption: %{public}lfmAh, total power consumption: %{public}lfmAh",
        radioScanPowerMah, radioOnPowerMah, radioOnPowerMah_);
}

void RadioEntity::CalculateRadioPowerForApp(int32_t uid)
{
    // Calculate Radio traffic power consumption
    // Calculate Radio RX power consumption
    auto radioRxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_RX);
    auto radioRxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_RX);
    auto radioRxPowerMah = radioRxAverageMa * radioRxTimeMs / StatsUtils::MS_IN_HOUR;

    // Calculate Radio TX power consumption
    auto radioTxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_TX);
    auto radioTxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_TX);
    auto radioTxPowerMah = radioTxAverageMa * radioTxTimeMs / StatsUtils::MS_IN_HOUR;

    auto radioUidPowerMah = radioRxPowerMah + radioTxPowerMah;

    STATS_HILOGD(COMP_SVC,
        "Calculate radio RX power consumption: %{public}lfmAh, TX power consumption: %{public}lfmAh",
        radioRxPowerMah, radioTxPowerMah);
    auto iter = appRadioPowerMap_.find(uid);
    if (iter != appRadioPowerMap_.end()) {
        iter->second = radioUidPowerMah;
        STATS_HILOGD(COMP_SVC, "Update app radio power consumption: %{public}lfmAh for uid: %{public}d",
            radioUidPowerMah, uid);
    } else {
        appRadioPowerMap_.insert(std::pair<int32_t, double>(uid, radioUidPowerMah));
        STATS_HILOGD(COMP_SVC, "Create app radio power consumption: %{public}lfmAh for uid: %{public}d",
            radioUidPowerMah, uid);
    }
}

long RadioEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        auto rxIter = appRadioRxTimerMap_.find(uid);
        if (rxIter != appRadioRxTimerMap_.end()) {
            time = rxIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got radio RX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio RX timer related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        auto txIter = appRadioTxTimerMap_.find(uid);
        if (txIter != appRadioTxTimerMap_.end()) {
            time = txIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got radio TX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio TX timer related with uid: %{public}d found, return 0", uid);
        }
    }
    return time;
}

long RadioEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_RADIO_SCAN) {
        if (radioScanTimer_ != nullptr) {
            time = radioScanTimer_->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got radio scan time: %{public}ldms", time);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio scan timer found, return 0");
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_ON) {
        auto iter = radioOnTimerMap_.find(level);
        if (iter != radioOnTimerMap_.end() && iter->second != nullptr) {
            time = iter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got radio on time: %{public}ldms of signal level: %{public}d", time,
                level);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio on timer found, return 0");
        }
    }
    return time;
}

double RadioEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;

    if (uidOrUserId > StatsUtils::INVALID_VALUE) {
        auto iter = appRadioPowerMap_.find(uidOrUserId);
        if (iter != appRadioPowerMap_.end()) {
            power = iter->second;
            STATS_HILOGD(COMP_SVC, "Got app radio power consumption: %{public}lfmAh for uid: %{public}d",
                power, uidOrUserId);
        } else {
            STATS_HILOGE(COMP_SVC,
                "No app radio power consumption related with uid: %{public}d found, return 0", uidOrUserId);
        }
    } else {
        power = radioPowerMah_;
        STATS_HILOGD(COMP_SVC, "Got radio power consumption: %{public}lfmAh", power);
    }
    return power;
}

double RadioEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_RADIO_SCAN) {
        power = radioScanPowerMah_;
        STATS_HILOGD(COMP_SVC, "Got radio scan power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_ON) {
        power = radioOnPowerMah_;
        STATS_HILOGD(COMP_SVC, "Got radio on power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        auto rxIter = appRadioRxPowerMap_.find(uid);
        if (rxIter != appRadioRxPowerMap_.end()) {
            power = rxIter->second;
            STATS_HILOGD(COMP_SVC, "Got radio RX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(COMP_SVC,
                "No radio RX power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        auto txIter = appRadioTxPowerMap_.find(uid);
        if (txIter != appRadioTxPowerMap_.end()) {
            power = txIter->second;
            STATS_HILOGD(COMP_SVC, "Got radio TX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(COMP_SVC,
                "No radio TX power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    return power;
}

void RadioEntity::Reset()
{
    STATS_HILOGD(COMP_SVC, "Reset");
    // Reset Radio total power consumption
    radioPowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset Radio scan timer and power consumption
    if (radioScanTimer_ != nullptr) {
        radioScanTimer_->Reset();
    }

    // Reset Radio on timer and power consumption
    for (auto &iter : radioOnTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset app Radio total power consumption
    for (auto &iter : appRadioPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Radio RX power consumption
    for (auto &iter : appRadioRxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Radio TX power consumption
    for (auto &iter : appRadioTxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Radio RX timer
    for (auto &iter : appRadioRxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Radio TX timer
    for (auto &iter : appRadioTxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Radio RX counter
    for (auto &iter : appRadioRxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Radio TX counter
    for (auto &iter : appRadioTxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}

long RadioEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    long count = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        auto rxIter = appRadioRxCounterMap_.find(uid);
        if (rxIter != appRadioRxCounterMap_.end()) {
            count = rxIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Got radio RX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio RX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        auto txIter = appRadioTxCounterMap_.find(uid);
        if (txIter != appRadioTxCounterMap_.end()) {
            count = txIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Got radio TX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(COMP_SVC, "No radio TX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    }
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> RadioEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        auto rxIter = appRadioRxTimerMap_.find(uid);
        if (rxIter != appRadioRxTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got radio RX timer for uid: %{public}d", uid);
            return rxIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio RX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appRadioRxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        auto txIter = appRadioTxTimerMap_.find(uid);
        if (txIter != appRadioTxTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got radio TX timer for uid: %{public}d", uid);
            return txIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio TX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appRadioTxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            return timer;
        }
    } else {
        STATS_HILOGI(COMP_SVC, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::ActiveTimer> RadioEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    if (statsType == StatsUtils::STATS_TYPE_RADIO_SCAN) {
        if (radioScanTimer_ != nullptr) {
            STATS_HILOGI(COMP_SVC, "Got radio scan timer");
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio scan timer");
            radioScanTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
        }
        return radioScanTimer_;
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_ON) {
        auto onIter = radioOnTimerMap_.find(level);
        if (onIter != radioOnTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got radio on timer for level: %{public}d", level);
            return onIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio on timer for level: %{public}d", level);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            radioOnTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(level, timer));
            return timer;
        }
    } else {
        STATS_HILOGI(COMP_SVC, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::Counter> RadioEntity::GetOrCreateCounter(StatsUtils::StatsType statsType, int32_t uid)
{
    if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        auto rxIter = appRadioRxCounterMap_.find(uid);
        if (rxIter != appRadioRxCounterMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got radio RX counter for uid: %{public}d", uid);
            return rxIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio RX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appRadioRxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            return counter;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        auto txIter = appRadioTxCounterMap_.find(uid);
        if (txIter != appRadioTxCounterMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got radio TX counter for uid: %{public}d", uid);
            return txIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create radio TX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appRadioTxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            return counter;
        }
    } else {
        STATS_HILOGI(COMP_SVC, "Create counter failed");
        return nullptr;
    }
}

void RadioEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGD(COMP_SVC, "Reset");

    long onTime = StatsUtils::DEFAULT_VALUE;
    for (int i = 0; i < StatsUtils::RADIO_SIGNAL_BIN; i++) {
        long time = GetActiveTimeMs(StatsUtils::STATS_TYPE_RADIO_ON, i);
        onTime += time;
    }
    result.append("Radio dump:\n")
        .append("Radio on time:")
        .append(ToString(onTime))
        .append("ms")
        .append("\n");

    long scanTime = GetActiveTimeMs(StatsUtils::STATS_TYPE_RADIO_SCAN);
    result.append("Radio scan time: ")
        .append(ToString(scanTime))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS