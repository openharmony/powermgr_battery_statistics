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

#include "entities/wifi_entity.h"

#include <cinttypes>

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

WifiEntity::WifiEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_WIFI;
}

void WifiEntity::Calculate(int32_t uid)
{
    if (uid > StatsUtils::INVALID_VALUE) {
        // Calculate Wifi scan and traffic power consumption caused by app
        CalculateWifiPowerForApp(uid);
    } else {
        // Calculate Wifi on and Wifi app power consumption caused by Wifi hardware
        CalculateWifiPower();
    }
}

void WifiEntity::CalculateWifiPower()
{
    // Calculate Wifi on power
    auto wifiOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    auto wifiOnTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_WIFI_ON);
    auto wifiOnPowerMah = wifiOnAverageMa * wifiOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto wifiUidPowerMah = GetWifiUidPower();

    // Sum Wifi power(mAms)
    auto wifiPowerMah = wifiOnPowerMah + wifiUidPowerMah;
    wifiPowerMah_ = wifiPowerMah;
    totalPowerMah_ += wifiPowerMah;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    statsInfo->SetPower(wifiPowerMah_);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGD(COMP_SVC, "Calculate wifi on power consumption: %{public}lfmAh,"   \
        "uid power consumption: %{public}lfmAh, total power consumption: %{public}lfmAh",
        wifiOnPowerMah, wifiUidPowerMah, wifiPowerMah);
}

void WifiEntity::CalculateWifiPowerForApp(int32_t uid)
{
    // Calculate Wifi scan power consumption
    auto wifiScanAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    auto wifiScanTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_SCAN);
    auto wifiScanPowerMams = wifiScanAverageMa * wifiScanTimeMs;

    // Calculate Wifi traffic power consumption
    // Calculate Wifi RX power consumption
    auto wifiRxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_RX);
    auto wifiRxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_RX);
    auto wifiRxPowerMams = wifiRxAverageMa * wifiRxTimeMs;

    // Calculate Wifi TX power consumption
    auto wifiTxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_TX);
    auto wifiTxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_TX);
    auto wifiTxPowerMams = wifiTxAverageMa * wifiTxTimeMs;

    auto wifiUidPowerMah =
        (wifiScanPowerMams + wifiRxPowerMams + wifiTxPowerMams) / StatsUtils::MS_IN_HOUR;

    STATS_HILOGD(COMP_SVC, "Calculate wifi scan power consumption: %{public}lfmAh," \
        "RX power consumption: %{public}lfmAh, wifi TX power consumption: %{public}lfmAh",
        wifiScanPowerMams / StatsUtils::MS_IN_HOUR,
        wifiRxPowerMams / StatsUtils::MS_IN_HOUR,
        wifiTxPowerMams / StatsUtils::MS_IN_HOUR);

    auto iter = appWifiPowerMap_.find(uid);
    if (iter != appWifiPowerMap_.end()) {
        iter->second = wifiUidPowerMah;
        STATS_HILOGD(COMP_SVC, "Update app wifi power consumption: %{public}lfmAh for uid: %{public}d",
            wifiUidPowerMah, uid);
    } else {
        appWifiPowerMap_.insert(std::pair<int32_t, double>(uid, wifiUidPowerMah));
        STATS_HILOGD(COMP_SVC, "Create app wifi power consumption: %{public}lfmAh for uid: %{public}d",
            wifiUidPowerMah, uid);
    }
}

int64_t WifiEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanTimerMap_.find(uid);
        if (scanIter != appWifiScanTimerMap_.end()) {
            time = scanIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got wifi scan time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No wifi scan timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxTimerMap_.find(uid);
        if (rxIter != appWifiRxTimerMap_.end()) {
            time = rxIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got wifi RX time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No wifi RX timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxTimerMap_.find(uid);
        if (txIter != appWifiTxTimerMap_.end()) {
            time = txIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got wifi TX time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No wifi TX timer related with uid: %{public}d found, return 0",
                uid);
        }
    }
    return time;
}

int64_t WifiEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    int64_t time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        if (wifiOnTimer_) {
            time = wifiOnTimer_->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Got wifi on time: %{public}" PRId64 "ms", time);
        } else {
            STATS_HILOGD(COMP_SVC, "Wifi has not been turned on yet, return 0");
        }
    }
    return time;
}

double WifiEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;

    if (uidOrUserId > StatsUtils::INVALID_VALUE) {
        auto iter = appWifiPowerMap_.find(uidOrUserId);
        if (iter != appWifiPowerMap_.end()) {
            power = iter->second;
            STATS_HILOGD(COMP_SVC, "Got app wifi power consumption: %{public}lfmAh for uid: %{public}d",
                power, uidOrUserId);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No app wifi power consumption related with uid: %{public}d found, return 0", uidOrUserId);
        }
    } else {
        power = wifiPowerMah_;
        STATS_HILOGD(COMP_SVC, "Got wifi power consumption: %{public}lfmAh", power);
    }
    return power;
}

double WifiEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        power = wifiPowerMah_;
        STATS_HILOGD(COMP_SVC, "Got wifi on power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanPowerMap_.find(uid);
        if (scanIter != appWifiScanPowerMap_.end()) {
            power = scanIter->second;
            STATS_HILOGD(COMP_SVC, "Got wifi scan power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No wifi scan power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxPowerMap_.find(uid);
        if (rxIter != appWifiRxPowerMap_.end()) {
            power = rxIter->second;
            STATS_HILOGD(COMP_SVC, "Got wifi RX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No wifi RX power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxPowerMap_.find(uid);
        if (txIter != appWifiTxPowerMap_.end()) {
            power = txIter->second;
            STATS_HILOGD(COMP_SVC, "Got wifi TX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No wifi TX power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    return power;
}

void WifiEntity::Reset()
{
    STATS_HILOGD(COMP_SVC, "Reset");
    // Reset Wifi on timer and power consumption
    wifiPowerMah_ = StatsUtils::DEFAULT_VALUE;
    if (wifiOnTimer_) {
        wifiOnTimer_->Reset();
    }

    // Reset app Wifi total power consumption
    for (auto& iter : appWifiPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi scan power consumption
    for (auto& iter : appWifiScanPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi RX power consumption
    for (auto& iter : appWifiRxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi TX power consumption
    for (auto& iter : appWifiTxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi scan timer
    for (auto& iter : appWifiScanTimerMap_) {
        iter.second->Reset();
    }

    // Reset Wifi RX timer
    for (auto& iter : appWifiRxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi TX timer
    for (auto& iter : appWifiTxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi RX counter
    for (auto& iter : appWifiRxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi TX counter
    for (auto& iter : appWifiTxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}

int64_t WifiEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    int64_t count = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxCounterMap_.find(uid);
        if (rxIter != appWifiRxCounterMap_.end()) {
            count = rxIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Got wifi RX traffic: %{public}" PRId64 "bytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No wifi RX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxCounterMap_.find(uid);
        if (txIter != appWifiTxCounterMap_.end()) {
            count = txIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Got wifi TX traffic: %{public}" PRId64 "bytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No wifi TX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    }
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> WifiEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanTimerMap_.find(uid);
        if (scanIter != appWifiScanTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got wifi scan timer for uid: %{public}d", uid);
            return scanIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi scan timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiScanTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiTxTimerMap_.find(uid);
        if (rxIter != appWifiRxTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got wifi RX timer for uid: %{public}d", uid);
            return rxIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi RX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiRxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxTimerMap_.find(uid);
        if (txIter != appWifiTxTimerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got wifi TX timer for uid: %{public}d", uid);
            return txIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi TX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiTxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            return timer;
        }
    } else {
        STATS_HILOGD(COMP_SVC, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::ActiveTimer> WifiEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        if (wifiOnTimer_ != nullptr) {
            STATS_HILOGD(COMP_SVC, "Got wifi on timer");
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi on timer");
            wifiOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
        }
        return wifiOnTimer_;
    } else {
        STATS_HILOGD(COMP_SVC, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::Counter> WifiEntity::GetOrCreateCounter(StatsUtils::StatsType statsType, int32_t uid)
{
    if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxCounterMap_.find(uid);
        if (rxIter != appWifiRxCounterMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got wifi RX counter for uid: %{public}d", uid);
            return rxIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi RX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appWifiRxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            return counter;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxCounterMap_.find(uid);
        if (txIter != appWifiTxCounterMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Got wifi TX counter for uid: %{public}d", uid);
            return txIter->second;
        } else {
            STATS_HILOGD(COMP_SVC, "Create wifi TX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appWifiTxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            return counter;
        }
    } else {
        STATS_HILOGD(COMP_SVC, "Create counter failed");
        return nullptr;
    }
}

double WifiEntity::GetWifiUidPower()
{
    double wifiUidPower = StatsUtils::DEFAULT_VALUE;
    auto bundleObj =
        DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGD(COMP_SVC, "failed to get bundle manager service, return default power");
        return wifiUidPower;
    }
    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    if (bmgr == nullptr) {
        STATS_HILOGD(COMP_SVC, "failed to get bundle manager proxy, return 0");
        return wifiUidPower;
    }

    std::string bundleName = "com.ohos.wifi";
    int32_t wifiUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto core = g_statsService->GetBatteryStatsCore();
    auto uidEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    if (uidEntity != nullptr) {
        wifiUidPower = uidEntity->GetEntityPowerMah(wifiUid);
    }
    STATS_HILOGD(COMP_SVC, "Get wifi uid power consumption: %{public}lfmAh", wifiUidPower);
    return wifiUidPower;
}

void WifiEntity::DumpInfo(std::string& result, int32_t uid)
{
    int64_t time = GetActiveTimeMs(StatsUtils::STATS_TYPE_WIFI_ON);
    result.append("Wifi dump:\n")
        .append("Wifi on time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS
