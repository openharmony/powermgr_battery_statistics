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

#include "entities/wifi_entity.h"

#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "system_ability_definition.h"
#include "sys_mgr_client.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (uid > StatsUtils::INVALID_VALUE) {
        // Calculate Wifi scan and traffic power consumption caused by app
        CalculateWifiPowerForApp(uid);
    } else {
        // Calculate Wifi on and Wifi app power consumption caused by Wifi hardware
        CalculateWifiPower();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void WifiEntity::CalculateWifiPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on time: %{public}ldms", wifiOnTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on power average: %{public}lfma", wifiOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on power consumption: %{public}lfmAh", wifiOnPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi uid power consumption: %{public}lfmAh", wifiUidPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi total power consumption: %{public}lfmAh", wifiPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void WifiEntity::CalculateWifiPowerForApp(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan time: %{public}ldms", wifiScanTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan power average: %{public}lfma", wifiScanAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan power consumption: %{public}lfmAh",
        wifiScanPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi RX time: %{public}ldms", wifiRxTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi RX power average: %{public}lfma", wifiRxAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi RX power consumption: %{public}lfmAh",
        wifiRxPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi TX time: %{public}ldms", wifiTxTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi TX power average: %{public}lfma", wifiTxAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi TX power consumption: %{public}lfmAh",
        wifiTxPowerMams / StatsUtils::MS_IN_HOUR);
    auto iter = appWifiPowerMap_.find(uid);
    if (iter != appWifiPowerMap_.end()) {
        iter->second = wifiUidPowerMah;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update app wifi power consumption: %{public}lfmAh for uid: %{public}d",
            wifiUidPowerMah, uid);
    } else {
        appWifiPowerMap_.insert(std::pair<int32_t, double>(uid, wifiUidPowerMah));
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create app wifi power consumption: %{public}lfmAh for uid: %{public}d",
            wifiUidPowerMah, uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long WifiEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanTimerMap_.find(uid);
        if (scanIter != appWifiScanTimerMap_.end()) {
            time = scanIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi scan time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No wifi scan timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxTimerMap_.find(uid);
        if (rxIter != appWifiRxTimerMap_.end()) {
            time = rxIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi RX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No wifi RX timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxTimerMap_.find(uid);
        if (txIter != appWifiTxTimerMap_.end()) {
            time = txIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi TX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No wifi TX timer related with uid: %{public}d found, return 0",
                uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

long WifiEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        if (wifiOnTimer_) {
            time = wifiOnTimer_->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi on time: %{public}ldms", time);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Wifi has not been turned on yet, return 0");
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

double WifiEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;

    if (uidOrUserId > StatsUtils::INVALID_VALUE) {
        auto iter = appWifiPowerMap_.find(uidOrUserId);
        if (iter != appWifiPowerMap_.end()) {
            power = iter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got app wifi power consumption: %{public}lfmAh for uid: %{public}d",
                power, uidOrUserId);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No app wifi power consumption related with uid: %{public}d found, return 0", uidOrUserId);
        }
    } else {
        power = wifiPowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi power consumption: %{public}lfmAh", power);
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double WifiEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        power = wifiPowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi on power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanPowerMap_.find(uid);
        if (scanIter != appWifiScanPowerMap_.end()) {
            power = scanIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi scan power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No wifi scan power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxPowerMap_.find(uid);
        if (rxIter != appWifiRxPowerMap_.end()) {
            power = rxIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi RX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No wifi RX power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxPowerMap_.find(uid);
        if (txIter != appWifiTxPowerMap_.end()) {
            power = txIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi TX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No wifi TX power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void WifiEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset Wifi on timer and power consumption
    wifiPowerMah_ = StatsUtils::DEFAULT_VALUE;
    if (wifiOnTimer_) {
        wifiOnTimer_->Reset();
    }

    // Reset app Wifi total power consumption
    for (auto &iter : appWifiPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi scan power consumption
    for (auto &iter : appWifiScanPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi RX power consumption
    for (auto &iter : appWifiRxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi TX power consumption
    for (auto &iter : appWifiTxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wifi scan timer
    for (auto &iter : appWifiScanTimerMap_) {
        iter.second->Reset();
    }

    // Reset Wifi RX timer
    for (auto &iter : appWifiRxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi TX timer
    for (auto &iter : appWifiTxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi RX counter
    for (auto &iter : appWifiRxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Wifi TX counter
    for (auto &iter : appWifiTxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long WifiEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long count = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxCounterMap_.find(uid);
        if (rxIter != appWifiRxCounterMap_.end()) {
            count = rxIter->second->GetCount();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi RX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No wifi RX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxCounterMap_.find(uid);
        if (txIter != appWifiTxCounterMap_.end()) {
            count = txIter->second->GetCount();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi TX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No wifi TX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> WifiEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        auto scanIter = appWifiScanTimerMap_.find(uid);
        if (scanIter != appWifiScanTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi scan timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return scanIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wifi scan timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiScanTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiTxTimerMap_.find(uid);
        if (rxIter != appWifiRxTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi RX timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return rxIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wifi RX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiRxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxTimerMap_.find(uid);
        if (txIter != appWifiTxTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi TX timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return txIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wifi TX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appWifiTxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::ActiveTimer> WifiEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_WIFI_ON) {
        if (wifiOnTimer_ != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi on timer");
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Create wifi on timer");
            wifiOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        return wifiOnTimer_;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::Counter> WifiEntity::GetOrCreateCounter(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        auto rxIter = appWifiRxCounterMap_.find(uid);
        if (rxIter != appWifiRxCounterMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi RX counter for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return rxIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wifi RX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appWifiRxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return counter;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        auto txIter = appWifiTxCounterMap_.find(uid);
        if (txIter != appWifiTxCounterMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wifi TX counter for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return txIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wifi TX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appWifiTxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return counter;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create counter failed");
        return nullptr;
    }
}

double WifiEntity::GetWifiUidPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double wifiUidPower = StatsUtils::DEFAULT_VALUE;
    auto bundleObj =
        DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "failed to get bundle manager service, return default power");
        return wifiUidPower;
    }
    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    std::string bundleName = "com.ohos.wifi";
    int32_t wifiUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto core = g_statsService->GetBatteryStatsCore();
    auto uidEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    if (uidEntity != nullptr) {
        wifiUidPower = uidEntity->GetEntityPowerMah(wifiUid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get wifi uid power consumption: %{public}lfmAh", wifiUidPower);
    return wifiUidPower;
}

void WifiEntity::DumpInfo(std::string& result)
{
    long time = wifiOnTimer_->GetRunningTimeMs();
    result.append("Wifi on time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS
