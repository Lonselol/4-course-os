#include "process_launcher.h"
#include <iostream>

namespace process_launcher {

int run_process(const std::string& program, const std::vector<std::string>& args, int& exit_code) {
#ifdef _WIN32
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    std::string command = program;
    for (const auto& arg : args) {
        command += " " + arg;
    }

    if (!CreateProcess(
            NULL, 
            const_cast<char*>(command.c_str()), 
            NULL, 
            NULL, 
            FALSE, 
            0, 
            NULL, 
            NULL, 
            &si, 
            &pi)) {
        return -1; // Ошибка создания процесса
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD code;
    if (!GetExitCodeProcess(pi.hProcess, &code)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return -1; // Ошибка получения кода выхода
    }

    exit_code = static_cast<int>(code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0; // Успешное завершение
#else
    pid_t pid = fork();
    if (pid < 0) {
        return -1; // Ошибка fork
    }

    if (pid == 0) {
        // Дочерний процесс
        std::vector<const char*> c_args;
        c_args.push_back(program.c_str());
        for (const auto& arg : args) {
            c_args.push_back(arg.c_str());
        }
        c_args.push_back(NULL);

        execvp(program.c_str(), const_cast<char* const*>(c_args.data()));
        _exit(127); // Ошибка exec
    } else {
        // Родительский процесс
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            return -1; // Ошибка ожидания процесса
        }

        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
            return 0; // Успешное завершение
        } else {
            return -1; // Процесс завершился с ошибкой
        }
    }
#endif
}

} // namespace process_launcher
