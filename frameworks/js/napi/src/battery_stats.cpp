/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "battery_stats.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "napi/native_api.h"

#include "battery_stats_client.h"
#include "stats_log.h"

using namespace OHOS::PowerMgr;

static void SetValueInt32(const napi_env& env, std::string fieldStr, const int intValue, napi_value& result)
{
    STATS_HILOGD(COMP_FWK, "Set int32_t value: %{public}d", intValue);
    napi_value value;
    napi_create_int32(env, intValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

static void SetValueDouble(const napi_env& env, std::string fieldStr, const double doubleValue, napi_value& result)
{
    STATS_HILOGD(COMP_FWK, "Set double value: %{public}lf", doubleValue);
    napi_value value;
    napi_create_double(env, doubleValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

static void StatsInfoToJsArray(const napi_env& env, const std::vector<BatteryStats>& vecJsStatsInfo,
    const int idx, napi_value& arrayResult)
{
    napi_value result;
    napi_create_object(env, &result);

    SetValueInt32(env, "uid", vecJsStatsInfo[idx].uid_, result);
    SetValueInt32(env, "type", vecJsStatsInfo[idx].type_, result);
    SetValueDouble(env, "power", vecJsStatsInfo[idx].power_, result);

    napi_status status = napi_set_element(env, arrayResult, idx, result);
    if (status != napi_ok) {
        STATS_HILOGE(COMP_FWK, "Napi sets element error: %{public}d", status);
    }
}

static void NativeCppStatsInfoToJsStatsInfo(const BatteryStatsInfoList& vecCppStatsInfos,
    std::vector<BatteryStats>& vecJsStatsInfo)
{
    for (auto& e : vecCppStatsInfos) {
        BatteryStats jsStatsInfo;

        jsStatsInfo.uid_ = e->GetUid();
        jsStatsInfo.type_ = e->GetConsumptionType();
        jsStatsInfo.power_ = e->GetPower();
        vecJsStatsInfo.push_back(jsStatsInfo);
    }
}

static bool GetBatteryStatsInfoList(std::vector<BatteryStats>& vecStatsInfo)
{
    BatteryStatsInfoList vecCppStatsInfos = BatteryStatsClient::GetInstance().GetBatteryStats();
    if (vecCppStatsInfos.size() <= 0) {
        STATS_HILOGW(COMP_FWK, "Get battery stats info failed");
        return false;
    }
    NativeCppStatsInfoToJsStatsInfo(vecCppStatsInfos, vecStatsInfo);
    STATS_HILOGD(COMP_FWK, "CppStatsInfos size: %{public}d, JsStatsInfo size: %{public}d",
        (int)vecCppStatsInfos.size(), (int)vecStatsInfo.size());
    return true;
}

static void BatteryStatsToNapiValue(napi_env env, std::vector<BatteryStats>& vecStatsInfo, napi_value& result)
{
    napi_status status = napi_create_array_with_length(env, vecStatsInfo.size(), &result);
    if (status != napi_ok) {
        STATS_HILOGE(COMP_FWK, "Napi creates array error: %{public}d", status);
        return;
    }
    for (size_t i = 0; i < vecStatsInfo.size(); ++i) {
        StatsInfoToJsArray(env, vecStatsInfo, i, result);
    }
}

static napi_value StatsInfoToPromise(const napi_env& env, std::unique_ptr<AsyncCallbackInfo>& asCallbackInfoPtr,
    napi_value& promise)
{
    napi_value resourceName;
    napi_create_string_latin1(env, "getBatteryStats", NAPI_AUTO_LENGTH, &resourceName);

    napi_deferred deferred;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asCallbackInfoPtr->deferred = deferred;

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfo *asCallbackInfo = (AsyncCallbackInfo *)data;
            GetBatteryStatsInfoList(asCallbackInfo->vecStatsInfo);
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo *asCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value result = nullptr;
            if (!asCallbackInfo->vecStatsInfo.empty()) {
                BatteryStatsToNapiValue(env, asCallbackInfo->vecStatsInfo, result);
                napi_resolve_deferred(env, asCallbackInfo->deferred, result);
            } else {
                napi_reject_deferred(env, asCallbackInfo->deferred, result);
            }
            napi_delete_async_work(env, asCallbackInfo->asyncWork);
            delete asCallbackInfo;
        },
        (void *)asCallbackInfoPtr.get(),
        &asCallbackInfoPtr->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asCallbackInfoPtr->asyncWork));
    asCallbackInfoPtr.release();
    return NULL;
}

static napi_value StatsInfoToCallBack(const napi_env& env, std::unique_ptr<AsyncCallbackInfo>& asCallbackInfoPtr,
    const size_t argc, const napi_value *argv)
{
    napi_value resourceName;
    napi_create_string_latin1(env, "getBatteryStats", NAPI_AUTO_LENGTH, &resourceName);

    for (size_t i = 0; i != argc; ++i) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[i], 1, &asCallbackInfoPtr->callback[i]);
    }

    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void* data) {
            AsyncCallbackInfo *asCallbackInfo = (AsyncCallbackInfo *)data;
            GetBatteryStatsInfoList(asCallbackInfo->vecStatsInfo);
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncCallbackInfo* asCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value undefine;
            napi_get_undefined(env, &undefine);
            napi_value callback;
            napi_value result = nullptr;
            if (!asCallbackInfo->vecStatsInfo.empty()) {
                BatteryStatsToNapiValue(env, asCallbackInfo->vecStatsInfo, result);
                napi_get_reference_value(env, asCallbackInfo->callback[0], &callback);
                napi_call_function(env, nullptr, callback, 1, &result, &undefine);
            } else {
                if (asCallbackInfo->callback[1]) {
                    napi_get_reference_value(env, asCallbackInfo->callback[1], &callback);
                    napi_call_function(env, nullptr, callback, 1, &result, &undefine);
                } else {
                    STATS_HILOGW(COMP_FWK, "StatsInfoList callback func is null");
                    napi_throw_error(env, "error", "StatsInfoList callback func is null");
                }
            }

            if (asCallbackInfo->callback[0] != nullptr) {
                napi_delete_reference(env, asCallbackInfo->callback[0]);
            }
            if (asCallbackInfo->callback[1] != nullptr) {
                napi_delete_reference(env, asCallbackInfo->callback[1]);
            }
            napi_delete_async_work(env, asCallbackInfo->asyncWork);
            delete asCallbackInfo;
        },
        (void *)asCallbackInfoPtr.get(),
        &asCallbackInfoPtr->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asCallbackInfoPtr->asyncWork));
    asCallbackInfoPtr.release();
    return NULL;
}

static napi_value GetBatteryStats(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));

    AsyncCallbackInfo *asCallbackInfo =
        new AsyncCallbackInfo {.asyncWork = nullptr, .deferred = nullptr};

    std::unique_ptr<AsyncCallbackInfo> asCallbackInfoPtr(asCallbackInfo);

    if (argc >= 1) {
        StatsInfoToCallBack(env, asCallbackInfoPtr, argc, argv);
        STATS_HILOGI(COMP_FWK, "Exit with callback");
        return NULL;
    } else {
        napi_value promise;
        StatsInfoToPromise(env, asCallbackInfoPtr, promise);
        STATS_HILOGI(COMP_FWK, "Exit with promise");
        return promise;
    }
}

static napi_value GetAppStatsMah(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype type1;
    napi_typeof(env, argv[0], &type1);
    NAPI_ASSERT(env, type1 == napi_number, "Wrong argument type. napi_number expected.");

    int32_t uid;
    napi_get_value_int32(env, argv[0], &uid);

    double appStatsMah = BatteryStatsClient::GetInstance().GetAppStatsMah(uid);

    napi_value result;
    napi_create_double(env, appStatsMah, &result);
    STATS_HILOGI(COMP_FWK, "Get battery stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return result;
}

static napi_value GetAppStatsPercent(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype type1;
    napi_typeof(env, argv[0], &type1);
    NAPI_ASSERT(env, type1 == napi_number, "Wrong argument type. napi_number expected.");

    int32_t uid;
    napi_get_value_int32(env, argv[0], &uid);

    double appStatsPercent = BatteryStatsClient::GetInstance().GetAppStatsPercent(uid);

    napi_value result;
    napi_create_double(env, appStatsPercent, &result);
    STATS_HILOGI(COMP_FWK, "Get battery stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    return result;
}

static napi_value GetPartStatsMah(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype type1;
    napi_typeof(env, argv[0], &type1);
    NAPI_ASSERT(env, type1 == napi_number, "Wrong argument type. napi_number expected.");

    int32_t typeJs;
    napi_get_value_int32(env, argv[0], &typeJs);

    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeJs);
    double partStatsMah = BatteryStatsClient::GetInstance().GetPartStatsMah(type);

    napi_value result;
    napi_create_double(env, partStatsMah, &result);
    STATS_HILOGI(COMP_FWK, "Get battery stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    return result;
}

static napi_value GetPartStatsPercent(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

    napi_valuetype type1;
    napi_typeof(env, argv[0], &type1);
    NAPI_ASSERT(env, type1 == napi_number, "Wrong argument type. napi_number expected.");

    int32_t typeJs;
    napi_get_value_int32(env, argv[0], &typeJs);

    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeJs);
    double partStatsPercent = BatteryStatsClient::GetInstance().GetPartStatsPercent(type);

    napi_value result;
    napi_create_double(env, partStatsPercent, &result);
    STATS_HILOGI(COMP_FWK, "Get battery stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return result;
}

static napi_value EnumStatsTypeConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

static napi_value CreateEnumStatsType(napi_env env, napi_value exports)
{
    STATS_HILOGD(COMP_FWK, "Create battery stats type");
    napi_value invalid = nullptr;
    napi_value app = nullptr;
    napi_value bluetooth = nullptr;
    napi_value idle = nullptr;
    napi_value phone = nullptr;
    napi_value radio = nullptr;
    napi_value screen = nullptr;
    napi_value user = nullptr;
    napi_value wifi = nullptr;

    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_INVALID, &invalid);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_APP, &app);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_BLUETOOTH, &bluetooth);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_IDLE, &idle);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_PHONE, &phone);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_RADIO, &radio);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_SCREEN, &screen);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_USER, &user);
    napi_create_int32(env, (int32_t)BatteryStatsInfo::ConsumptionType::CONSUMPTION_TYPE_WIFI, &wifi);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_INVALID", invalid),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_APP", app),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_BLUETOOTH", bluetooth),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_IDLE", idle),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_PHONE", phone),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_RADIO", radio),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_SCREEN", screen),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_USER", user),
        DECLARE_NAPI_STATIC_PROPERTY("CONSUMPTION_TYPE_WIFI", wifi),
    };
    napi_value result = nullptr;
    napi_define_class(env, "ConsumptionType", NAPI_AUTO_LENGTH, EnumStatsTypeConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "ConsumptionType", result);
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value BatteryStatsInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getBatteryStats", GetBatteryStats),
        DECLARE_NAPI_FUNCTION("getAppPowerValue", GetAppStatsMah),
        DECLARE_NAPI_FUNCTION("getAppPowerPercent", GetAppStatsPercent),
        DECLARE_NAPI_FUNCTION("getHardwareUnitPowerValue", GetPartStatsMah),
        DECLARE_NAPI_FUNCTION("getHardwareUnitPowerPercent", GetPartStatsPercent),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    CreateEnumStatsType(env, exports);
    return exports;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module batteryStatsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "batteryStats",
    .nm_register_func = BatteryStatsInit,
    .nm_modname = "batteryStatistics",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&batteryStatsModule);
}
