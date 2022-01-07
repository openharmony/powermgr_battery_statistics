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
#include "battery_stats_core.h"

#include <json/json.h>
#include <fstream>

#include <ohos_account_kits_impl.h>

#include "bluetooth_entity.h"
#include "idle_entity.h"
#include "phone_entity.h"
#include "radio_entity.h"
#include "screen_entity.h"
#include "wifi_entity.h"
#include "user_entity.h"
#include "stats_hilog_wrapper.h"
#include "battery_stats_info.h"
#include "battery_srv_client.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
namespace{
static const std::string BATTERY_STATS_JSON = "/data/system/battery_stats.json";
} // namespace
bool BatteryStatsCore::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto pmsptr = bss_.promote();
    if (pmsptr == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Initialization failed");
        return false;
    }

    if (cpuReader_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created cpu reader");
        cpuReader_ = std::make_shared<CpuTimeReader>();
        cpuReader_->Init();
    }

    auto& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryPluggedType plugType = batterySrvClient.GetPluggedType();
    if (plugType == BatteryPluggedType::PLUGGED_TYPE_NONE || plugType == BatteryPluggedType::PLUGGED_TYPE_BUTT) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Device is not charing.");
        TimeHelper::SetOnBattery(true);
    } else {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Device is charing.");
        TimeHelper::SetOnBattery(false);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Initialization succeeded");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void BatteryStatsCore::CreateBatteryStatsEntity(int32_t statType, int32_t id)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::shared_ptr<BatteryStatsEntity> entity;
    if (id > BatteryStatsUtils::INVALID_VALUE) {
        if (statsEntityMap_.find(id) == statsEntityMap_.end()) {
            entity = std::make_shared<UidEntity>(id);
            statsEntityMap_.insert(std::make_pair(id, entity));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Created BatteryStatsEntity for uid: %{public}d", id);
            AddUserEntity(id);
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "BatteryStatsEntity exists for uid: %{public}d", id);
        }
    } else {
        if (statsEntityMap_.find(id) == statsEntityMap_.end()) {
            switch (statType) {
                case BatteryStatsInfo::STATS_TYPE_BLUETOOTH:
                    entity = std::make_shared<BluetoothEntity>();
                    break;
                case BatteryStatsInfo::STATS_TYPE_IDLE:
                    entity = std::make_shared<IdleEntity>();
                    break;
                case BatteryStatsInfo::STATS_TYPE_PHONE:
                    entity = std::make_shared<PhoneEntity>();
                    break;
                case BatteryStatsInfo::STATS_TYPE_RADIO:
                    entity = std::make_shared<RadioEntity>();
                    break;
                case BatteryStatsInfo::STATS_TYPE_SCREEN:
                    entity = std::make_shared<ScreenEntity>();
                    break;
                case BatteryStatsInfo::STATS_TYPE_WIFI:
                    entity = std::make_shared<WifiEntity>();
                    break;
                default:
                    STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid type: %{public}d", statType);
                    return;
            }
            statsEntityMap_.insert(std::make_pair(statType, entity));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Created entity for type: %{public}d", statType);
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "BatteryStatsEntity exists for stats type: %{public}d", statType);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::ComputePower() {
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    statsInfoList_.clear();
    totalConsumption = BatteryStatsUtils::DEFAULT_VALUE;
    for (auto uidIter = statsEntityMap_.begin(); uidIter != statsEntityMap_.end(); uidIter++) {
        if (uidIter->second->GetType() == BatteryStatsInfo::STATS_TYPE_APP) {
            uidIter->second->Calculate();
            auto info = std::make_shared<BatteryStatsInfo>();
            auto uid = uidIter->second->GetUid();
            auto uidPower = uidIter->second->GetTotalPower();
            totalConsumption += uidPower;
            info->SetUid(uid);
            info->SetType(BatteryStatsInfo::STATS_TYPE_APP);
            info->SetPower(uidPower);
            statsInfoList_.emplace_back(info);
            int32_t userId = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(uid);
            auto userEntity = GetEntity(userId);
            if (userEntity != nullptr) {
                userEntity->AddUidRelatedUserPower(uidPower);
            }
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculated power: %{public}lf for uid: %{public}d", uidPower, uid);
        }
    }

    for (auto userIter = statsEntityMap_.begin(); userIter != statsEntityMap_.end(); userIter++) {
        if (userIter->second->GetType() == BatteryStatsInfo::STATS_TYPE_USER) {
            userIter->second->Calculate();
            auto info = std::make_shared<BatteryStatsInfo>();
            auto userId = userIter->second->GetUid();
            auto userPower = userIter->second->GetTotalPower();
            info->SetUid(userId);
            info->SetType(BatteryStatsInfo::STATS_TYPE_USER);
            info->SetPower(userPower);
            statsInfoList_.emplace_back(info);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculated power: %{public}lf for userId: %{public}d", userPower,
                userId);
        }
    }

    for (auto hwIter = statsEntityMap_.begin(); hwIter != statsEntityMap_.end(); hwIter++) {
        auto type = hwIter->second->GetType();
        if (type != BatteryStatsInfo::STATS_TYPE_INVALID && type != BatteryStatsInfo::STATS_TYPE_APP &&
            type != BatteryStatsInfo::STATS_TYPE_USER) {
            hwIter->second->Calculate();
            auto info = std::make_shared<BatteryStatsInfo>();
            auto hwPower = hwIter->second->GetTotalPower();
            totalConsumption += hwPower;
            info->SetType(type);
            info->SetPower(hwPower);
            statsInfoList_.emplace_back(info);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculated power: %{public}lf for type: %{public}d", hwPower, type);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::AddUserEntity(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int32_t userId = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(uid);
    if (GetEntity(userId) == nullptr) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Entity was not found for userId: %{public}d, so created it.", userId);
        auto entity = std::make_shared<UserEntity>(userId);
        statsEntityMap_.insert(std::make_pair(userId, entity));
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Entity was already in stats entity map for user id: %{public}d", userId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

BatteryStatsInfoList BatteryStatsCore::GetBatteryStats()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return statsInfoList_;
}

std::shared_ptr<BatteryStatsEntity> BatteryStatsCore::GetEntity(int32_t id)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto iter = statsEntityMap_.find(id);
    if (iter != statsEntityMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got the stats entity");
        return iter->second;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Didn't find the stats entity");
        return nullptr;
    }
}

void BatteryStatsCore::UpdateStats(std::string hwId, long time, long data, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter for data and time updating");
    STATS_HILOGI(STATS_MODULE_SERVICE, "hwId: %{public}s, time: %{public}ld, data: %{public}ld, uid: %{public}d",
        hwId.c_str(), time, data, uid);

    auto timer = GetOrCreateTimer(hwId, uid);
    if (timer != nullptr) {
        timer->AddRunningTimeMs(time);
        auto hwType = BatteryStatsInfo::CovertStatsType(hwId);
        CreateBatteryStatsEntity(hwType, uid);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s active timer with time: %{public}ld for uid: %{public}d",
            hwId.c_str(), time, uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related active timer, update failed");
    }

    auto counter = GetOrCreateCounter(hwId, uid);
    if (counter != nullptr) {
        counter->AddCount(data);
        auto hwType = BatteryStatsInfo::CovertStatsType(hwId);
        CreateBatteryStatsEntity(hwType, uid);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s counter with data: %{public}ld for uid: %{public}d",
            hwId.c_str(), data, uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related counter, update failed");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit for data and time updating");
}

void BatteryStatsCore::UpdateStats(std::string hwId, bool isUsing, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter for time updating");
    STATS_HILOGI(STATS_MODULE_SERVICE, "hwId: %{public}s, isUsing: %{public}d, uid: %{public}d", hwId.c_str(), isUsing,
        uid);

    auto timer = GetOrCreateTimer(hwId, uid);
    if (timer != nullptr) {
        if (isUsing) {
            timer->StartRunning();
        } else {
            timer->StopRunning();
        }
        auto hwType = BatteryStatsInfo::CovertStatsType(hwId);
        CreateBatteryStatsEntity(hwType, uid);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s active timer for uid: %{public}d", hwId.c_str(), uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related active timer, update failed");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit for time updating");
}

void BatteryStatsCore::UpdateStats(std::string hwId, BatteryStatsUtils::StatsDataState state, int32_t level,
    int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter for level time updating");
    if (hwId == BatteryStatsUtils::TYPE_SCREEN_ON) {
        int32_t bin = level / (256 / BatteryStatsUtils::DEFAULT_BIN);
        if (bin < 0) {
            bin = 0;
        } else if (bin >= BatteryStatsUtils::DEFAULT_BIN) {
            bin = BatteryStatsUtils::DEFAULT_BIN-1;
        }
        auto onTimer = GetOrCreateTimer(hwId);
        auto binTimer = GetOrCreateLevelTimer(BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS, bin);
        if (onTimer == nullptr || binTimer == nullptr) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer, update failed");
            return;
        }
        if (state == BatteryStatsUtils::STATE_DISPLAY_OFF) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d", hwId.c_str(), state);
            onTimer->StopRunning();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated brightness bin: %{public}d timer", bin);
            binTimer->StopRunning();
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d", hwId.c_str(), state);
            onTimer->StartRunning();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated brightness bin: %{public}d timer", bin);
            binTimer->StartRunning();
        }
    } else if (hwId == BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS) {
        int32_t bin = level / (256 / BatteryStatsUtils::DEFAULT_BIN);
        if (bin < 0) {
            bin = 0;
        } else if (bin >= BatteryStatsUtils::DEFAULT_BIN) {
            bin = BatteryStatsUtils::DEFAULT_BIN-1;
        }
        if (lastScreenBrightnessbin_ != bin) {
            if (state == BatteryStatsUtils::STATE_DISPLAY_ON) {
                auto oldBinTimer = GetOrCreateLevelTimer(hwId, lastScreenBrightnessbin_);
                auto newBinTimer = GetOrCreateLevelTimer(hwId, bin);
                if (oldBinTimer != nullptr) {
                    oldBinTimer->StopRunning();
                } else {
                    STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer - bin: %{public}d, update failed",
                        lastScreenBrightnessbin_);
                }
                if (newBinTimer != nullptr) {
                    STATS_HILOGI(STATS_MODULE_SERVICE, "Updated brightness bin: %{public}d timer", bin);
                    newBinTimer->StartRunning();
                } else {
                    STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer, update failed");
                }
            }
            lastScreenBrightnessbin_ = bin;
        }
    } else if (hwId == BatteryStatsUtils::TYPE_RADIO_SCAN) {
        bool scanning = false;
        auto scanTimer = GetOrCreateTimer(hwId);
        if (scanTimer == nullptr) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer, update failed");
            return;
        }
        if (state == BatteryStatsUtils::STATE_NETWORK_SEARCH) {
            scanning = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d", hwId.c_str(), state);
            scanTimer->StartRunning();
        }
        if (!scanning) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d", hwId.c_str(), state);
            scanTimer->StopRunning();
        }
    } else if (hwId == BatteryStatsUtils::TYPE_RADIO_ON) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Last signal bin: %{public}d", lastSignalStrengthbin_);
        if (lastSignalStrengthbin_ != level) {
            auto oldBinTimer = GetOrCreateLevelTimer(hwId, lastSignalStrengthbin_);
            auto newBinTimer = GetOrCreateLevelTimer(hwId, level);
            if (oldBinTimer != nullptr) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Updated signal bin: %{public}d timer", level);
                oldBinTimer->StopRunning();
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer - bin: %{public}d, update failed",
                    lastSignalStrengthbin_);
            }
            if (newBinTimer != nullptr) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Updated signal bin: %{public}d timer", level);
                newBinTimer->StartRunning();
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer, update failed");
            }
            lastSignalStrengthbin_ = level;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit for level time updating");
}

