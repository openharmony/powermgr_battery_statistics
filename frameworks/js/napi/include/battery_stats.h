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

#ifndef BATTERY_STATS_H
#define BATTERY_STATS_H

#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "battery_stats_info.h"
#include "stats_utils.h"

using BatteryStatsInfo = OHOS::PowerMgr::BatteryStatsInfo;
using StatsUtils = OHOS::PowerMgr::StatsUtils;

struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = { 0 };
    void *obj;
    napi_value result;
    bool isSuccess;
};

class JsBatteryStatsInfo {
public:
    int32_t uid_ = StatsUtils::INVALID_VALUE;
    int32_t type_ = BatteryStatsInfo::CONSUMPTION_TYPE_INVALID;
    double power_ = StatsUtils::DEFAULT_VALUE;
};

#endif // BATTERY_STATS_H