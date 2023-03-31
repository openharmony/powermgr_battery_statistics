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

#include "napi_utils.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
void NapiUtils::GetCallbackInfo(napi_env& env, napi_callback_info& info, size_t& argc, napi_value argv[])
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    if (napi_ok != napi_get_cb_info(env, info, &argc, argv, &thisVar, &data)) {
        STATS_HILOGW(COMP_FWK, "Failed to get the input parameter");
    }
}

napi_ref NapiUtils::CreateReference(napi_env& env, napi_value& value)
{
    napi_ref refVal = nullptr;
    if (napi_ok != napi_create_reference(env, value, 1, &refVal)) {
        STATS_HILOGW(COMP_FWK, "Failed to create a value reference");
        return refVal;
    }
    return refVal;
}

void NapiUtils::ReleaseReference(napi_env& env, napi_ref& ref)
{
    if (ref != nullptr) {
        napi_delete_reference(env, ref);
        ref = nullptr;
    }
}

bool NapiUtils::CheckValueType(napi_env& env, napi_value& value, napi_valuetype checkType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != checkType) {
        STATS_HILOGW(COMP_FWK, "Parameter type error");
        return false;
    }
    return true;
}

void NapiUtils::SetIntValue(napi_env& env, const std::string fieldStr, const int32_t value, napi_value& result)
{
    napi_value napiValue = nullptr;
    napi_create_int32(env, value, &napiValue);
    napi_set_named_property(env, result, fieldStr.c_str(), napiValue);
}

void NapiUtils::SetDoubleValue(napi_env& env, const std::string fieldStr, const double value, napi_value& result)
{
    napi_value napiValue = nullptr;
    napi_create_double(env, value, &napiValue);
    napi_set_named_property(env, result, fieldStr.c_str(), napiValue);
}
}
}
