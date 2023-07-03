/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef BATTERY_STATS_IPC_INTERFACE_DODE_H
#define BATTERY_STATS_IPC_INTERFACE_DODE_H

/* SAID: 3304 */
namespace OHOS {
namespace PowerMgr {
enum class BatteryStatsInterfaceCode {
    BATTERY_STATS_GET = 0,
    BATTERY_STATS_GETAPPMAH,
    BATTERY_STATS_GETAPPPER,
    BATTERY_STATS_GETPARTMAH,
    BATTERY_STATS_GETPARTPER,
    BATTERY_STATS_GETTIME,
    BATTERY_STATS_GETDATA,
    BATTERY_STATS_RESET,
    BATTERY_STATS_SETONBATT,
    BATTERY_STATS_DUMP,
};
} // space PowerMgr
} // namespace OHOS

#endif // BATTERY_STATS_IPC_INTERFACE_DODE_H