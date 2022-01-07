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

#ifndef TIME_HELPER_H
#define TIME_HELPER_H

namespace OHOS {
namespace PowerMgr {
class TimeHelper {
public:
    static void SetOnBattery(bool onBattery);
    static void SetScreenOff(bool screenOff);
    static long GetOnBatteryBootTimeMs();
    static bool IsOnBattery();
    static bool IsOnBatteryScreenOff();
private:
    static long latestUnplugTimeMs_;
    static long onBatteryBootTimeMs_;
    static bool onBattery_;
    static bool screenOff_;
    static long GetBootTimeMs();
};
} // namespace PowerMgr
} // namespace OHOS
#endif // TIME_HELPER_H