/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_STATISTICS_ASYNC_CALLBACK_INFO_H
#define BATTERY_STATISTICS_ASYNC_CALLBACK_INFO_H

#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "battery_stats_info.h"
#include "napi_error.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class AsyncCallbackInfo {
public:
    void CallFunction(napi_env& env, napi_value results);
    void CreatePromise(napi_env& env, napi_value& promise);
    void Release(napi_env& env);
    void CreateCallback(napi_env& env, napi_value& callback);
    napi_async_work& GetAsyncWork()
    {
        return asyncWork_;
    }
    napi_deferred& GetDeferred()
    {
        return deferred_;
    }
    inline NapiError& GetError()
    {
        return error_;
    }

    class AsyncData {
    public:
        struct StatsInfo {
            int32_t uid_ = StatsUtils::INVALID_VALUE;
            int32_t type_ = BatteryStatsInfo::CONSUMPTION_TYPE_INVALID;
            double power_ = StatsUtils::DEFAULT_VALUE;
        };
        StatsError GetBatteryStatsInfo();
        void CreateArrayValue(napi_env& env, napi_value& arrRes);

    private:
        std::vector<StatsInfo> statsInfos_;
    };
    inline AsyncData& GetData()
    {
        return data_;
    }

private:
    napi_ref callbackRef_ = nullptr;
    napi_async_work asyncWork_ = nullptr;
    napi_deferred deferred_ = nullptr;
    NapiError error_;
    AsyncData data_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_STATISTICS_ASYNC_CALLBACK_INFO_H