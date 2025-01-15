#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstdio> 
#endif

class SerialPort {
public:
    SerialPort(const std::string& port) : portName(port) {
    #ifdef _WIN32
        hSerial = CreateFile(portName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening serial port\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error getting serial port state\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error setting serial port state\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
    #else
        fd = open(portName.c_str(), O_RDONLY | O_NOCTTY);
        if (fd < 0) {
            std::cerr << "Error opening serial port\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) {
            std::cerr << "Error getting serial port attributes\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        cfsetispeed(&tty, B9600);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            std::cerr << "Error setting serial port attributes\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
    #endif
    }

    ~SerialPort() {
        #ifdef _WIN32
            CloseHandle(hSerial);
        #else
            close(fd);
        #endif
    }

    std::string read() {
        char buffer[256];
        std::string data;
        #ifdef _WIN32
            DWORD bytesRead;
            ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL);
            data.assign(buffer, bytesRead);
        #else
            ssize_t bytesRead = ::read(fd, buffer, sizeof(buffer));
            data.assign(buffer, bytesRead);
        #endif
            return data;
    }

private:
    std::string portName;
    #ifdef _WIN32
        HANDLE hSerial;
    #else
        int fd;
    #endif
};

class Logger {
public:
    Logger(const std::string& logFile) : logFileName(logFile) {}

    void log(const std::string& message) {
        std::ofstream logFile(logFileName, std::ios::app);
        if (logFile.is_open()) {
            logFile << message << std::endl;
            std::cout << message << std::endl;
        }
    }

    void clearOldEntries(const std::string& logFile, int maxEntries) {
        std::ifstream inFile(logFile);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();

        if (lines.size() > maxEntries) {
            std::ofstream outFile(logFile);
            for (size_t i = lines.size() - maxEntries; i < lines.size(); ++i) {
                outFile << lines[i] << std::endl;
            }
        }
    }

private:
    std::string logFileName;
};

int main() {
    std::string port;
    std::cout << "End port name: ";
    std::getline(std::cin, port);
    if (port.empty()){
        #ifdef _WIN32
                port = "COM2";
        #else
                port = "/dev/ttyS1"
        #endif
    }
    std::cout<<"port is "<<port<<std::endl;
    SerialPort serial(port);

    Logger allTempsLogger("all_temps.txt");
    Logger hourlyAvgLogger("hourly_avg.txt");
    Logger dailyAvgLogger("daily_avg.txt");

    std::vector<int> hourlyTemps;
    std::vector<int> dailyTemps;
    auto lastHour = std::chrono::system_clock::now();
    auto lastDay = std::chrono::system_clock::now();

    while (true) {
        std::string data = serial.read();
        if (!data.empty()) {
            int temperature = std::stoi(data);
            allTempsLogger.log(std::to_string(temperature));
            hourlyTemps.push_back(temperature);
            dailyTemps.push_back(temperature);

            auto now = std::chrono::system_clock::now();
            if (now - lastHour >= std::chrono::hours(1)) {
                int hourlyAvg = 0;
                for (int temp : hourlyTemps) {
                    hourlyAvg += temp;
                }
                hourlyAvg /= hourlyTemps.size();
                hourlyAvgLogger.log(std::to_string(hourlyAvg));
                hourlyTemps.clear();
                lastHour = now;

                hourlyAvgLogger.clearOldEntries("hourly_avg.log", 720); // 24 часа * 30 дней
            }

            if (now - lastDay >= std::chrono::hours(24)) {
                int dailyAvg = 0;
                for (int temp : dailyTemps) {
                    dailyAvg += temp;
                }
                dailyAvg /= dailyTemps.size();
                dailyAvgLogger.log(std::to_string(dailyAvg));
                dailyTemps.clear();
                lastDay = now;
            }

            allTempsLogger.clearOldEntries("all_temps.log", 86400); // 24 часа * 60 минут * 60 секунд
        }
        else{
            std::cout<<"Empty data"<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}