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

#ifndef USER_ENTITY_H
#define USER_ENTITY_H

#include <map>

#include "entities/battery_stats_entity.h"
#include "entities/uid_entity.h"

namespace OHOS {
namespace PowerMgr {
class UserEntity : public BatteryStatsEntity {
public:
    UserEntity();
    ~UserEntity() = default;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    void AggregateUserPowerMah(int32_t userId, double power) override;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    void Reset() override;
private:
    std::map<int32_t, double> userPowerMap_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // USER_ENTITY_H
