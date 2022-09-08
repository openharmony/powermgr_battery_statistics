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

#ifndef BATTERY_STATS_H
#define BATTERY_STATS_H

#include <functional>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "napi_error.h"
namespace OHOS {
namespace PowerMgr {
class BatteryStats {
public:
    explicit BatteryStats(napi_env& env);
    void StatsAsyncCallBack(napi_value& value);
    napi_value StatsPromise();
    napi_value GetAppStatsMah(napi_callback_info& info, uint32_t maxArgc, uint32_t index);
    napi_value GetAppStatsPercent(napi_callback_info& info, uint32_t maxArgc, uint32_t index);
    napi_value GetPartStatsMah(napi_callback_info& info, uint32_t maxArgc, uint32_t index);
    napi_value GetPartStatsPercent(napi_callback_info& info, uint32_t maxArgc, uint32_t index);

private:
    napi_value GetAppOrPartStats(napi_callback_info& info, uint32_t maxArgc, uint32_t index,
        std::function<double(int32_t, NapiError&)> getAppOrPart);
    napi_env env_ {nullptr};
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_H
