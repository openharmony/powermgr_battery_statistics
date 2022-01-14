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

#ifndef STATS_HELPER_H
#define STATS_HELPER_H

#include "stats_hilog_wrapper.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class StatsHelper {
public:
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
            startTimeMs_ = GetOnBatteryBootTimeMs();
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
            auto stopTimeMs = GetOnBatteryBootTimeMs();
            totalTimeMs_ += stopTimeMs - startTimeMs_;
            isRunning_ = false;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Active timer is stopped");
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
        }

        long GetRunningTimeMs()
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (isRunning_) {
                auto tmpStopTimeMs = GetOnBatteryBootTimeMs();
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
            if (avtiveTime > StatsUtils::DEFAULT_VALUE) {
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
            startTimeMs_ = GetOnBatteryBootTimeMs();
            totalTimeMs_ = StatsUtils::DEFAULT_VALUE;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit active timer reset");
        }
    private:
        bool isRunning_ = false;
        long startTimeMs_ = StatsUtils::DEFAULT_VALUE;
        long totalTimeMs_ = StatsUtils::DEFAULT_VALUE;
    };

    class Counter {
    public:
        Counter() = default;
        ~Counter() = default;
        void AddCount(long count)
        {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
            if (count > StatsUtils::DEFAULT_VALUE) {
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
            totalCount_ = StatsUtils::DEFAULT_VALUE;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit counter reset");
        }
    private:
        long totalCount_ = StatsUtils::DEFAULT_VALUE;
    };
    static void SetOnBattery(bool onBattery);
    static void SetScreenOff(bool screenOff);
    static long GetOnBatteryBootTimeMs();
    static long GetOnBatteryUpTimeMs();
    static bool IsOnBattery();
    static bool IsOnBatteryScreenOff();
private:
    static long latestUnplugBootTimeMs_;
    static long latestUnplugUpTimeMs_;
    static long onBatteryBootTimeMs_;
    static long onBatteryUpTimeMs_;
    static bool onBattery_;
    static bool screenOff_;
    static long GetBootTimeMs();
    static long GetUpTimeMs();
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_HELPER_H