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

#include "battery_stats_entity.h"
#include "battery_stats_info.h"

namespace OHOS {
namespace PowerMgr {
class UidEntity : public BatteryStatsEntity {
public:
    UidEntity(int32_t uid);
    ~UidEntity() = default;
    int32_t GetUid() override;
    void Calculate() override;
private:
    int32_t uid_ = BatteryStatsUtils::INVALID_VALUE;
    double CalculateCameraPower();
    double CalculateFlashlightPower();
    double CalculateCpuPower();
    double CalculateRunningLockPower();
    double CalculateMobileRadioPower();
    double CalculateWifiPower();
    double CalculateBluetoothPower();
    double CalculateGpsPower();
    double CalculateSensorPower();
};
} // namespace PowerMgr
} // namespace OHOS
#endif // UID_ENTITY_H