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
#include "time_helper.h"

#include <time.h>

#include "stats_hilog_wrapper.h"
#include "battery_stats_info.h"
#include "battery_stats_utils.h"

namespace OHOS {
namespace PowerMgr {
long TimeHelper::latestUnplugTimeMs_ = BatteryStatsUtils::DEFAULT_VALUE;
long TimeHelper::onBatteryBootTimeMs_ = BatteryStatsUtils::DEFAULT_VALUE;
bool TimeHelper::onBattery_ = false;
bool TimeHelper::screenOff_ = false;
long TimeHelper::GetBootTimeMs() {
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long bootTimeMs = BatteryStatsUtils::DEFAULT_VALUE;
    struct timespec rawBootTime;
    int errCode = clock_gettime(CLOCK_BOOTTIME, &rawBootTime);
    if (errCode != 0) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Get boot time failed, return default time.");
    } else {
        bootTimeMs = (long) (rawBootTime.tv_nsec / 1000000);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got idle time: %{public}ld", bootTimeMs);
    }
    return bootTimeMs;
}

void TimeHelper::SetOnBattery(bool onBattery)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "Enter");
    if (onBattery_ != onBattery) {
        onBattery_ = onBattery;
        // when onBattery is ture, status is unplugin.
        long currentBootTimeMs = GetBootTimeMs();
        if (onBattery) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is disconnected");
            latestUnplugTimeMs_ = currentBootTimeMs;
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is connected");
            onBatteryBootTimeMs_ += currentBootTimeMs - latestUnplugTimeMs_;
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Exit");
}

void TimeHelper::SetScreenOff(bool screenOff)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "Enter");
    if (screenOff_ != screenOff) {
        screenOff_ = screenOff;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Update screen off state: %{public}d", screenOff);
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Exit");
}

bool TimeHelper::IsOnBattery()
{
    return onBattery_;
}

bool TimeHelper::IsOnBatteryScreenOff()
{
    return onBattery_ && screenOff_;
}

long TimeHelper::GetOnBatteryBootTimeMs()
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "Enter");
    long onBatteryBootTimeMs = onBatteryBootTimeMs_;
    long currentBootTimeMs = GetBootTimeMs();
    if (onBattery_) {
        onBatteryBootTimeMs += currentBootTimeMs - latestUnplugTimeMs_;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got on battery boot time: %{public}ld", onBatteryBootTimeMs);
    return onBatteryBootTimeMs;
}
} // namespace PowerMgr
} // namespace OHOS