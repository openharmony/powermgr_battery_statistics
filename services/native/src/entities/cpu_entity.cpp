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

#include "entities/cpu_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

CpuEntity::CpuEntity()
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "Created cpu entity");
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_CPU;
    if (!cpuReader_) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created cpu reader");
        cpuReader_ = std::make_shared<CpuTimeReader>();
        cpuReader_->Init();
    }
}

int64_t CpuEntity::GetCpuTimeMs(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int64_t cpuTimeMs = StatsUtils::DEFAULT_VALUE;
    auto iter = cpuTimeMap_.find(uid);
    if (iter != cpuTimeMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu time: %{public}sms for uid: %{public}d",
            std::to_string(cpuTimeMs).c_str(), uid);
        cpuTimeMs = iter->second;
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No cpu time realted with uid: %{public}d found, return 0", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuTimeMs;
}

void CpuEntity::UpdateCpuTime()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (cpuReader_) {
        if (!cpuReader_->UpdateCpuTime()) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Update CPU time failed");
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Update CPU time successfully");
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Can't find CPU reader");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void CpuEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double cpuTotalPowerMah = StatsUtils::DEFAULT_VALUE;
    // Get cpu time related with uid
    std::vector<int64_t> cpuTimeVec = cpuReader_->GetUidCpuTimeMs(uid);
    int64_t cpuTimeMs = StatsUtils::DEFAULT_VALUE;
    for (uint32_t i = 0; i < cpuTimeVec.size(); i++) {
        cpuTimeMs += cpuTimeVec[i];
    }
    auto cpuTimeIter = cpuTimeMap_.find(uid);
    if (cpuTimeIter != cpuTimeMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update cpu time: %{public}sms for uid: %{public}d",
            std::to_string(cpuTimeMs).c_str(), uid);
        cpuTimeIter->second = cpuTimeMs;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create cpu time: %{public}sms for uid: %{public}d",
            std::to_string(cpuTimeMs).c_str(), uid);
        cpuTimeMap_.insert(std::pair<int32_t, int64_t>(uid, cpuTimeMs));
    }

    // Calculate cpu active power
    cpuTotalPowerMah += CalculateCpuActivePower(uid);

    // Calculate cpu cluster power
    cpuTotalPowerMah += CalculateCpuClusterPower(uid);

    // Calculate cpu speed power
    cpuTotalPowerMah += CalculateCpuSpeedPower(uid);

    auto cpuTotalIter = cpuTotalPowerMap_.find(uid);
    if (cpuTotalIter != cpuTotalPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update cpu speed power consumption: %{public}lfmAh for uid: %{public}d",
            cpuTotalPowerMah, uid);
        cpuTotalIter->second = cpuTotalPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create cpu speed power consumption: %{public}lfmAh for uid: %{public}d",
            cpuTotalPowerMah, uid);
        cpuTotalPowerMap_.insert(std::pair<int32_t, double>(uid, cpuTotalPowerMah));
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double CpuEntity::CalculateCpuActivePower(int32_t uid)
{
    double cpuActivePower = StatsUtils::DEFAULT_VALUE;
    double cpuActiveAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_ACTIVE);
    int64_t cpuActiveTimeMs = cpuReader_->GetUidCpuActiveTimeMs(uid);
    cpuActivePower = cpuActiveAverageMa * cpuActiveTimeMs / StatsUtils::MS_IN_HOUR;

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu active average power: %{public}lfma", cpuActiveAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu active time: %{public}sms for uid: %{public}d",
        std::to_string(cpuActiveTimeMs).c_str(), uid);
    auto cpuActiveIter = cpuActivePowerMap_.find(uid);
    if (cpuActiveIter != cpuActivePowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update cpu active power consumption: %{public}lfmAh for uid: %{public}d",
            cpuActivePower, uid);
        cpuActiveIter->second = cpuActivePower;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create cpu active power consumption: %{public}lfmAh for uid: %{public}d",
            cpuActivePower, uid);
        cpuActivePowerMap_.insert(std::pair<int32_t, double>(uid, cpuActivePower));
    }
    return cpuActivePower;
}