std::shared_ptr<BatteryStatsCore::ActiveTimer> BatteryStatsCore::GetOrCreateLevelTimer(std::string hwId, int32_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::shared_ptr<BatteryStatsCore::ActiveTimer> timer = nullptr;
    if (level > BatteryStatsUtils::INVALID_VALUE && level < BatteryStatsUtils::DEFAULT_BIN) {
        if (levelTimerMap_.find(hwId) == levelTimerMap_.end()) {
            BatteryStatsCore::LevelTimerMap levelTimerMap;
            std::vector<std::shared_ptr<ActiveTimer>> timerVec;
            for (int i = 0; i < BatteryStatsUtils::DEFAULT_BIN; i++) {
                std::shared_ptr<BatteryStatsCore::ActiveTimer> timer =
                    std::make_shared<BatteryStatsCore::ActiveTimer>();
                timerVec.push_back(timer);
                STATS_HILOGI(STATS_MODULE_SERVICE, "Found no %{public}s active timer, so created it", hwId.c_str());
            }
            levelTimerMap_.insert(std::pair<std::string, std::vector<std::shared_ptr<ActiveTimer>>>(hwId, timerVec));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no LevelTimerMap for type: %{public}s, so created it",
                hwId.c_str());
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s active timer", hwId.c_str());
        timer = levelTimerMap_.at(hwId)[level];
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid level.");
        return timer;
    }
    if (hwId == BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got brightness timer.");
    } else if (hwId == BatteryStatsUtils::TYPE_RADIO_ON) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got signal strength timer.");
    }
    return timer;
}

