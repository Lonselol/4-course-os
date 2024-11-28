#ifndef PROCESS_LIB_H
#define PROCESS_LIB_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <string>
#include <vector>

namespace process_launcher {

// Запуск программы в фоновом режиме
int run_process(const std::string& program, const std::vector<std::string>& args, int& exit_code);

} // namespace process_launcher

#endif // PROCESS_LIB_H
