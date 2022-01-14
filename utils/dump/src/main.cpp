#include "statistics_shell_command.h"

#include <iostream>

int main(int argc, char *argv[])
{
  OHOS::PowerMgr::StatisticsShellCommand cmd(argc, argv);
  std::cout << cmd.ExecCommand();
  return 0;
}