double CpuEntity::CalculateCpuClusterPower(int32_t uid)
{
    double cpuClusterPower = StatsUtils::DEFAULT_VALUE;
    for (uint16_t i = 0; i < g_statsService->GetBatteryStatsParser()->GetClusterNum(); i++) {
        double cpuClusterAverageMa =
            g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_CLUSTER, i);
        int64_t cpuClusterTimeMs = cpuReader_->GetUidCpuClusterTimeMs(uid, i);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu cluster: %{public}d average power: %{public}lfma", i,
            cpuClusterAverageMa);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu cluster: %{public}d time: %{public}sms for uid: %{public}d",
            i, std::to_string(cpuClusterTimeMs).c_str(), uid);
        cpuClusterPower += cpuClusterAverageMa * cpuClusterTimeMs / StatsUtils::MS_IN_HOUR;
    }
    auto cpuClusterIter = cpuClusterPowerMap_.find(uid);
    if (cpuClusterIter != cpuClusterPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update cpu cluster power consumption: %{public}lfmAh for uid: %{public}d",
            cpuClusterPower, uid);
        cpuClusterIter->second = cpuClusterPower;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create cpu cluster power consumption: %{public}lfmAh for uid: %{public}d",
            cpuClusterPower, uid);
        cpuClusterPowerMap_.insert(std::pair<int32_t, double>(uid, cpuClusterPower));
    }
    return cpuClusterPower;
}

double CpuEntity::CalculateCpuSpeedPower(int32_t uid)
{
    double cpuSpeedPower = StatsUtils::DEFAULT_VALUE;
    for (uint16_t i = 0; i < g_statsService->GetBatteryStatsParser()->GetClusterNum(); i++) {
        for (uint16_t j = 0; j < g_statsService->GetBatteryStatsParser()->GetSpeedNum(i); j++) {
            std::string statType = StatsUtils::CURRENT_CPU_SPEED + std::to_string(i);
            double cpuSpeedAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(statType, j);
            int64_t cpuSpeedTimeMs = cpuReader_->GetUidCpuFreqTimeMs(uid, i, j);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cluster: %{public}d, speed: %{public}d", j, i);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu speed average power: %{public}lfma", cpuSpeedAverageMa);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu speed time: %{public}sms for uid: %{public}d",
                std::to_string(cpuSpeedTimeMs).c_str(), uid);
            cpuSpeedPower += cpuSpeedAverageMa * cpuSpeedTimeMs / StatsUtils::MS_IN_HOUR;
        }
    }
    auto cpuSpeedIter = cpuSpeedPowerMap_.find(uid);
    if (cpuSpeedIter != cpuSpeedPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update cpu speed power consumption: %{public}lfmAh for uid: %{public}d",
            cpuSpeedPower, uid);
        cpuSpeedIter->second = cpuSpeedPower;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create cpu speed power consumption: %{public}lfmAh for uid: %{public}d",
            cpuSpeedPower, uid);
        cpuSpeedPowerMap_.insert(std::pair<int32_t, double>(uid, cpuSpeedPower));
    }
    return cpuSpeedPower;
}

double CpuEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = cpuTotalPowerMap_.find(uidOrUserId);
    if (iter != cpuTotalPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app cpu total power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app cpu total power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double CpuEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;

    if (statsType == StatsUtils::STATS_TYPE_CPU_ACTIVE) {
        auto cpuActiveIter = cpuActivePowerMap_.find(uid);
        if (cpuActiveIter != cpuActivePowerMap_.end()) {
            power = cpuActiveIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu active power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No cpu active power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_CLUSTER) {
        auto cpuClusterIter = cpuClusterPowerMap_.find(uid);
        if (cpuClusterIter != cpuClusterPowerMap_.end()) {
            power = cpuClusterIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu cluster power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No cpu cluster power consumption related with uid: %{public}d found, return 0", uid);
        }
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_SPEED) {
        auto cpuSpeedIter = cpuSpeedPowerMap_.find(uid);
        if (cpuSpeedIter != cpuSpeedPowerMap_.end()) {
            power = cpuSpeedIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu speed power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No cpu speed power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void CpuEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Cpu time
    for (auto &iter : cpuTimeMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app Cpu total power consumption
    for (auto &iter : cpuTotalPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app Cpu active power consumption
    for (auto &iter : cpuActivePowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app Cpu cluster power consumption
    for (auto &iter : cpuClusterPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset app Cpu speed power consumption
    for (auto &iter : cpuSpeedPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void CpuEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (cpuReader_) {
        cpuReader_->DumpInfo(result, uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS
