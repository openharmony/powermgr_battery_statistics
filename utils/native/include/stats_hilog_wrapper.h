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

#ifndef STATS_HILOG_WRAPPER_H
#define STATS_HILOG_WRAPPER_H

#define STATS_CONFIG_HILOG
#ifdef STATS_CONFIG_HILOG
#include "hilog/log.h"
namespace OHOS {
namespace PowerMgr {
#define __STATSFILENAME__            (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define __STATSFORMATED(fmt, ...)    "[%{public}s] %{public}s# " fmt, __STATSFILENAME__, __FUNCTION__, ##__VA_ARGS__

#ifdef STATS_HILOGF
#undef STATS_HILOGF
#endif

#ifdef STATS_HILOGE
#undef STATS_HILOGE
#endif

#ifdef STATS_HILOGW
#undef STATS_HILOGW
#endif

#ifdef STATS_HILOGI
#undef STATS_HILOGI
#endif

#ifdef STATS_HILOGD
#undef STATS_HILOGD
#endif

// param of log interface, such as STATS_HILOGF.
enum BatteryStatsSubModule {
    STATS_MODULE_INNERKIT = 0,
    STATS_MODULE_SERVICE,
    STATS_MODULE_JS_NAPI,
    STATS_MODULE_COMMON,
    STATS_MODULE_BUTT,
};

// 0xD002900: subsystem:PowerMgr module:BatteryStats, reserved 8 bit.
static constexpr unsigned int BASE_POWERMGR_DOMAIN_ID = 0xD002920;

enum BatteryStatsDomainId {
    STATS_DOMAIN_INNERKIT = BASE_POWERMGR_DOMAIN_ID + STATS_MODULE_INNERKIT,
    STATS_DOMAIN_SERVICE,
    STATS_DOMAIN_JS,
    STATS_DOMAIN_COMMON,
    STATS_DOMAIN_BUTT,
};

static constexpr OHOS::HiviewDFX::HiLogLabel STATS_LABEL[STATS_MODULE_BUTT] = {
    {LOG_CORE, STATS_DOMAIN_INNERKIT, "BatteryStatsClient"},
    {LOG_CORE, STATS_DOMAIN_SERVICE, "BatteryStatsService"},
    {LOG_CORE, STATS_DOMAIN_JS, "BatteryStatsJsNapi"},
    {LOG_CORE, STATS_DOMAIN_COMMON, "BatteryStatsCommon"},
};

// In order to improve performance, do not check the module range, module should less than STATS_MODULE_BUTT.
#define STATS_HILOGF(module, ...) (void)OHOS::HiviewDFX::HiLog::Fatal(STATS_LABEL[module], __STATSFORMATED(__VA_ARGS__))
#define STATS_HILOGE(module, ...) (void)OHOS::HiviewDFX::HiLog::Error(STATS_LABEL[module], __STATSFORMATED(__VA_ARGS__))
#define STATS_HILOGW(module, ...) (void)OHOS::HiviewDFX::HiLog::Warn(STATS_LABEL[module], __STATSFORMATED(__VA_ARGS__))
#define STATS_HILOGI(module, ...) (void)OHOS::HiviewDFX::HiLog::Info(STATS_LABEL[module], __STATSFORMATED(__VA_ARGS__))
#define STATS_HILOGD(module, ...) (void)OHOS::HiviewDFX::HiLog::Debug(STATS_LABEL[module], __STATSFORMATED(__VA_ARGS__))
} // namespace PowerMgr
} // namespace OHOS

#else

#define STATS_HILOGF(...)
#define STATS_HILOGE(...)
#define STATS_HILOGW(...)
#define STATS_HILOGI(...)
#define STATS_HILOGD(...)

#endif // STATS_CONFIG_HILOG

#endif // STATS_HILOG_WRAPPER_H