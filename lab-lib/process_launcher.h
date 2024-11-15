#ifndef PROCESS_LAUNCHER_H
#define PROCESS_LAUNCHER_H

#include <string>
#include <system_error>

class ProcessLauncher {
public:
    // Запуск процесса в фоновом режиме, возвращает идентификатор процесса
    static int launch(const std::string& command, bool waitForCompletion = false, int* exitCode = nullptr);
};

#endif // PROCESS_LAUNCHER_H
