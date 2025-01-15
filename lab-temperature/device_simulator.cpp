#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstdio>
#include <cstring>
#endif

class SerialPort {
public:
    SerialPort(const std::string& port) : portName(port) {
    #ifdef _WIN32
        hSerial = CreateFile(portName.c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
        fd = open(portName.c_str(), O_WRONLY | O_NOCTTY);
        if (fd < 0) {
            std::cerr << "Error opening serial port\n";
            std::cout << "press any key to exit...";
            getchar();
            exit(1);
        }
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) {
            std::cerr << "Error getting serial port attributes\n";
            std::cout << "press any key to exit...";
            getchar();
            exit(1);
        }
        cfsetospeed(&tty, B9600);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            std::cerr << "Error setting serial port attributes\n";
            std::cout << "press any key to exit...";
            getchar();
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

    void write(const std::string& data) {
        char buffer[256];
        memcpy(buffer, data.c_str(), sizeof(buffer));
        #ifdef _WIN32
            DWORD bytesWritten;
            WriteFile(hSerial, buffer, sizeof(buffer), &bytesWritten, NULL);
        #else
            ::write(fd, buffer, sizeof(buffer));
        #endif
    }


private:
        std::string portName;
    #ifdef _WIN32
        HANDLE hSerial;
    #else
        int fd;
    #endif
};

int main() {
    std::srand(std::time(0));
    std::string port;
    std::cout << "Enter port name: ";
    std::getline(std::cin, port);
    if (port.empty()){
        #ifdef _WIN32
                port = "COM1";
        #else
                port = "/dev/pts/3";
        #endif
    }
    std::cout<<"port is "<<port<<std::endl;
    SerialPort serial(port);
    while (true) {
        int temperature = std::rand() % 50; // Генерация случайной температуры от 0 до 49
        std::string data = std::to_string(temperature) + "\n";
        serial.write(data);
        std::cout<<data;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Отправка каждую секунду
    }

    return 0;
}
