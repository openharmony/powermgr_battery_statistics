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

#ifndef STATISTICS_SHELL_COMMAND_H
#define STATISTICS_SHELL_COMMAND_H

#include "shell_command.h"

#include "battery_stats_service.h"

namespace OHOS {
namespace PowerMgr {
class StatisticsShellCommand : public OHOS::AAFwk::ShellCommand {
public:
    StatisticsShellCommand(int argc, char *argv[]);
    ~StatisticsShellCommand() = default;
private:
    static constexpr const char * const STATS_DUMP_LOG_PATH = "/data/battery_stats_dump.log";
    static constexpr int DUMP_BUFF_SIZE = 100;
    virtual ErrCode CreateCommandMap() override;
    virtual ErrCode CreateMessageMap() override;
    virtual ErrCode init() override;
    ErrCode CommandDump(void);
    ErrCode CommandHelp(void);
    ErrCode OptionHelp(void);
    ErrCode CheckParameter(void);
};
}
}
#endif // STATISTICS_SHELL_COMMAND_H