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

#include "entities/bluetooth_entity.h"

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

BluetoothEntity::BluetoothEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH;
}

void BluetoothEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (uid > StatsUtils::INVALID_VALUE) {
        // Calculate Bluetooth scan and traffic power consumption caused by app
        CalculateBtPowerForApp(uid);
    } else {
        // Calculate Bluetooth on and Bluetooth app power consumption caused by Bluetooth hardware
        CalculateBtPower();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BluetoothEntity::CalculateBtPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on time: %{public}ldms", bluetoothOnTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on power average: %{public}lfma", bluetoothOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on power consumption: %{public}lfmAh",
        bluetoothOnPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth uid power consumption: %{public}lfmAh",
        bluetoothUidPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth total power consumption: %{public}lfmAh",
        bluetoothPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BluetoothEntity::CalculateBtPowerForApp(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan time: %{public}ldms", bluetoothScanTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan power average: %{public}lfma", bluetoothScanAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan power consumption: %{public}lfmAh",
        bluetoothScanPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth RX time: %{public}ldms", bluetoothRxTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth RX power average: %{public}lfma", bluetoothRxAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth RX power consumption: %{public}lfmAh",
        bluetoothRxPowerMams / StatsUtils::MS_IN_HOUR);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth TX time: %{public}ldms", bluetoothTxTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth TX power average: %{public}lfma", bluetoothTxAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth TX power consumption: %{public}lfmAh",
        bluetoothTxPowerMams / StatsUtils::MS_IN_HOUR);
    auto iter = appBluetoothPowerMap_.find(uid);
    if (iter != appBluetoothPowerMap_.end()) {
        iter->second = bluetoothUidPowerMah;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update app bluetooth power consumption: %{public}lfmAh for uid: %{public}d",
            bluetoothUidPowerMah, uid);
    } else {
        appBluetoothPowerMap_.insert(std::pair<int32_t, double>(uid, bluetoothUidPowerMah));
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create app bluetooth power consumption: %{public}lfmAh for uid: %{public}d",
            bluetoothUidPowerMah, uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long BluetoothEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        auto scanIter = appBluetoothScanTimerMap_.find(uid);
        if (scanIter != appBluetoothScanTimerMap_.end()) {
            time = scanIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth scan time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No bluetooth scan timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxTimerMap_.find(uid);
        if (rxIter != appBluetoothRxTimerMap_.end()) {
            time = rxIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth RX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No bluetooth RX timer related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxTimerMap_.find(uid);
        if (txIter != appBluetoothTxTimerMap_.end()) {
            time = txIter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth TX time: %{public}ldms for uid: %{public}d", time, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No bluetooth TX timer related with uid: %{public}d found, return 0",
                uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

long BluetoothEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_ON) {
        if (bluetoothOnTimer_) {
            time = bluetoothOnTimer_->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth on time: %{public}ldms", time);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Bluetooth has not been turned on yet, return 0");
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

double BluetoothEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;

    if (uidOrUserId > StatsUtils::INVALID_VALUE) {
        auto iter = appBluetoothPowerMap_.find(uidOrUserId);
        if (iter != appBluetoothPowerMap_.end()) {
            power = iter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got app blueooth power consumption: %{public}lfmAh for uid: %{public}d",
                power, uidOrUserId);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No app blueooth power consumption related with uid: %{public}d found, return 0", uidOrUserId);
        }
    } else {
        power = bluetoothPowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth power consumption: %{public}lfmAh", power);
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double BluetoothEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_ON) {
        power = bluetoothPowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth on power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        auto scanIter = appBluetoothScanPowerMap_.find(uid);
        if (scanIter != appBluetoothScanPowerMap_.end()) {
            power = scanIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE,
                "Got blueooth scan power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No bluetooth scan power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxPowerMap_.find(uid);
        if (rxIter != appBluetoothRxPowerMap_.end()) {
            power = rxIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth RX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No bluetooth RX power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxPowerMap_.find(uid);
        if (txIter != appBluetoothTxPowerMap_.end()) {
            power = txIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth TX power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No bluetooth TX power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void BluetoothEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset Bluetooth on timer and power consumption
    bluetoothPowerMah_ = StatsUtils::DEFAULT_VALUE;
    if (bluetoothOnTimer_) {
        bluetoothOnTimer_->Reset();
    }

    // Reset app Bluetooth total power consumption
    for (auto &iter : appBluetoothPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth scan power consumption
    for (auto &iter : appBluetoothScanPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth RX power consumption
    for (auto &iter : appBluetoothRxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth TX power consumption
    for (auto &iter : appBluetoothTxPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Bluetooth scan timer
    for (auto &iter : appBluetoothScanTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth RX timer
    for (auto &iter : appBluetoothRxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth TX timer
    for (auto &iter : appBluetoothTxTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth RX counter
    for (auto &iter : appBluetoothRxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }

    // Reset Bluetooth TX counter
    for (auto &iter : appBluetoothTxCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long BluetoothEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long count = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxCounterMap_.find(uid);
        if (rxIter != appBluetoothRxCounterMap_.end()) {
            count = rxIter->second->GetCount();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth RX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No bluetooth RX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxCounterMap_.find(uid);
        if (txIter != appBluetoothTxCounterMap_.end()) {
            count = txIter->second->GetCount();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth TX traffic: %{public}ldbytes for uid: %{public}d", count,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No bluetooth TX traffic related with uid: %{public}d found, return 0",
                uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> BluetoothEntity::GetOrCreateTimer(int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        auto scanIter = appBluetoothScanTimerMap_.find(uid);
        if (scanIter != appBluetoothScanTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth scan timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return scanIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create bluetooth scan timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothScanTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothTxTimerMap_.find(uid);
        if (rxIter != appBluetoothRxTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth RX timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return rxIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create bluetooth RX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothRxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxTimerMap_.find(uid);
        if (txIter != appBluetoothTxTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth TX timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return txIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create bluetooth TX timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            appBluetoothTxTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::ActiveTimer> BluetoothEntity::GetOrCreateTimer(StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_ON) {
        if (bluetoothOnTimer_ != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth on timer");
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Create blueooth on timer");
            bluetoothOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        return bluetoothOnTimer_;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

std::shared_ptr<StatsHelper::Counter> BluetoothEntity::GetOrCreateCounter(StatsUtils::StatsType statsType,
    int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        auto rxIter = appBluetoothRxCounterMap_.find(uid);
        if (rxIter != appBluetoothRxCounterMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth RX counter for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return rxIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create bluetooth RX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appBluetoothRxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return counter;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        auto txIter = appBluetoothTxCounterMap_.find(uid);
        if (txIter != appBluetoothTxCounterMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got blueooth TX counter for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return txIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create bluetooth TX counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> counter = std::make_shared<StatsHelper::Counter>();
            appBluetoothTxCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return counter;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create counter failed");
        return nullptr;
    }
}

double BluetoothEntity::GetBluetoothUidPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double bluetoothUidPower = StatsUtils::DEFAULT_VALUE;
    auto bundleObj =
        DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "failed to get bundle manager service, return 0");
        return bluetoothUidPower;
    }

    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    if (bmgr == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "failed to get bundle manager proxy, return 0");
        return bluetoothUidPower;
    }

    std::string bundleName = "com.ohos.bluetooth";
    int32_t bluetoothUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto core = g_statsService->GetBatteryStatsCore();
    auto uidEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    if (uidEntity != nullptr) {
        bluetoothUidPower = uidEntity->GetEntityPowerMah(bluetoothUid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get bluetooth uid power consumption: %{public}lfmAh", bluetoothUidPower);
    return bluetoothUidPower;
}

void BluetoothEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = GetActiveTimeMs(StatsUtils::STATS_TYPE_BLUETOOTH_ON);
    result.append("Bluetooth dump:\n")
        .append("Bluetooth on time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS
