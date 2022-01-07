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

#include "battery_stats_entity.h"
#include "uid_entity.h"

namespace OHOS {
namespace PowerMgr {
class UserEntity : public BatteryStatsEntity {
public:
    UserEntity(int32_t userId);
    ~UserEntity() = default;
    int32_t GetUid() override;
    void AddUidRelatedUserPower(double power) override;
    void Calculate() override;
    void Reset() override;
private:
    int32_t userId_ = BatteryStatsUtils::INVALID_VALUE;
    // void AggregateUserPower(std::shared_ptr<BatteryStatsEntity> entity);
    double uidRelatedUserPower_ = BatteryStatsUtils::DEFAULT_VALUE;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // USER_ENTITY_H
