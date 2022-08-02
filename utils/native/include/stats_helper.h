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

#include <cinttypes>

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
                STATS_HILOGD(COMP_SVC, "Active timer was already started");
                return false;
            }
            startTimeMs_ = GetOnBatteryBootTimeMs();
            isRunning_ = true;
            STATS_HILOGD(COMP_SVC, "Active timer is started");
            return true;
        }

        bool StopRunning()
        {
            if (!isRunning_) {
                STATS_HILOGD(COMP_SVC, "No related active timer is running");
                return false;
            }
            auto stopTimeMs = GetOnBatteryBootTimeMs();
            totalTimeMs_ += stopTimeMs - startTimeMs_;
            isRunning_ = false;
            STATS_HILOGD(COMP_SVC, "Active timer is stopped");
            return true;
        }

        int64_t GetRunningTimeMs()
        {
            if (isRunning_) {
                auto tmpStopTimeMs = GetOnBatteryBootTimeMs();
                totalTimeMs_ += tmpStopTimeMs - startTimeMs_;
                startTimeMs_ = tmpStopTimeMs;
            }
            return totalTimeMs_;
        }

        void AddRunningTimeMs(int64_t avtiveTime)
        {
            if (avtiveTime > StatsUtils::DEFAULT_VALUE) {
                totalTimeMs_ += avtiveTime;
                STATS_HILOGD(COMP_SVC, "Add on active Time: %{public}" PRId64 "", avtiveTime);
            } else {
                STATS_HILOGW(COMP_SVC, "Invalid active time, ignore");
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
        int64_t startTimeMs_ = StatsUtils::DEFAULT_VALUE;
        int64_t totalTimeMs_ = StatsUtils::DEFAULT_VALUE;
    };

    class Counter {
    public:
        Counter() = default;
        ~Counter() = default;
        void AddCount(int64_t count)
        {
            if (count > StatsUtils::DEFAULT_VALUE) {
                if (IsOnBattery()) {
                    totalCount_ += count;
                }
                STATS_HILOGD(COMP_SVC, "Add data bytes: %{public}" PRId64 ", total data bytes is: %{public}" PRId64 "",
                    count, totalCount_);
            } else {
                STATS_HILOGW(COMP_SVC, "Invalid data counts");
            }
        }

        int64_t GetCount()
        {
            return totalCount_;
        }

        void Reset()
        {
            totalCount_ = StatsUtils::DEFAULT_VALUE;
        }
    private:
        int64_t totalCount_ = StatsUtils::DEFAULT_VALUE;
    };
    static void SetOnBattery(bool onBattery);
    static void SetScreenOff(bool screenOff);
    static int64_t GetOnBatteryBootTimeMs();
    static int64_t GetOnBatteryUpTimeMs();
    static bool IsOnBattery();
    static bool IsOnBatteryScreenOff();
    static int64_t GetBootTimeMs();
    static int64_t GetUpTimeMs();
private:
    static int64_t latestUnplugBootTimeMs_;
    static int64_t latestUnplugUpTimeMs_;
    static int64_t onBatteryBootTimeMs_;
    static int64_t onBatteryUpTimeMs_;
    static bool onBattery_;
    static bool screenOff_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_HELPER_H