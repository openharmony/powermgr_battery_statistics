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

#ifndef HISYSEVENT_OPERATION_H
#define HISYSEVENT_OPERATION_H

#include <string>
#include <sstream>
#include <vector>

namespace OHOS {
namespace PowerMgr {
class HiSysEventOperation {
public:
    template<typename... Types>
    static std::string CombineHiSysEvent(const std::string &domain, const std::string &eventName, int type,
        Types... keyValues)
    {
        return InnerCombine(domain, eventName, type, keyValues...);
    }

private:
    class EventBase {
    public:
        EventBase(const std::string &domain, const std::string &eventName, int type)
            : retCode_(0), keyCnt_(0), domain_(domain), eventName_(eventName), type_(type)
            {};
        ~EventBase() {}
    public:
        int retCode_;
        unsigned int keyCnt_;
        std::stringstream jsonStr_;
        const std::string domain_;
        const std::string eventName_;
        const int type_;
    };
    
private:
    template<typename... Types>
    static std::string InnerCombine(const std::string &domain, const std::string &eventName, int type,
        Types... keyValues)
    {
        EventBase eventBase(domain, eventName, type);
        eventBase.jsonStr_ << "{";
        CombineBaseInfo(eventBase);
        if (IsError(eventBase)) {
            ExplainRetCode(eventBase);
            return eventBase.jsonStr_.str();
        }

        InnerCombine(eventBase, keyValues...);
        if (IsError(eventBase)) {
            ExplainRetCode(eventBase);
            return eventBase.jsonStr_.str();
        }
        eventBase.jsonStr_ << "}";
        return eventBase.jsonStr_.str();
    }

    template<typename T>
    static void AppendData(EventBase &eventBase, const std::string &key, T value)
    {
        if (IsWarnAndUpdate(CheckKey(key), eventBase)) {
            return;
        }
        if (UpdateAndCheckKeyNumIsOver(eventBase)) {
            return;
        }
        eventBase.jsonStr_ << "\"" << key << "\":";
        AppendValue(eventBase, value);
        eventBase.jsonStr_ << ",";
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, bool value, Types... keyValues)
    {
        AppendData<bool>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const char value, Types... keyValues)
    {
        AppendData<short>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key,
        const unsigned char value, Types... keyValues)
    {
        AppendData<unsigned short>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const short value, Types... keyValues)
    {
        AppendData<short>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key,
        const unsigned short value, Types... keyValues)
    {
        AppendData<unsigned short>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const int value, Types... keyValues)
    {
        AppendData<int>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const unsigned int value, Types... keyValues)
    {
        AppendData<unsigned int>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const long value, Types... keyValues)
    {
        AppendData<long>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key,
        const unsigned long value, Types... keyValues)
    {
        AppendData<unsigned long>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const long long value, Types... keyValues)
    {
        AppendData<long long>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase,
        const std::string &key, const unsigned long long value, Types... keyValues)
    {
        AppendData<unsigned long long>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const float value, Types... keyValues)
    {
        AppendData<float>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const double value, Types... keyValues)
    {
        AppendData<double>(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const std::string &value, Types... keyValues)
    {
        AppendData(eventBase, key, value);
        InnerCombine(eventBase, keyValues...);
    }

    template<typename... Types>
    static void InnerCombine(EventBase &eventBase, const std::string &key, const char *value, Types... keyValues)
    {
        AppendData(eventBase, key, std::string(value));
        InnerCombine(eventBase, keyValues...);
    }

    template<typename T>
    static void AppendValue(EventBase &eventBase, const T item)
    {
        eventBase.jsonStr_ << item;
    }

    static void AppendValue(EventBase &eventBase, const std::string &item);
    static void AppendValue(EventBase &eventBase, const char item);
    static void AppendValue(EventBase &eventBase, const signed char item);
    static void AppendValue(EventBase &eventBase, const unsigned char item);
    static void InnerCombine(EventBase &eventBase);
    static void CombineBaseInfo(EventBase &eventBase);

    static int CheckKey(const std::string &key);
    static bool IsError(EventBase &eventBase);
    static void ExplainRetCode(EventBase &eventBase);
    static bool IsWarnAndUpdate(int retCode, EventBase &eventBase);
    static bool UpdateAndCheckKeyNumIsOver(EventBase &eventBase);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // HISYSEVENT_OPERATION_H