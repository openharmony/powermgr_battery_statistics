/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "hisysevent_operation.h"

#include "def.h"
#include "stats_log.h"
#include "string_filter.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace PowerMgr {
bool HiSysEventOperation::IsError(HiSysEventOperation::EventBase &eventBase)
{
    return (eventBase.retCode_ < SUCCESS);
}

void HiSysEventOperation::ExplainRetCode(HiSysEventOperation::EventBase &eventBase)
{
    if (eventBase.retCode_ > SUCCESS) {
        STATS_HILOGD(LABEL_TEST, "invalid data, error=%{public}d, message=%{public}s",
            eventBase.retCode_, ERR_MSG_LEVEL1[eventBase.retCode_ - 1]);
    } else if (eventBase.retCode_ < SUCCESS) {
        STATS_HILOGD(LABEL_TEST, "discard data, error=%{public}d, message=%{public}s",
            eventBase.retCode_, ERR_MSG_LEVEL0[-eventBase.retCode_ - 1]);
    }
}

int HiSysEventOperation::CheckKey(const std::string &key)
{
    if (!StringFilter::GetInstance().IsValidName(key, MAX_PARAM_NAME_LENGTH)) {
        return ERR_KEY_NAME_INVALID;
    }
    return SUCCESS;
}

bool HiSysEventOperation::IsWarnAndUpdate(int retCode, EventBase &eventBase)
{
    if (retCode != SUCCESS) {
        eventBase.retCode_ = retCode;
        return true;
    }
    return false;
}

bool HiSysEventOperation::UpdateAndCheckKeyNumIsOver(HiSysEventOperation::EventBase &eventBase)
{
    eventBase.keyCnt_++;
    if (eventBase.keyCnt_ > MAX_PARAM_NUMBER) {
        eventBase.retCode_ = ERR_KEY_NUMBER_TOO_MUCH;
        return true;
    }
    return false;
}

void HiSysEventOperation::CombineBaseInfo(HiSysEventOperation::EventBase &eventBase)
{
    if (!StringFilter::GetInstance().IsValidName(eventBase.domain_, MAX_DOMAIN_LENGTH)) {
        eventBase.retCode_ = ERR_DOMAIN_NAME_INVALID;
        return;
    }
    if (!StringFilter::GetInstance().IsValidName(eventBase.eventName_, MAX_EVENT_NAME_LENGTH)) {
        eventBase.retCode_ = ERR_EVENT_NAME_INVALID;
        return;
    }
    AppendData(eventBase, "domain_", eventBase.domain_);
    AppendData(eventBase, "name_", eventBase.eventName_);
    AppendData(eventBase, "type_", eventBase.type_);
    eventBase.keyCnt_ = 0;
}

void HiSysEventOperation::InnerCombine(HiSysEventOperation::EventBase &eventBase)
{
    if (eventBase.jsonStr_.tellp() != 0) {
        eventBase.jsonStr_.seekp(-1, std::ios_base::end);
    }
}

void HiSysEventOperation::AppendValue(HiSysEventOperation::EventBase &eventBase, const std::string &item)
{
    std::string text = item;
    if (item.length() > MAX_STRING_LENGTH) {
        text = item.substr(0, MAX_STRING_LENGTH);
        eventBase.retCode_ = ERR_VALUE_LENGTH_TOO_LONG;
    }
    eventBase.jsonStr_ << "\"" << StringFilter::GetInstance().EscapeToRaw(text) << "\"";
}

void HiSysEventOperation::AppendValue(HiSysEventOperation::EventBase &eventBase, const char item)
{
    eventBase.jsonStr_ << static_cast<short>(item);
}

void HiSysEventOperation::AppendValue(EventBase &eventBase, const signed char item)
{
    eventBase.jsonStr_ << static_cast<short>(item);
}

void HiSysEventOperation::AppendValue(HiSysEventOperation::EventBase &eventBase, const unsigned char item)
{
    eventBase.jsonStr_ << static_cast<unsigned short>(item);
}
} // namespace PowerMgr
} // namespace OHOS