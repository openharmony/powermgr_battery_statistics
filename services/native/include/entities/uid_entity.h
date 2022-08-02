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

#ifndef UID_ENTITY_H
#define UID_ENTITY_H

#include <map>

#include "entities/battery_stats_entity.h"
#include "stats_helper.h"

namespace OHOS {
namespace PowerMgr {
class UidEntity : public BatteryStatsEntity {
public:
    UidEntity();
    ~UidEntity() = default;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    double GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE)
        override;
    void UpdateUidMap(int32_t uid) override;
    std::vector<int32_t> GetUids() override;
    void Reset() override;
    void DumpInfo(std::string& result, int32_t uid = StatsUtils::INVALID_VALUE) override;
private:
    std::map<int32_t, double> uidPowerMap_;
    void AddtoStatsList(int32_t uid, double power);
    double GetPowerForCommon(StatsUtils::StatsType statsType, int32_t uid);
    double GetPowerForConnectivity(StatsUtils::StatsType statsType, int32_t uid);
    void DumpForBluetooth(int32_t uid, std::string& result);
    void DumpForCommon(int32_t uid, std::string& result);
    double CalculateForConnectivity(int32_t uid);
    double CalculateForCommon(int32_t uid);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // UID_ENTITY_H