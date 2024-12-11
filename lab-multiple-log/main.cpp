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
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#endif

std::atomic<bool> terminate_program(false);
std::mutex log_mutex;
std::string log_file = "log.txt";

// Общий счетчик для всех процессов
struct SharedCounter {
    std::atomic<int> counter;
    std::atomic<std::chrono::system_clock::time_point> last_active_time; // Время последней активности мастера
};

SharedCounter* shared_counter;

#if defined(_WIN32) || defined(_WIN64)
LPCSTR lock_file = "Global\\master.lock";
HANDLE master_semaphore;
#else
const char* lock_file = "/tmp/master.lock";
sem_t* master_semaphore;
#endif

// Запись лога
void write_log(const std::string &message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log(log_file, std::ios::app);
    if (log.is_open()) {
        log << message << std::endl;
    }
}

// Текущее время
std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return std::string(buf) + "." + std::to_string(ms.count());
}

// Проверка, является ли текущий экземпляр программы мастером
bool check_and_set_master() {
#if defined(_WIN32) || defined(_WIN64)
    master_semaphore = CreateSemaphore(NULL, 1, 1, lock_file);
    if (master_semaphore == NULL) {
        std::cerr << "Failed to create semaphore: " << GetLastError() << std::endl;
        return false;
    }
    if (WaitForSingleObject(master_semaphore, 0) == WAIT_TIMEOUT) {
        CloseHandle(master_semaphore);
        return false;
    }
    return true;
#else
    master_semaphore = sem_open(lock_file, O_CREAT | O_EXCL, 0644, 1);
    if (master_semaphore == SEM_FAILED) {
        std::cerr << "Failed to create semaphore: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
#endif
}

void release_master() {
#if defined(_WIN32) || defined(_WIN64)
    ReleaseSemaphore(master_semaphore, 1, NULL);
    CloseHandle(master_semaphore);
#else
    sem_close(master_semaphore);
    sem_unlink(lock_file);
#endif
}

//таймер для ожидания
void timer_function() {
    while (!terminate_program) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        shared_counter->counter++;
        if (shared_counter->counter.load() > 0) {
            shared_counter->last_active_time.store(std::chrono::system_clock::now()); // Обновляем время последней активности
        }
    }
}

//мастер пишет лог
void log_master_function() {
    while (!terminate_program) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        write_log("Master Process [PID: " + std::to_string(getpid()) + "] " +
                  "Time: " + get_current_time() + " Counter: " +
                  std::to_string(shared_counter->counter.load()));
    }
}

//ввод команды для изменения счетчика
void user_input_function() {
    while (!terminate_program) {
        std::string command;
        std::cout << "FROM [PID: " + std::to_string(getpid()) + "]: Enter command (set <value>): ";
        std::cin >> command;

        if (command == "set") {
            int value;
            std::cin >> value;
            shared_counter->counter = value;
            std::cout << "Counter set to " << value << std::endl;
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
}

//Дочерние процессы
void child_copy_1() {
    write_log("Child Copy 1 [PID: " + std::to_string(getpid()) + "] " +
              "Started at: " + get_current_time());
    shared_counter->counter += 10;
    write_log("Child Copy 1 [PID: " + std::to_string(getpid()) + "] " +
              "Exiting at: " + get_current_time());
}

void child_copy_2() {
    write_log("Child Copy 2 [PID: " + std::to_string(getpid()) + "] " +
              "Started at: " + get_current_time());
    shared_counter->counter = shared_counter->counter * 2;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    shared_counter->counter = shared_counter->counter / 2;
    write_log("Child Copy 2 [PID: " + std::to_string(getpid()) + "] " +
              "Exiting at: " + get_current_time());
}

//рожаем дочерние процессы от мастера
void spawn_children() {
    write_log("From [PID: " + std::to_string(getpid()) + "]: Spawning child processes...");
#if defined(_WIN32) || defined(_WIN64)
    HANDLE handle1 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)child_copy_1, nullptr, 0, nullptr);
    HANDLE handle2 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)child_copy_2, nullptr, 0, nullptr);
    CloseHandle(handle1);
    CloseHandle(handle2);
#else
    pid_t pid1 = fork();
    if (pid1 == 0) {
        child_copy_1();
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {
        child_copy_2();
        exit(0);
    }
#endif
}

void signal_handler(int signal) {
    terminate_program = true;
}

//Проверяем жив ли мастер-процесс
bool is_master_active() {
    auto now = std::chrono::system_clock::now();
    auto last_active = shared_counter->last_active_time.load();
    bool active = (now - last_active) < std::chrono::milliseconds(100);
    //write_log("Checking master activity: " + std::to_string(active));
    return active;
}

int main() {
    std::signal(SIGINT, signal_handler);

#if defined(_WIN32) || defined(_WIN64)
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedCounter), "Global\\SharedCounter");
    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
        return 1;
    }
    shared_counter = (SharedCounter*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedCounter));
    if (shared_counter == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }
    shared_counter->counter = 0;
    shared_counter->last_active_time = std::chrono::system_clock::now(); // Инициализация времени последней активности
#else
    int shm_fd = shm_open("/shared_counter", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedCounter));
    shared_counter = (SharedCounter*)mmap(0, sizeof(SharedCounter), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shared_counter->counter = 0;
    shared_counter->last_active_time = std::chrono::system_clock::now(); // Инициализация времени последней активности
#endif

    bool is_master = check_and_set_master();
    if (is_master) {
        write_log("Master Process [PID: " + std::to_string(getpid()) + "] " +
                  "Started at: " + get_current_time());
    } else {
        write_log("Worker Process [PID: " + std::to_string(getpid()) + "] " +
                  "Started at: " + get_current_time());
    }

    std::thread timer_thread(timer_function);
    std::thread log_thread;
    std::thread input_thread(user_input_function);

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

        // Проверка активности мастера
        if (!is_master_active() && !is_master) {
            // Если мастер не активен, пробуем стать мастером
            if (check_and_set_master()) {
                is_master = true;
                write_log("New Master Process [PID: " + std::to_string(getpid()) + "] " +
                          "Started at: " + get_current_time());
            }
        }
    }

    if (is_master) {
        release_master();
    }

    terminate_program = true;

    if (timer_thread.joinable()) {
        timer_thread.join();
    }
    if (log_thread.joinable()) {
        log_thread.join();
    }
    if (input_thread.joinable()) {
        input_thread.join();
    }

    write_log((is_master ? "Master" : "Worker") + std::string(" Process [PID: ") +
              std::to_string(getpid()) + "] Terminated at: " + get_current_time());

#if defined(_WIN32) || defined(_WIN64)
    UnmapViewOfFile(shared_counter);
    CloseHandle(hMapFile);
#else
    munmap(shared_counter, sizeof(SharedCounter));
    shm_unlink("/shared_counter");
#endif

    return 0;
}
