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
    // Calculate Wifi on power
    auto wifiOnAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_ON);
    auto wifiOnTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_WIFI_ON);
    auto wifiOnPowerMah = wifiOnAverageMa * wifiOnTimeMs / StatsUtils::MS_IN_HOUR;

    // Calculate Wifi scan power
    auto wifiScanAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_WIFI_SCAN);
    auto wifiScanCount = GetConsumptionCount(StatsUtils::STATS_TYPE_WIFI_SCAN);
    auto wifiScanPowerMah = wifiScanAverageMa * wifiScanCount;

    wifiPowerMah_ = wifiOnPowerMah + wifiScanPowerMah;
    totalPowerMah_ += wifiPowerMah_;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    statsInfo->SetPower(wifiPowerMah_);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGD(COMP_SVC, "Calculate wifi power consumption: %{public}lfmAh", wifiPowerMah_);
}

int64_t WifiEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    int64_t time = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WIFI_ON: {
            if (wifiOnTimer_) {
                time = wifiOnTimer_->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get wifi on time: %{public}" PRId64 "ms", time);
                break;
            }
            STATS_HILOGD(COMP_SVC, "Wifi has not been turned on yet, return 0");
            break;
        }
        default:
            break;
    }
    return time;
}

double WifiEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    return wifiPowerMah_;
}

double WifiEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    return wifiPowerMah_;
}

int64_t WifiEntity::GetConsumptionCount(StatsUtils::StatsType statsType, int32_t uid)
{
    int64_t count = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WIFI_SCAN: {
            if (wifiScanCounter_) {
                count = wifiScanCounter_->GetCount();
                STATS_HILOGD(COMP_SVC, "Get wifi scan count: %{public}" PRId64 "", count);
                break;
            }
            STATS_HILOGD(COMP_SVC, "Wifi scan has not been triggered yet, return 0");
            break;
        }
        default:
            break;
    }
    return count;
}

std::shared_ptr<StatsHelper::ActiveTimer> WifiEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WIFI_ON: {
            if (wifiOnTimer_ != nullptr) {
                STATS_HILOGD(COMP_SVC, "Get wifi on timer");
                timer = wifiOnTimer_;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create wifi on timer");
            wifiOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
            timer = wifiOnTimer_;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

std::shared_ptr<StatsHelper::Counter> WifiEntity::GetOrCreateCounter(StatsUtils::StatsType statsType, int32_t uid)
{
    std::shared_ptr<StatsHelper::Counter> counter = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WIFI_SCAN: {
            if (wifiScanCounter_ != nullptr) {
                STATS_HILOGD(COMP_SVC, "Get wifi scan counter");
                counter = wifiScanCounter_;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create wifi scan counter");
            wifiScanCounter_ = std::make_shared<StatsHelper::Counter>();
            counter = wifiScanCounter_;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create wifi scan counter failed");
            break;
    }
    return counter;
}

void WifiEntity::Reset()
{
    // Reset Wifi power consumption
    wifiPowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset Wifi on timer
    if (wifiOnTimer_) {
        wifiOnTimer_->Reset();
    }

    // Reset Wifi scan counter
    if (wifiScanCounter_) {
        wifiScanCounter_->Reset();
    }
}

void WifiEntity::DumpInfo(std::string& result, int32_t uid)
{
    int64_t time = GetActiveTimeMs(StatsUtils::STATS_TYPE_WIFI_ON);
    int64_t conut = GetConsumptionCount(StatsUtils::STATS_TYPE_WIFI_SCAN);
    result.append("Wifi dump:\n")
        .append("Wifi on time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n")
        .append("Wifi scan count: ")
        .append(ToString(conut))
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS
