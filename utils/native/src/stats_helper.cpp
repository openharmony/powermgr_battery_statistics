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
#include "stats_helper.h"

#include <ctime>

#include "battery_stats_info.h"

namespace OHOS {
namespace PowerMgr {
long StatsHelper::latestUnplugBootTimeMs_ = StatsUtils::DEFAULT_VALUE;
long StatsHelper::latestUnplugUpTimeMs_ = StatsUtils::DEFAULT_VALUE;
long StatsHelper::onBatteryBootTimeMs_ = StatsUtils::DEFAULT_VALUE;
long StatsHelper::onBatteryUpTimeMs_ = StatsUtils::DEFAULT_VALUE;
bool StatsHelper::onBattery_ = false;
bool StatsHelper::screenOff_ = false;

long StatsHelper::GetBootTimeMs()
{
    long bootTimeMs = StatsUtils::DEFAULT_VALUE;
    struct timespec rawBootTime;
    int errCode = clock_gettime(CLOCK_BOOTTIME, &rawBootTime);
    if (errCode != 0) {
        STATS_HILOGE(COMP_SVC, "Get boot time failed, return default time");
    } else {
        bootTimeMs =
            (long) (rawBootTime.tv_sec * StatsUtils::MS_IN_SECOND + rawBootTime.tv_nsec / StatsUtils::NS_IN_MS);
        STATS_HILOGD(COMP_SVC, "Got boot time: %{public}ld", bootTimeMs);
    }
    return bootTimeMs;
}

long StatsHelper::GetUpTimeMs()
{
    long upTimeMs = StatsUtils::DEFAULT_VALUE;
    struct timespec rawUpTime;
    int errCode = clock_gettime(CLOCK_MONOTONIC, &rawUpTime);
    if (errCode != 0) {
        STATS_HILOGE(COMP_SVC, "Get up time failed, return default time");
    } else {
        upTimeMs = (long) (rawUpTime.tv_sec * StatsUtils::MS_IN_SECOND + rawUpTime.tv_nsec / StatsUtils::NS_IN_MS);
        STATS_HILOGD(COMP_SVC, "Got up time: %{public}ld", upTimeMs);
    }
    return upTimeMs;
}

void StatsHelper::SetOnBattery(bool onBattery)
{
    if (onBattery_ != onBattery) {
        onBattery_ = onBattery;
        // when onBattery is ture, status is unplugin.
        long currentBootTimeMs = GetBootTimeMs();
        long currentUpTimeMs = GetUpTimeMs();
        if (onBattery) {
            STATS_HILOGD(COMP_SVC, "Power supply is disconnected");
            latestUnplugBootTimeMs_ = currentBootTimeMs;
            latestUnplugUpTimeMs_ = currentUpTimeMs;
        } else {
            STATS_HILOGD(COMP_SVC, "Power supply is connected");
            onBatteryBootTimeMs_ += currentBootTimeMs - latestUnplugBootTimeMs_;
            onBatteryUpTimeMs_ += currentUpTimeMs - latestUnplugUpTimeMs_;
        }
    }
}

void StatsHelper::SetScreenOff(bool screenOff)
{
    if (screenOff_ != screenOff) {
        screenOff_ = screenOff;
        STATS_HILOGD(COMP_SVC, "Update screen off state: %{public}d", screenOff);
    }
}

bool StatsHelper::IsOnBattery()
{
    return onBattery_;
}

bool StatsHelper::IsOnBatteryScreenOff()
{
    return onBattery_ && screenOff_;
}

long StatsHelper::GetOnBatteryBootTimeMs()
{
    long onBatteryBootTimeMs = onBatteryBootTimeMs_;
    long currentBootTimeMs = GetBootTimeMs();
    if (IsOnBattery()) {
        onBatteryBootTimeMs += currentBootTimeMs - latestUnplugBootTimeMs_;
    }
    STATS_HILOGD(COMP_SVC, "Got on battery boot time: %{public}ld, currentBootTimeMs: %{public}ld," \
        "latestUnplugBootTimeMs_: %{public}ld",
        onBatteryBootTimeMs, currentBootTimeMs, latestUnplugBootTimeMs_);
    return onBatteryBootTimeMs;
}

long StatsHelper::GetOnBatteryUpTimeMs()
{
    long onBatteryUpTimeMs = onBatteryUpTimeMs_;
    long currentUpTimeMs = GetUpTimeMs();
    if (IsOnBattery()) {
        onBatteryUpTimeMs += currentUpTimeMs - latestUnplugUpTimeMs_;
    }
    STATS_HILOGD(COMP_SVC, "Got on battery up time: %{public}ld", onBatteryUpTimeMs);
    return onBatteryUpTimeMs;
}
} // namespace PowerMgr
} // namespace OHOS