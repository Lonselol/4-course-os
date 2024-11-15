#include "process_launcher.h"
#include <iostream>

int main() {
    std::string command;

#ifdef _WIN32
    command = "ping -n 1 127.0.0.1";
#else
    command = "ping -c 1 127.0.0.1";
#endif

    std::cout << "Запуск команды: " << command << std::endl;
    int exitCode;
    int pid = ProcessLauncher::launch(command, true, &exitCode);

    if (pid < 0) {
        std::cerr << "Ошибка запуска процесса." << std::endl;
        return 1;
    }

    std::cout << "Процесс завершился с кодом: " << exitCode << std::endl;
    return 0;
}
