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

#ifndef SCREEN_ENTITY_H
#define SCREEN_ENTITY_H

#include "battery_stats_entity.h"

namespace OHOS {
namespace PowerMgr {
class ScreenEntity : public BatteryStatsEntity {
public:
    ScreenEntity();
    ~ScreenEntity() = default;
    void Calculate() override;
private:
    long GetScreenBrightnessTime(int32_t bin);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // SCREEN_ENTITY_H