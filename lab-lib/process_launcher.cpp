#include "process_launcher.h"

#ifdef _WIN32
#include <Windows.h>
#include <string>

// Функция для преобразования std::string в std::wstring
std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#endif

int ProcessLauncher::launch(const std::string& command, bool waitForCompletion, int* exitCode) {
#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Преобразуем команду в std::wstring и используем CreateProcessW
    std::wstring wcommand = stringToWString(command);

    if (!CreateProcessW(
            nullptr, &wcommand[0], nullptr, nullptr,
            FALSE, 0, nullptr, nullptr, &si, &pi)) {
        return -1; // Ошибка запуска процесса
    }

    if (waitForCompletion) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code;
        GetExitCodeProcess(pi.hProcess, &code);
        if (exitCode) {
            *exitCode = static_cast<int>(code);
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return pi.dwProcessId;

#else
    pid_t pid = fork();
    if (pid < 0) {
        return -1; // Ошибка запуска процесса
    } else if (pid == 0) {
        // В дочернем процессе
        execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
        _exit(127); // В случае ошибки
    } else {
        // В родительском процессе
        if (waitForCompletion) {
            int status;
            waitpid(pid, &status, 0);
            if (exitCode) {
                *exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
            }
        }
        return pid;
    }
#endif
}