std::shared_ptr<BatteryStatsCore::ActiveTimer> BatteryStatsCore::GetOrCreateTimer(std::string hwId, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (uid == BatteryStatsUtils::INVALID_VALUE) {
        if (timerMap_.find(hwId) == timerMap_.end()) {
            std::shared_ptr<BatteryStatsCore::ActiveTimer> timer = std::make_shared<BatteryStatsCore::ActiveTimer>();
            timerMap_.insert(std::pair<std::string, std::shared_ptr<BatteryStatsCore::ActiveTimer>>(hwId, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no %{public}s active timer, so created it", hwId.c_str());
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s active timer", hwId.c_str());
        return timerMap_.at(hwId);
    } else {
        if (uidTimerMap_.find(uid) == uidTimerMap_.end()) {
            BatteryStatsCore::TimerMap timerMap;
            uidTimerMap_.insert(std::pair<int32_t, BatteryStatsCore::TimerMap>(uid, timerMap));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no TimerMap for uid: %{public}d, so created it", uid);
        }
        if (uidTimerMap_.at(uid).find(hwId) == uidTimerMap_.at(uid).end()) {
            std::shared_ptr<BatteryStatsCore::ActiveTimer> timer = std::make_shared<BatteryStatsCore::ActiveTimer>();
            uidTimerMap_.at(uid).insert(std::pair<std::string, std::shared_ptr<BatteryStatsCore::ActiveTimer>>(hwId,
                timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no %{public}s active timer for uid: %{public}d, so created it",
                hwId.c_str(), uid);
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s active timer for uid: %{public}d", hwId.c_str(), uid);
        return uidTimerMap_.at(uid).at(hwId);
    }
}

std::shared_ptr<BatteryStatsCore::Counter> BatteryStatsCore::GetOrCreateCounter(std::string hwId, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (uid == BatteryStatsUtils::INVALID_VALUE) {
        if (counterMap_.find(hwId) == counterMap_.end()) {
            std::shared_ptr<BatteryStatsCore::Counter> counter = std::make_shared<BatteryStatsCore::Counter>();
            counterMap_.insert(std::pair<std::string, std::shared_ptr<BatteryStatsCore::Counter>>(hwId, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no %{public}s counter, so created it", hwId.c_str());
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s counter", hwId.c_str());
        return counterMap_.at(hwId);
    } else {
        if (uidCounterMap_.find(uid) == uidCounterMap_.end()) {
            BatteryStatsCore::CounterMap counterMap;
            uidCounterMap_.insert(std::pair<int32_t, BatteryStatsCore::CounterMap>(uid, counterMap));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no CounterMap for uid: %{public}d, so created it", uid);
        }
        if (uidCounterMap_.at(uid).find(hwId) == uidCounterMap_.at(uid).end()) {
            std::shared_ptr<BatteryStatsCore::Counter> counter = std::make_shared<BatteryStatsCore::Counter>();
            uidCounterMap_.at(uid).
                insert(std::pair<std::string, std::shared_ptr<BatteryStatsCore::Counter>>(hwId, counter));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found no %{public}s counter for uid: %{public}d, so created it",
                hwId.c_str(), uid);
        }
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s counter for uid: %{public}d", hwId.c_str(), uid);
        return uidCounterMap_.at(uid).at(hwId);
    }
}

long BatteryStatsCore::GetTotalTimeMs(int32_t level, std::string hwId, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle level: %{public}d", level);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle hwId: %{public}s", hwId.c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle uid: %{public}d", uid);

    long time = BatteryStatsUtils::DEFAULT_VALUE;
    if (uid > BatteryStatsUtils::INVALID_VALUE) {
        std::shared_ptr<BatteryStatsCore::ActiveTimer> timer = nullptr;
        std::vector<std::shared_ptr<ActiveTimer>> timerVec;
        auto iter = levelTimerMap_.find(hwId);
        if (iter != levelTimerMap_.end()) {
            timerVec = iter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Found level timer map for type: %{public}s", hwId.c_str());
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related level timer map for type: %{public}s, return 0",
                hwId.c_str());
            return time;
        }

        bool isFound = false;
        for (u_int16_t i = 0; i < timerVec.size(); i++) {
            if (i == level) {
                timer = timerVec[i];
                isFound = true;
                STATS_HILOGI(STATS_MODULE_SERVICE, "Found level timer for type: %{public}s", hwId.c_str());
            }
        }

        if (!isFound) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related level timer for type: %{public}s, return 0",
                hwId.c_str());
            return time;
        }

        if (timer != nullptr) {
            auto time = timer->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s time: %{public}ld", hwId.c_str(), time);
            return time;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer for hw: %{public}s, return default time",
                hwId.c_str());
            return time;
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Currently no level timer for app");
        return time;
    }
}

void BatteryStatsCore::DumpInfo(std::string& result)
{
    result.append("BATTERY STATS DUMP (hidumper -batterystats):\n");
    cpuReader_->DumpInfo(result);
}

long BatteryStatsCore::GetTotalTimeMs(std::string hwId, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::shared_ptr<BatteryStatsCore::ActiveTimer> timer = nullptr;
    if (uid == BatteryStatsUtils::INVALID_VALUE) {
        auto iter = timerMap_.find(hwId);
        if (iter != timerMap_.end()) {
            timer = iter->second;
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Found no related timer for hw: %{public}s", hwId.c_str());
        }
    } else {
        auto uidIter = uidTimerMap_.find(uid);
        TimerMap timerMap;
        if (uidIter != uidTimerMap_.end()) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Found related timer map for uid: %{public}d", uid);
            timerMap = uidIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer map for uid: %{public}d, return default time",
                uid);
            return BatteryStatsUtils::DEFAULT_VALUE;
        }
        auto iter = timerMap.find(hwId);
        if (iter != timerMap.end()) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Found related timer for hw: %{public}s", hwId.c_str());
            timer = iter->second;
        } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer for hw: %{public}s, return default time",
            hwId.c_str());
            return BatteryStatsUtils::DEFAULT_VALUE;
        }
    }
    if (timer != nullptr) {
        auto time = timer->GetRunningTimeMs();
        if (uid == BatteryStatsUtils::INVALID_VALUE) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s time: %{public}ld", hwId.c_str(), time);
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s time: %{public}ld for uid: %{public}d", hwId.c_str(),
                time, uid);
        }
        return time;
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer for hw: %{public}s, return default time",
            hwId.c_str());
        return BatteryStatsUtils::DEFAULT_VALUE;
    }
}

