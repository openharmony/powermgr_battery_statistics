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

#include "battery_stats.h"

#include <memory>

#include "async_callback_info.h"
#include "battery_stats_client.h"
#include "battery_stats_info.h"
#include "napi_utils.h"
#include "stats_common.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
BatteryStats::BatteryStats(napi_env& env) : env_(env) {}

void BatteryStats::StatsAsyncCallBack(napi_value& value)
{
    std::unique_ptr<AsyncCallbackInfo> asyncInfo = std::make_unique<AsyncCallbackInfo>();
    asyncInfo->CreateCallback(env_, value);
    napi_value resource = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, "StatsAsyncCallBack", NAPI_AUTO_LENGTH, &resource));
    napi_create_async_work(
        env_, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncCallbackInfo* asCallbackInfo = (AsyncCallbackInfo*)data;
            STATS_RETURN_IF(asCallbackInfo == nullptr);
            asCallbackInfo->GetError().Error(asCallbackInfo->GetData().GetBatteryStatsInfo());
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncCallbackInfo* asCallbackInfo = (AsyncCallbackInfo*)data;
            STATS_RETURN_IF(asCallbackInfo == nullptr);
            napi_value arrRes = nullptr;
            asCallbackInfo->GetData().CreateArrayValue(env, arrRes);
            asCallbackInfo->CallFunction(env, arrRes);
            asCallbackInfo->Release(env);
            delete asCallbackInfo;
        },
        (void*)asyncInfo.get(), &asyncInfo->GetAsyncWork());
    NAPI_CALL_RETURN_VOID(env_, napi_queue_async_work(env_, asyncInfo->GetAsyncWork()));
    asyncInfo.release();
}

napi_value BatteryStats::StatsPromise()
{
    std::unique_ptr<AsyncCallbackInfo> asyncInfo = std::make_unique<AsyncCallbackInfo>();
    napi_value promise;
    asyncInfo->CreatePromise(env_, promise);
    STATS_RETURN_IF_WITH_RET(promise == nullptr, nullptr);

    napi_value resourceName = nullptr;
    NAPI_CALL_BASE(env_, napi_create_string_utf8(env_, "StatsPromise", NAPI_AUTO_LENGTH, &resourceName), promise);
    napi_create_async_work(
        env_, nullptr, resourceName,
        [](napi_env env, void* data) {
            AsyncCallbackInfo* asCallbackInfo = (AsyncCallbackInfo*)data;
            STATS_RETURN_IF(asCallbackInfo == nullptr);
            asCallbackInfo->GetError().Error(asCallbackInfo->GetData().GetBatteryStatsInfo());
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncCallbackInfo* asCallbackInfo = (AsyncCallbackInfo*)data;
            STATS_RETURN_IF(asCallbackInfo == nullptr);
            if (asCallbackInfo->GetError().IsError()) {
                napi_reject_deferred(env, asCallbackInfo->GetDeferred(), asCallbackInfo->GetError().GetNapiError(env));
            } else {
                napi_value arrRes = nullptr;
                asCallbackInfo->GetData().CreateArrayValue(env, arrRes);
                napi_resolve_deferred(env, asCallbackInfo->GetDeferred(), arrRes);
            }
            asCallbackInfo->Release(env);
            delete asCallbackInfo;
        },
        (void*)asyncInfo.get(), &asyncInfo->GetAsyncWork());
    NAPI_CALL_BASE(env_, napi_queue_async_work(env_, asyncInfo->GetAsyncWork()), promise);
    asyncInfo.release();
    return promise;
}

napi_value BatteryStats::GetAppStatsMah(napi_callback_info& info, uint32_t maxArgc, uint32_t index)
{
    return GetAppOrPartStats(info, maxArgc, index, [&](int32_t uid, NapiError& error) {
        double appStatsMah = BatteryStatsClient::GetInstance().GetAppStatsMah(uid);
        error.Error(BatteryStatsClient::GetInstance().GetLastError());
        STATS_HILOGD(COMP_FWK, "get app stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
        return appStatsMah;
    });
}

napi_value BatteryStats::GetAppStatsPercent(napi_callback_info& info, uint32_t maxArgc, uint32_t index)
{
    return GetAppOrPartStats(info, maxArgc, index, [&](int32_t uid, NapiError& error) {
        double appStatsPercent = BatteryStatsClient::GetInstance().GetAppStatsPercent(uid);
        error.Error(BatteryStatsClient::GetInstance().GetLastError());
        STATS_HILOGD(COMP_FWK, "get app stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
        return appStatsPercent;
    });
}

napi_value BatteryStats::GetPartStatsMah(napi_callback_info& info, uint32_t maxArgc, uint32_t index)
{
    return GetAppOrPartStats(info, maxArgc, index, [&](int32_t type, NapiError& error) {
        BatteryStatsInfo::ConsumptionType naviveType = BatteryStatsInfo::ConsumptionType(type);
        double partStatsMah = BatteryStatsClient::GetInstance().GetPartStatsMah(naviveType);
        error.Error(BatteryStatsClient::GetInstance().GetLastError());
        STATS_HILOGD(COMP_FWK, "get part stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
        return partStatsMah;
    });
}

napi_value BatteryStats::GetPartStatsPercent(napi_callback_info& info, uint32_t maxArgc, uint32_t index)
{
    return GetAppOrPartStats(info, maxArgc, index, [&](int32_t type, NapiError& error) {
        BatteryStatsInfo::ConsumptionType naviveType = BatteryStatsInfo::ConsumptionType(type);
        double partStatsPercent = BatteryStatsClient::GetInstance().GetPartStatsPercent(naviveType);
        error.Error(BatteryStatsClient::GetInstance().GetLastError());
        STATS_HILOGD(COMP_FWK, "get part stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
        return partStatsPercent;
    });
}

napi_value BatteryStats::GetAppOrPartStats(
    napi_callback_info& info, uint32_t maxArgc, uint32_t index, std::function<double(int32_t, NapiError&)> getAppOrPart)
{
    size_t argc = maxArgc;
    napi_value argv[argc];
    NapiUtils::GetCallbackInfo(env_, info, argc, argv);
    NapiError error;

    if (argc != maxArgc || !NapiUtils::CheckValueType(env_, argv[index], napi_number)) {
        return error.ThrowError(env_, StatsError::ERR_PARAM_INVALID);
    }

    int32_t jsValue;
    napi_get_value_int32(env_, argv[index], &jsValue);
    double statsData = getAppOrPart(jsValue, error);
    if (error.IsError()) {
        return error.ThrowError(env_);
    }

    napi_value result = nullptr;
    napi_create_double(env_, statsData, &result);
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
