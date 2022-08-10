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

#ifndef BATTERY_STATS_CORE_H
#define BATTERY_STATS_CORE_H

#include <memory>
#include <string>
#include <cstdint>
#include <iosfwd>
#include "json/value.h"

#include "battery_stats_info.h"
#include "entities/battery_stats_entity.h"
#include "stats_log.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsCore {
public:
    explicit BatteryStatsCore()
    {
        STATS_HILOGI(COMP_SVC, "BatteryStatsCore instance is created");
    }
    ~BatteryStatsCore() = default;
    void ComputePower();
    BatteryStatsInfoList GetBatteryStats();
    double GetAppStatsMah(const int32_t& uid);
    double GetAppStatsPercent(const int32_t& uid);
    double GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type);
    double GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type);
    int64_t GetTotalTimeMs(StatsUtils::StatsType statsType, int16_t level = StatsUtils::INVALID_VALUE);
    int64_t GetTotalTimeMs(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE);
    int64_t GetTotalDataCount(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE);
    int64_t GetTotalConsumptionCount(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE);
    void UpdateStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state,
        int16_t level = StatsUtils::INVALID_VALUE, int32_t uid = StatsUtils::INVALID_VALUE,
        const std::string& deviceId = "");
    void UpdateStats(StatsUtils::StatsType statsType, int64_t time, int64_t data,
        int32_t uid = StatsUtils::INVALID_VALUE);
    std::shared_ptr<BatteryStatsEntity> GetEntity(const BatteryStatsInfo::ConsumptionType& type);
    bool SaveBatteryStatsData();
    bool LoadBatteryStatsData();
    void DumpInfo(std::string& result);
    void UpdateDebugInfo(const std::string& info);
    void GetDebugInfo(std::string& result);
    void Reset();
    bool Init();
private:
    std::shared_ptr<BatteryStatsEntity> audioEntity_;
    std::shared_ptr<BatteryStatsEntity> bluetoothEntity_;
    std::shared_ptr<BatteryStatsEntity> cameraEntity_;
    std::shared_ptr<BatteryStatsEntity> cpuEntity_;
    std::shared_ptr<BatteryStatsEntity> flashlightEntity_;
    std::shared_ptr<BatteryStatsEntity> gnssEntity_;
    std::shared_ptr<BatteryStatsEntity> idleEntity_;
    std::shared_ptr<BatteryStatsEntity> phoneEntity_;
    std::shared_ptr<BatteryStatsEntity> screenEntity_;
    std::shared_ptr<BatteryStatsEntity> sensorEntity_;
    std::shared_ptr<BatteryStatsEntity> uidEntity_;
    std::shared_ptr<BatteryStatsEntity> userEntity_;
    std::shared_ptr<BatteryStatsEntity> wifiEntity_;
    std::shared_ptr<BatteryStatsEntity> wakelockEntity_;
    std::shared_ptr<BatteryStatsEntity> alarmEntity_;
    bool isCameraOn_ = false;
    bool isScreenOn_ = false;
    int32_t lastBrightnessLevel_ = StatsUtils::INVALID_VALUE;
    int32_t lastCameraUid_ = StatsUtils::INVALID_VALUE;
    std::string debugInfo_;
    void UpdateTimer(std::shared_ptr<BatteryStatsEntity> entity, StatsUtils::StatsType statsType,
        StatsUtils::StatsState state, int32_t uid = StatsUtils::INVALID_VALUE);
    void UpdateTimer(std::shared_ptr<BatteryStatsEntity> entity, StatsUtils::StatsType statsType,
        int64_t time, int32_t uid = StatsUtils::INVALID_VALUE);
    void UpdateCameraTimer(StatsUtils::StatsState state, int32_t uid, const std::string& deviceId);
    void UpdateScreenTimer(StatsUtils::StatsState state);
    void UpdateBrightnessTimer(StatsUtils::StatsState state, int16_t level);
    void UpdateCounter(std::shared_ptr<BatteryStatsEntity> entity, StatsUtils::StatsType statsType,
        int64_t data, int32_t uid = StatsUtils::INVALID_VALUE);
    void UpdateScreenStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int16_t level);
    void UpdateCameraStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int32_t uid,
        const std::string& deviceId);
    void UpdatePhoneStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int16_t level);
    void UpdateConnectivityStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int32_t uid);
    void UpdateCommonStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int32_t uid);
    void CreatePartEntity();
    void CreateAppEntity();
    void SaveForHardware(Json::Value& root);
    void SaveForSoftware(Json::Value& root);
    void SaveForSoftwareCommon(Json::Value& root, int32_t uid);
    void SaveForSoftwareConnectivity(Json::Value& root, int32_t uid);
    void SaveForPower(Json::Value& root);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_CORE_H