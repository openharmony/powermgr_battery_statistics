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

#include "battery_stats_subscriber.h"

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "common_event_support.h"

#include "battery_info.h"
#include "battery_stats_service.h"
#include "stats_helper.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    const int32_t BATTERY_LEVEL_FULL = 100;
}
void BatteryStatsSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    if (statsService == nullptr) {
        STATS_HILOGE(COMP_SVC, "Battery stats service is null");
        return;
    }
    if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SHUTDOWN) {
        statsService->GetBatteryStatsCore()->SaveBatteryStatsData();
        STATS_HILOGI(COMP_SVC, "Received COMMON_EVENT_SHUTDOWN event");
    } else if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED) {
        int capacity = data.GetWant().GetIntParam(BatteryInfo::COMMON_EVENT_KEY_CAPACITY, StatsUtils::INVALID_VALUE);
        int pluggedType = data.GetWant().GetIntParam(
            BatteryInfo::COMMON_EVENT_KEY_PLUGGED_TYPE, StatsUtils::INVALID_VALUE);

        STATS_HILOGI(COMP_SVC,
            "Received COMMON_EVENT_BATTERY_CHANGED event, capacity=%{public}d, pluggedType=%{public}d",
            capacity, pluggedType);
        if (capacity == BATTERY_LEVEL_FULL) {
            statsService->GetBatteryStatsCore()->Reset();
        }
        if (pluggedType == static_cast<int32_t>(BatteryPluggedType::PLUGGED_TYPE_NONE) ||
            pluggedType == static_cast<int32_t>(BatteryPluggedType::PLUGGED_TYPE_BUTT)) {
            StatsHelper::SetOnBattery(true);
        } else {
            StatsHelper::SetOnBattery(false);
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS
