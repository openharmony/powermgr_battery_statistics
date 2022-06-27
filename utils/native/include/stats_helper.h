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

#ifndef STATS_HELPER_H
#define STATS_HELPER_H

#include "stats_log.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class StatsHelper {
public:
    class ActiveTimer {
    public:
        ActiveTimer() = default;
        ~ActiveTimer() = default;
        bool StartRunning()
        {
            if (isRunning_) {
                STATS_HILOGI(COMP_SVC, "Active timer was already started");
                return false;
            }
            startTimeMs_ = GetOnBatteryBootTimeMs();
            isRunning_ = true;
            STATS_HILOGI(COMP_SVC, "Active timer is started");
            return true;
        }

        bool StopRunning()
        {
            if (!isRunning_) {
                STATS_HILOGI(COMP_SVC, "No related active timer is running");
                return false;
            }
            auto stopTimeMs = GetOnBatteryBootTimeMs();
            totalTimeMs_ += stopTimeMs - startTimeMs_;
            isRunning_ = false;
            STATS_HILOGI(COMP_SVC, "Active timer is stopped");
            return true;
        }

        long GetRunningTimeMs()
        {
            if (isRunning_) {
                auto tmpStopTimeMs = GetOnBatteryBootTimeMs();
                totalTimeMs_ += tmpStopTimeMs - startTimeMs_;
                startTimeMs_ = tmpStopTimeMs;
            }
            return totalTimeMs_;
        }

        void AddRunningTimeMs(long avtiveTime)
        {
            if (avtiveTime > StatsUtils::DEFAULT_VALUE) {
                totalTimeMs_ += avtiveTime;
                STATS_HILOGD(COMP_SVC, "Add on active Time: %{public}ld", avtiveTime);
            } else {
                STATS_HILOGE(COMP_SVC, "Invalid active time, ignore");
            }
        }

        void Reset()
        {
            isRunning_ = false;
            startTimeMs_ = GetOnBatteryBootTimeMs();
            totalTimeMs_ = StatsUtils::DEFAULT_VALUE;
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
            if (count > StatsUtils::DEFAULT_VALUE) {
                totalCount_ += count;
                STATS_HILOGD(COMP_SVC, "Add data bytes: %{public}ld, total data bytes is: %{public}ld",
                    count, totalCount_);
            } else {
                STATS_HILOGE(COMP_SVC, "Invalid data counts");
            }
        }

        long GetCount()
        {
            return totalCount_;
        }

        void Reset()
        {
            totalCount_ = StatsUtils::DEFAULT_VALUE;
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
    static long GetBootTimeMs();
    static long GetUpTimeMs();
private:
    static long latestUnplugBootTimeMs_;
    static long latestUnplugUpTimeMs_;
    static long onBatteryBootTimeMs_;
    static long onBatteryUpTimeMs_;
    static bool onBattery_;
    static bool screenOff_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_HELPER_H