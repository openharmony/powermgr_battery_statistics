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

#include "entities/bluetooth_entity.h"

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

BluetoothEntity::BluetoothEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH;
}

void BluetoothEntity::Calculate(int32_t uid)
{
    if (uid > StatsUtils::INVALID_VALUE) {
        // Calculate Bluetooth scan and traffic power consumption caused by app
        CalculateBtPowerForApp(uid);
    } else {
        // Calculate Bluetooth on and Bluetooth app power consumption caused by Bluetooth hardware
        CalculateBtPower();
    }
}

void BluetoothEntity::CalculateBtPower()
{
    // Calculate Bluetooth on power
    auto bluetoothOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_ON);
    auto bluetoothOnTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_BLUETOOTH_ON);
    auto bluetoothOnPowerMah = bluetoothOnAverageMa * bluetoothOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto bluetoothUidPowerMah = GetBluetoothUidPower();

    // Sum Bluetooth power(mAms)
    auto bluetoothPowerMah = bluetoothOnPowerMah + bluetoothUidPowerMah;
    bluetoothPowerMah_ = bluetoothPowerMah;
    totalPowerMah_ += bluetoothOnPowerMah;

    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    statsInfo->SetPower(bluetoothPowerMah);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGD(COMP_SVC, "Calculate bluetooth time: %{public}" PRId64 "ms, power average: %{public}lfma," \
        "power consumption: %{public}lfmAh, uid power consumption: %{public}lfmAh,"                 \
        "total power consumption: %{public}lfmAh",
        bluetoothOnTimeMs,
        bluetoothOnAverageMa,
        bluetoothOnPowerMah,
        bluetoothUidPowerMah,
        bluetoothPowerMah);
}

void BluetoothEntity::CalculateBtPowerForApp(int32_t uid)
{
    // Calculate Bluetooth scan power consumption
    auto bluetoothScanAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_SCAN);
    auto bluetoothScanTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_SCAN);
    auto bluetoothScanPowerMams = bluetoothScanAverageMa * bluetoothScanTimeMs;

    // Calculate Bluetooth traffic power consumption
    // Calculate Bluetooth RX power consumption
    auto bluetoothRxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_RX);
    auto bluetoothRxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    auto bluetoothRxPowerMams = bluetoothRxAverageMa * bluetoothRxTimeMs;

    // Calculate Bluetooth TX power consumption
    auto bluetoothTxAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_BLUETOOTH_TX);
    auto bluetoothTxTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    auto bluetoothTxPowerMams = bluetoothTxAverageMa * bluetoothTxTimeMs;

    auto bluetoothUidPowerMah =
        (bluetoothScanPowerMams + bluetoothRxPowerMams + bluetoothTxPowerMams) / StatsUtils::MS_IN_HOUR;
    STATS_HILOGD(COMP_SVC,"Calculate bluetooth scan time: %{public}" PRId64 "ms, scan power average: %{public}lfma," \
        "scan power consumption: %{public}lfmAh",
        bluetoothScanTimeMs, bluetoothScanAverageMa, bluetoothScanPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGD(COMP_SVC, "Calculate bluetooth RX time: %{public}" PRId64 "ms, RX power average: %{public}lfma"     \
        "RX power consumption: %{public}lfmAh",
        bluetoothRxTimeMs, bluetoothRxAverageMa, bluetoothRxPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGD(COMP_SVC, "Calculate bluetooth TX time: %{public}" PRId64 "ms, TX power average: %{public}lfma"     \
        "TX power consumption: %{public}lfmAh",
        bluetoothTxTimeMs, bluetoothTxAverageMa, bluetoothTxPowerMams / StatsUtils::MS_IN_HOUR);
    auto iter = appBluetoothPowerMap_.find(uid);
    if (iter != appBluetoothPowerMap_.end()) {
        iter->second = bluetoothUidPowerMah;
        STATS_HILOGD(COMP_SVC, "Update app bluetooth power consumption: %{public}lfmAh for uid: %{public}d",
            bluetoothUidPowerMah, uid);
    } else {
        appBluetoothPowerMap_.insert(std::pair<int32_t, double>(uid, bluetoothUidPowerMah));
        STATS_HILOGD(COMP_SVC, "Create app bluetooth power consumption: %{public}lfmAh for uid: %{public}d",
            bluetoothUidPowerMah, uid);
    }
}

int64_t BluetoothEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        auto scanIter = appBluetoothScanTimerMap_.find(uid);
        if (scanIter != appBluetoothScanTimerMap_.end()) {
            time = scanIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Get blueooth scan time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No bluetooth scan timer related to uid: %{public}d was found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxTimerMap_.find(uid);
        if (rxIter != appBluetoothRxTimerMap_.end()) {
            time = rxIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Get blueooth RX time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No bluetooth RX timer related to uid: %{public}d was found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxTimerMap_.find(uid);
        if (txIter != appBluetoothTxTimerMap_.end()) {
            time = txIter->second->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Get blueooth TX time: %{public}" PRId64 "ms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No bluetooth TX timer related to uid: %{public}d was found, return 0",
                uid);
        }
    }
    return time;
}

