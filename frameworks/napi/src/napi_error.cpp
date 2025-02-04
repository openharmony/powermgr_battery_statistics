/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "napi_error.h"

#include "stats_common.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
std::map<StatsError, std::string> NapiError::errorTable_ = {
    {StatsError::ERR_CONNECTION_FAIL,   "Failed to connect to the service."},
    {StatsError::ERR_PERMISSION_DENIED, "Permission is denied"             },
    {StatsError::ERR_SYSTEM_API_DENIED, "System permission is denied"      },
    {StatsError::ERR_PARAM_INVALID,     "Invalid input parameter."         }
};

napi_value NapiError::GetNapiError(napi_env& env) const
{
    napi_value napiError;
    if (!IsError()) {
        napi_get_undefined(env, &napiError);
        return napiError;
    }

    std::string msg;
    auto item = errorTable_.find(code_);
    if (item != errorTable_.end()) {
        msg = item->second;
    }
    napi_value napiMsg;
    NAPI_CALL(env, napi_create_string_utf8(env, msg.c_str(), msg.size(), &napiMsg));
    NAPI_CALL(env, napi_create_error(env, nullptr, napiMsg, &napiError));

    napi_value napiCode;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(code_), &napiCode));

    napi_set_named_property(env, napiError, "code", napiCode);
    napi_set_named_property(env, napiError, "message", napiMsg);

    STATS_HILOGW(COMP_FWK, "throw error code: %{public}d, msg: %{public}s,", static_cast<int32_t>(code_), msg.c_str());
    return napiError;
}

napi_value NapiError::ThrowError(napi_env& env, StatsError code)
{
    Error(code);
    napi_value error = GetNapiError(env);
    STATS_RETURN_IF_WITH_RET(error == nullptr, nullptr);
    napi_throw(env, error);
    return nullptr;
}
} // namespace PowerMgr
} // namespace OHOS
