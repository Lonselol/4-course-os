#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <csignal>
#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

std::atomic<int> counter(0);
std::atomic<bool> is_master(true);
std::atomic<bool> terminate_program(false);
std::mutex log_mutex;
std::string log_file = "log.txt";

void write_log(const std::string &message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log(log_file, std::ios::app);
    if (log.is_open()) {
        log << message << std::endl;
    }
}

std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return std::string(buf) + "." + std::to_string(ms.count());
}

void timer_function() {
    while (!terminate_program) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ++counter;
    }
}

void log_master_function() {
    while (!terminate_program) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        write_log("Master Process [PID: " + std::to_string(getpid()) + "] " +
                  "Time: " + get_current_time() + " Counter: " +
                  std::to_string(counter.load()));
    }
}

void child_copy_1() {
    write_log("Child Copy 1 [PID: " + std::to_string(getpid()) + "] " +
              "Started at: " + get_current_time());
    counter += 10;
    write_log("Child Copy 1 [PID: " + std::to_string(getpid()) + "] " +
              "Exiting at: " + get_current_time());
}

void child_copy_2() {
    write_log("Child Copy 2 [PID: " + std::to_string(getpid()) + "] " +
              "Started at: " + get_current_time());
    counter = counter * 2;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    counter = counter / 2;
    write_log("Child Copy 2 [PID: " + std::to_string(getpid()) + "] " +
              "Exiting at: " + get_current_time());
}

void spawn_children() {
    static std::atomic<bool> child_1_running(false);
    static std::atomic<bool> child_2_running(false);

    if (!child_1_running && !child_2_running) {
#if defined(_WIN32) || defined(_WIN64)
        if (auto handle1 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)child_copy_1, nullptr, 0, nullptr)) {
            child_1_running = true;
            CloseHandle(handle1);
        }
        if (auto handle2 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)child_copy_2, nullptr, 0, nullptr)) {
            child_2_running = true;
            CloseHandle(handle2);
        }
#else
        pid_t pid = fork();
        if (pid == 0) {
            child_copy_1();
            exit(0);
        } else if (pid > 0) {
            child_1_running = true;
        }

        pid = fork();
        if (pid == 0) {
            child_copy_2();
            exit(0);
        } else if (pid > 0) {
            child_2_running = true;
        }
#endif
    } else {
        write_log("Child processes are still running. Skipping spawn.");
    }
}

void signal_handler(int signal) {
    terminate_program = true;
}

int main() {
    std::signal(SIGINT, signal_handler);
    write_log("Master Process [PID: " + std::to_string(getpid()) + "] " +
              "Started at: " + get_current_time());

    std::thread timer_thread(timer_function);
    std::thread log_thread;

    if (is_master) {
        log_thread = std::thread(log_master_function);
    }

    int spawn_timer = 0;
    while (!terminate_program) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        spawn_timer++;

        if (spawn_timer % 3 == 0 && is_master) {
            spawn_children();
        }
    }

    if (timer_thread.joinable()) {
        timer_thread.join();
    }
    if (log_thread.joinable()) {
        log_thread.join();
    }

    write_log("Master Process [PID: " + std::to_string(getpid()) + "] " +
              "Terminated at: " + get_current_time());
    return 0;
}
