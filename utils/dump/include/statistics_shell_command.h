#ifndef STATISTICS_SHELL_COMMAND_H
#define STATISTICS_SHELL_COMMAND_H


#include "shell_command.h"

namespace OHOS {
namespace PowerMgr {
class StatisticsShellCommand : public OHOS::AAFwk::ShellCommand {
public:
    StatisticsShellCommand(int argc, char *argv[]);
    ~StatisticsShellCommand() override {};

    ErrCode CreateCommandMap() override;
    ErrCode CreateMessageMap() override;
    ErrCode init() override;
};
}
}

#endif // STATISTICS_SHELL_COMMAND_H