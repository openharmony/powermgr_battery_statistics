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

#ifndef BATTERY_STATS_CORE_H
#define BATTERY_STATS_CORE_H

#include <map>
#include <memory>
#include <string>

#include "battery_stats_utils.h"
#include "battery_stats_entity.h"
#include "battery_stats_service.h"
#include "battery_stats_info.h"
#include "cpu_time_reader.h"
#include "stats_hilog_wrapper.h"
#include "time_helper.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsService;
class BatteryStatsCore {
public:
    explicit BatteryStatsCore(const wptr<BatteryStatsService>& bss) : bss_(bss)
    {
        STATS_HILOGI(STATS_MODULE_SERVICE, "BatteryStatsCore instance is created.");
    }
    ~BatteryStatsCore() = default;
    class ActiveTimer {
    public:
        ActiveTimer() = default;
        ~ActiveTimer() = default;
        void StartRunning()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (isRunning_) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Active timer was already started");
                STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
                return;
            }
            startTimeMs_ = TimeHelper::GetOnBatteryBootTimeMs();
            isRunning_ = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Active timer is started");
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        }

        void StopRunning()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (!isRunning_) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "No related active timer is running");
                STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
                return;
            }
            auto stopTimeMs = TimeHelper::GetOnBatteryBootTimeMs();
            totalTimeMs_ += stopTimeMs - startTimeMs_;
            isRunning_ = false;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Active timer is stopped");
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        }

        long GetRunningTimeMs()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (isRunning_) {
                auto tmpStopTimeMs = TimeHelper::GetOnBatteryBootTimeMs();
                totalTimeMs_ += tmpStopTimeMs - startTimeMs_;
                startTimeMs_ = tmpStopTimeMs;
            }
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got total Time in ms: %{public}ld", totalTimeMs_);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return totalTimeMs_;
        }

        void AddRunningTimeMs(long avtiveTime)
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (avtiveTime > BatteryStatsUtils::DEFAULT_VALUE) {
                totalTimeMs_ += avtiveTime;
                STATS_HILOGI(STATS_MODULE_SERVICE, "Add on active Time: %{public}ld", avtiveTime);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid active time, ignore");
            }
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        }

        void Reset()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter active timer reset");
            isRunning_ = false;
            startTimeMs_ = TimeHelper::GetOnBatteryBootTimeMs();
            totalTimeMs_ = BatteryStatsUtils::DEFAULT_VALUE;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit active timer reset");
        }
    private:
        bool isRunning_ = false;
        long startTimeMs_ = BatteryStatsUtils::DEFAULT_VALUE;
        long totalTimeMs_ = BatteryStatsUtils::DEFAULT_VALUE;
    };

    class Counter {
    public:
        Counter() = default;
        ~Counter() = default;
        void AddCount(long count)
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (count > BatteryStatsUtils::DEFAULT_VALUE) {
                totalCount_ += count;
                STATS_HILOGI(STATS_MODULE_SERVICE, "Add data counts: %{public}ld, the total data counts is: %{public}ld",
                    count, totalCount_);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid data counts");
            }
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        }

        long GetCount()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got total data counts: %{public}ld", totalCount_);
            return totalCount_;
        }

        void Reset()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter counter reset");
            totalCount_ = BatteryStatsUtils::DEFAULT_VALUE;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit counter reset");
        }
    private:
        long totalCount_ = BatteryStatsUtils::DEFAULT_VALUE;
    };
    void CreateBatteryStatsEntity(int32_t statType, int32_t id);
    void ComputePower();
    BatteryStatsInfoList GetBatteryStats();
    double GetAppStatsMah(const int32_t& uid);
    double GetAppStatsPercent(const int32_t& uid);
    double GetPartStatsMah(const BatteryStatsInfo::BatteryStatsType& type);
    double GetPartStatsPercent(const BatteryStatsInfo::BatteryStatsType& type);
    long GetTotalTimeMs(int32_t level, std::string hwId, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    long GetTotalTimeMs(std::string hwId, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    long GetTotalDataCount(std::string hwId, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    void UpdateStats(std::string hwId, BatteryStatsUtils::StatsDataState state, int32_t level,
        int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    void UpdateStats(std::string hwId, bool isUsing, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    void UpdateStats(std::string hwId, long time, long data, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    std::shared_ptr<BatteryStatsEntity> GetEntity(int32_t id);
    bool SaveBatteryStatsData();
    bool LoadBatteryStatsData();
    void DumpInfo(std::string& result);
    void Reset();
    bool Init();
private:
    using EntityMap = std::map<int32_t, std::shared_ptr<BatteryStatsEntity>>;
    using TimerMap = std::map<std::string, std::shared_ptr<ActiveTimer>>;
    using LevelTimerMap = std::map<std::string, std::vector<std::shared_ptr<ActiveTimer>>>;
    using CounterMap = std::map<std::string, std::shared_ptr<Counter>>;
    using UidTimerMap = std::map<int32_t, TimerMap>;
    using UidCounterMap = std::map<int32_t, CounterMap>;
    BatteryStatsInfoList statsInfoList_;
    double totalConsumption = BatteryStatsUtils::DEFAULT_VALUE;
    EntityMap statsEntityMap_;
    TimerMap timerMap_;
    LevelTimerMap levelTimerMap_;
    CounterMap counterMap_;
    UidTimerMap uidTimerMap_;
    UidCounterMap uidCounterMap_;
    int32_t lastScreenBrightnessbin_ = BatteryStatsUtils::INVALID_VALUE;
    int32_t lastSignalStrengthbin_ = BatteryStatsUtils::INVALID_VALUE;
    void AddUserEntity(int32_t uid);
    std::shared_ptr<ActiveTimer> GetOrCreateTimer(std::string hwId, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    std::shared_ptr<ActiveTimer> GetOrCreateLevelTimer(std::string hwId, int32_t level);
    std::shared_ptr<Counter> GetOrCreateCounter(std::string hwId, int32_t uid = BatteryStatsUtils::INVALID_VALUE);
    const wptr<BatteryStatsService> bss_;
    std::shared_ptr<CpuTimeReader> cpuReader_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_CORE_H