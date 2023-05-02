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

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "battery_stats.h"
#include "battery_stats_info.h"
#include "napi_error.h"
#include "napi_utils.h"
#include "stats_log.h"

using namespace OHOS::PowerMgr;

namespace {
constexpr uint32_t MAX_ARGC = 1;
constexpr uint32_t ARGV_IND_0 = 0;
} // namespace

static napi_value GetBatteryStats(napi_env env, napi_callback_info info)
{
    size_t argc = MAX_ARGC;
    napi_value argv[argc];

    NapiUtils::GetCallbackInfo(env, info, argc, argv);
    NapiError error;

    if (argc > MAX_ARGC) {
        return error.ThrowError(env, StatsError::ERR_PARAM_INVALID);
    }

    BatteryStats stats(env);
    if (argc == MAX_ARGC) {
        if (!NapiUtils::CheckValueType(env, argv[ARGV_IND_0], napi_function)) {
            return error.ThrowError(env, StatsError::ERR_PARAM_INVALID);
        }
        stats.StatsAsyncCallBack(argv[ARGV_IND_0]);
        return nullptr;
    }

    return stats.StatsPromise();
}

static napi_value GetAppStatsMah(napi_env env, napi_callback_info info)
{
    BatteryStats stats(env);
    return stats.GetAppStatsMah(info, MAX_ARGC, ARGV_IND_0);
}

static napi_value GetAppStatsPercent(napi_env env, napi_callback_info info)
{
    BatteryStats stats(env);
    return stats.GetAppStatsPercent(info, MAX_ARGC, ARGV_IND_0);
}

static napi_value GetPartStatsMah(napi_env env, napi_callback_info info)
{
    BatteryStats stats(env);
    return stats.GetPartStatsMah(info, MAX_ARGC, ARGV_IND_0);
}

static napi_value GetPartStatsPercent(napi_env env, napi_callback_info info)
{
    BatteryStats stats(env);
    return stats.GetPartStatsPercent(info, MAX_ARGC, ARGV_IND_0);
}

static napi_value EnumStatsTypeConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

static napi_value CreateEnumStatsType(napi_env env, napi_value exports)
{
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
    .nm_priv = ((void*)0),
    .reserved = {0}};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&batteryStatsModule);
}