long BatteryStatsCore::GetTotalDataCount(std::string hwId, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::shared_ptr<BatteryStatsCore::Counter> counter = nullptr;
    if (uid == BatteryStatsUtils::INVALID_VALUE) {
        auto iter = counterMap_.find(hwId);
        if (iter != counterMap_.end()) {
            counter = iter->second;
        }
    } else {
        auto uidIter = uidCounterMap_.find(uid);
        CounterMap counterMap;
        if (uidIter != uidCounterMap_.end()) {
            counterMap = uidIter->second;
        }
        auto iter = counterMap.find(hwId);
        if (iter != counterMap.end()) {
            counter = iter->second;
        }
    }
    if (counter != nullptr) {
        auto count = counter->GetCount();
        if (uid == BatteryStatsUtils::INVALID_VALUE) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s count: %{public}ld for uid: %{public}d", hwId.c_str(),
                count, uid);
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s count: %{public}ld", hwId.c_str(), count);
        }
        return count;
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related counter, return default count");
        return BatteryStatsUtils::DEFAULT_VALUE;
    }
}

double BatteryStatsCore::GetAppStatsMah(const int32_t& uid)
{
    double appStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    for(auto iter = statsInfoList_.begin(); iter != statsInfoList_.end(); iter++){
        if ((*iter)->GetType() == BatteryStatsInfo::STATS_TYPE_APP) {
            if ((*iter)->GetUid() == uid) {
                appStatsMah = (*iter)->GetPower();
                break;
            }
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return appStatsMah;
}

double BatteryStatsCore::GetAppStatsPercent(const int32_t& uid)
{
    double appStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    for(auto iter = statsInfoList_.begin(); iter != statsInfoList_.end(); iter++){
        if ((*iter)->GetType() == BatteryStatsInfo::STATS_TYPE_APP) {
            if ((*iter)->GetUid() == uid) {
                appStatsPercent = (*iter)->GetPower() / totalConsumption;
                break;
            }
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    return appStatsPercent;
}

double BatteryStatsCore::GetPartStatsMah(const BatteryStatsInfo::BatteryStatsType& type)
{
    double partStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    for(auto iter = statsInfoList_.begin(); iter != statsInfoList_.end(); iter++){
        if ((*iter)->GetType() == type) {
            partStatsMah = (*iter)->GetPower();
            break;
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    return partStatsMah;
}

double BatteryStatsCore::GetPartStatsPercent(const BatteryStatsInfo::BatteryStatsType& type)
{
    double partStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    for(auto iter = statsInfoList_.begin(); iter != statsInfoList_.end(); iter++){
        if ((*iter)->GetType() == type) {
            partStatsPercent = (*iter)->GetPower() / totalConsumption;
            break;
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return partStatsPercent;
}

bool BatteryStatsCore::SaveBatteryStatsData()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    ComputePower();
    Json::Value root;
    for(auto iter = statsInfoList_.begin(); iter != statsInfoList_.end(); iter++){
        if ((*iter)->GetUid() == BatteryStatsUtils::INVALID_VALUE) {
            std::string name = std::to_string((*iter)->GetType());
            root[name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Saved power: %{public}lf for type: %{public}s", (*iter)->GetPower(),
                name.c_str());

        } else {
            std::string name = std::to_string((*iter)->GetUid());
            root[name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Saved power: %{public}lf for uid: %{public}s", (*iter)->GetPower(),
                name.c_str());
        }
    }
    Json::StreamWriterBuilder swb;
    std::ofstream ofs;
    ofs.open(BATTERY_STATS_JSON);
    if (!ofs.is_open()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening json file failed");
        return false;
    }
    swb.newStreamWriter()->write(root, &ofs);
    ofs.close();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool BatteryStatsCore::LoadBatteryStatsData()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errors;
    std::ifstream ifs(BATTERY_STATS_JSON, std::ios::binary);
    if( !ifs.is_open()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Json file doesn't exist");
        return false;
    }
    if (!parseFromStream(reader, ifs, &root, &errors)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parsing json file failed");
        return false;
    }
    ifs.close();
    statsInfoList_.clear();
    std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
    Json::Value::Members member = root.getMemberNames();
    for (auto iter = member.begin(); iter != member.end(); iter++) {
        auto id = std::stoi(*iter);
        if (id > BatteryStatsUtils::INVALID_VALUE) {
            info->SetUid(id);
        } else if (id < BatteryStatsUtils::INVALID_VALUE && id > BatteryStatsInfo::STATS_TYPE_INVALID ){
            info->SetType(static_cast<BatteryStatsInfo::BatteryStatsType>(id));
        }
        info->SetPower(root[*iter].asDouble());
        STATS_HILOGI(STATS_MODULE_SERVICE, "Loaded power: %{public}lf for uid: %{public}d", info->GetPower(),
            info->GetUid());
        statsInfoList_.emplace_back(info);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void BatteryStatsCore::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Rest all the uid related timers.
    STATS_HILOGI(STATS_MODULE_SERVICE, "Rest all the uid related timers");
    for (auto timerMapIter = uidTimerMap_.begin(); timerMapIter != uidTimerMap_.end(); timerMapIter++) {
        auto timerMap = timerMapIter->second;
        for (auto uidTimerIter = timerMap.begin(); uidTimerIter != timerMap.end(); uidTimerIter++) {
            uidTimerIter->second->Reset();
        }
    }

    // Rest all the uid related counters.
    STATS_HILOGI(STATS_MODULE_SERVICE, "Rest all the uid related counters");
    for (auto counterMapIter = uidCounterMap_.begin(); counterMapIter != uidCounterMap_.end(); counterMapIter++) {
        auto counterMap = counterMapIter->second;
        for (auto uidCounterIter = counterMap.begin(); uidCounterIter != counterMap.end(); uidCounterIter++) {
            uidCounterIter->second->Reset();
        }
    }

    // Rest all the hw related timers.
    STATS_HILOGI(STATS_MODULE_SERVICE, "Rest all the hw related timers");
    for (auto timerIter = timerMap_.begin(); timerIter != timerMap_.end(); timerIter++) {
        timerIter->second->Reset();
    }

    // Rest all the hw related counters.
    STATS_HILOGI(STATS_MODULE_SERVICE, "Rest all the hw related counters");
    for (auto counterIter = counterMap_.begin(); counterIter != counterMap_.end(); counterIter++) {
        counterIter->second->Reset();
    }

    // Rest all the entities.
    STATS_HILOGI(STATS_MODULE_SERVICE, "Rest all the entities");
    for (auto statsIter = statsEntityMap_.begin(); statsIter != statsEntityMap_.end(); statsIter++) {
        statsIter->second->Reset();
    }

    // Reset info list
    STATS_HILOGI(STATS_MODULE_SERVICE, "Reset info list");
    statsInfoList_.clear();

    // Reset total consumption
    totalConsumption = BatteryStatsUtils::DEFAULT_VALUE;
}
} // namespace PowerMgr
} // namespace OHOS