int64_t BluetoothEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    int64_t time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_ON) {
        if (bluetoothOnTimer_) {
            time = bluetoothOnTimer_->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Get blueooth on time: %{public}" PRId64 "ms", time);
        } else {
            STATS_HILOGD(COMP_SVC, "Bluetooth has not been turned on yet, return 0");
        }
    }
    return time;
}

double BluetoothEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (uidOrUserId > StatsUtils::INVALID_VALUE) {
        auto iter = appBluetoothPowerMap_.find(uidOrUserId);
        if (iter != appBluetoothPowerMap_.end()) {
            power = iter->second;
            STATS_HILOGD(COMP_SVC, "Get app blueooth power consumption: %{public}lfmAh for uid: %{public}d",
                power, uidOrUserId);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No app blueooth power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
        }
    } else {
        power = bluetoothPowerMah_;
        STATS_HILOGD(COMP_SVC, "Get blueooth power consumption: %{public}lfmAh", power);
    }
    return power;
}

double BluetoothEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_ON) {
        power = bluetoothPowerMah_;
        STATS_HILOGD(COMP_SVC, "Get blueooth on power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        auto scanIter = appBluetoothScanPowerMap_.find(uid);
        if (scanIter != appBluetoothScanPowerMap_.end()) {
            power = scanIter->second;
            STATS_HILOGD(COMP_SVC,
                "Get blueooth scan power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No bluetooth scan power consumption related to uid: %{public}d was found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxPowerMap_.find(uid);
        if (rxIter != appBluetoothRxPowerMap_.end()) {
            power = rxIter->second;
            STATS_HILOGD(COMP_SVC, "Get blueooth RX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No bluetooth RX power consumption related to uid: %{public}d was found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxPowerMap_.find(uid);
        if (txIter != appBluetoothTxPowerMap_.end()) {
            power = txIter->second;
            STATS_HILOGD(COMP_SVC, "Get blueooth TX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No bluetooth TX power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

void BluetoothEntity::Reset()
{
    STATS_HILOGI(COMP_SVC, "Reset");
    // Reset Bluetooth on timer and power consumption
    bluetoothPowerMah_ = StatsUtils::DEFAULT_VALUE;
    if (bluetoothOnTimer_) {
        bluetoothOnTimer_->Reset();
    }

    // Reset app Bluetooth total power consumption
    for (auto& iter : appBluetoothPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth scan power consumption
    for (auto& iter : appBluetoothScanPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth RX power consumption
    for (auto& iter : appBluetoothRxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth TX power consumption
    for (auto& iter : appBluetoothTxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth scan timer
    for (auto& iter : appBluetoothScanTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth RX timer
    for (auto& iter : appBluetoothRxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth TX timer
    for (auto& iter : appBluetoothTxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth RX counter
    for (auto& iter : appBluetoothRxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth TX counter
    for (auto& iter : appBluetoothTxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}

int64_t BluetoothEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    int64_t count = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxCounterMap_.find(uid);
        if (rxIter != appBluetoothRxCounterMap_.end()) {
            count = rxIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Get blueooth RX traffic: %{public}" PRId64 "bytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No bluetooth RX traffic related to uid: %{public}d was found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxCounterMap_.find(uid);
        if (txIter != appBluetoothTxCounterMap_.end()) {
            count = txIter->second->GetCount();
            STATS_HILOGD(COMP_SVC, "Get blueooth TX traffic: %{public}" PRId64 "bytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGD(COMP_SVC, "No bluetooth TX traffic related to uid: %{public}d was found, return 0",
                uid);
        }
    }
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> BluetoothEntity::GetOrCreateTimer(int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN: {
            auto scanIter = appBluetoothScanTimerMap_.find(uid);
            if (scanIter != appBluetoothScanTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get blueooth scan timer for uid: %{public}d", uid);
                timer = scanIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create bluetooth scan timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> scanTimer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothScanTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, scanTimer));
            timer = scanTimer;
            break;
        }
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX: {
            auto rxIter = appBluetoothTxTimerMap_.find(uid);
            if (rxIter != appBluetoothRxTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get blueooth RX timer for uid: %{public}d", uid);
                timer = rxIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create bluetooth RX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> rxTimer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothRxTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, rxTimer));
            timer = rxTimer;
            break;
        }
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX: {
            auto txIter = appBluetoothTxTimerMap_.find(uid);
            if (txIter != appBluetoothTxTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get blueooth TX timer for uid: %{public}d", uid);
                timer = txIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create bluetooth TX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> txTimer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothTxTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, txTimer));
            timer = txTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

std::shared_ptr<StatsHelper::ActiveTimer> BluetoothEntity::GetOrCreateTimer(StatsUtils::StatsType statsType,
    int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON: {
            if (bluetoothOnTimer_ != nullptr) {
                STATS_HILOGD(COMP_SVC, "Get blueooth on timer");
                timer = bluetoothOnTimer_;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create blueooth on timer");
            bluetoothOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
            timer = bluetoothOnTimer_;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

std::shared_ptr<StatsHelper::Counter> BluetoothEntity::GetOrCreateCounter(StatsUtils::StatsType statsType,
    int32_t uid)
{
    std::shared_ptr<StatsHelper::Counter> counter = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX: {
            auto rxIter = appBluetoothRxCounterMap_.find(uid);
            if (rxIter != appBluetoothRxCounterMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get blueooth RX counter for uid: %{public}d", uid);
                counter = rxIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create bluetooth RX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> rxCounter = std::make_shared<StatsHelper::Counter>();
            appBluetoothRxCounterMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, rxCounter));
            counter = rxCounter;
            break;
        }
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX: {
            auto txIter = appBluetoothTxCounterMap_.find(uid);
            if (txIter != appBluetoothTxCounterMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get blueooth TX counter for uid: %{public}d", uid);
                counter = txIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create bluetooth TX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> txCounter = std::make_shared<StatsHelper::Counter>();
            appBluetoothTxCounterMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, txCounter));
            counter = txCounter;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create counter failed");
            break;
    }
    return counter;
}

double BluetoothEntity::GetBluetoothUidPower()
{
    double bluetoothUidPower = StatsUtils::DEFAULT_VALUE;
    auto bundleObj =
        DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGW(COMP_SVC, "Failed to get bundle manager service, return 0");
        return bluetoothUidPower;
    }

    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    if (bmgr == nullptr) {
        STATS_HILOGW(COMP_SVC, "Failed to get bundle manager proxy, return 0");
        return bluetoothUidPower;
    }

    std::string bundleName = "com.ohos.bluetooth";
    int32_t bluetoothUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto core = g_statsService->GetBatteryStatsCore();
    auto uidEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    if (uidEntity != nullptr) {
        bluetoothUidPower = uidEntity->GetEntityPowerMah(bluetoothUid);
    }
    STATS_HILOGD(COMP_SVC, "Get bluetooth uid power consumption: %{public}lfmAh", bluetoothUidPower);
    return bluetoothUidPower;
}

void BluetoothEntity::DumpInfo(std::string& result, int32_t uid)
{
    int64_t time = GetActiveTimeMs(StatsUtils::STATS_TYPE_BLUETOOTH_ON);
    result.append("Bluetooth dump:\n")
        .append("Bluetooth on time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS
