#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <sqlite3.h>
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

// Класс для работы с последовательным портом
class SerialPort {
public:
    SerialPort(const std::string& port) : portName(port) {
    #ifdef _WIN32
        hSerial = CreateFile(portName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cout << "Error opening serial port\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cout << "Error getting serial port state\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cout << "Error setting serial port state\n";
            std::cout << "press any key to exit...";
            getch();
            exit(1);
        }
    #else
        fd = open(portName.c_str(), O_RDONLY | O_NOCTTY);
        if (fd < 0) {
            std::cout << "Error opening serial port\n";
            std::cout << "press any key to exit...";
            getchar();
            exit(1);
        }
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0) {
            std::cout << "Error getting serial port attributes\n";
            std::cout << "press any key to exit...";
            getchar();
            exit(1);
        }
        cfsetispeed(&tty, B9600);
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            std::cout << "Error setting serial port attributes\n";
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

// Класс для работы с базой данных SQLite
class Database {
public:
    Database(const std::string& dbName) {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            std::cout << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            exit(1);
        }
        createTables();
    }

    ~Database() {
        sqlite3_close(db);
    }

    void logTemperature(int temperature) {
        std::string query = "INSERT INTO temperatures (temperature) VALUES (" + std::to_string(temperature) + ");";
        executeQuery(query);
    }

    void logHourlyAvg(int avgTemp) {
        std::string query = "INSERT INTO hourly_avg (avg_temp) VALUES (" + std::to_string(avgTemp) + ");";
        executeQuery(query);
    }

    void logDailyAvg(int avgTemp) {
        std::string query = "INSERT INTO daily_avg (avg_temp) VALUES (" + std::to_string(avgTemp) + ");";
        executeQuery(query);
    }

    std::vector<int> getTemperatures(const std::string& startTime, const std::string& endTime) {
        std::string formattedStartTime = formatTime(startTime);
        std::string formattedEndTime = formatTime(endTime);
        std::cout<<"GettingTemperatureFromDB"<<std::endl;
        std::string query = "SELECT temperature FROM temperatures WHERE timestamp BETWEEN " + formattedStartTime + " AND " + formattedEndTime + ";";
        return executeSelectQuery(query);
    }

    std::vector<int> getHourlyAvg(const std::string& startTime, const std::string& endTime) {
        std::string formattedStartTime = formatTime(startTime);
        std::string formattedEndTime = formatTime(endTime);
        std::string query = "SELECT avg_temp FROM hourly_avg WHERE timestamp BETWEEN " + formattedStartTime + " AND " + formattedEndTime + ";";
        return executeSelectQuery(query);
    }

    std::vector<int> getDailyAvg(const std::string& startTime, const std::string& endTime) {
        std::string formattedStartTime = formatTime(startTime);
        std::string formattedEndTime = formatTime(endTime);
        std::string query = "SELECT avg_temp FROM daily_avg WHERE timestamp BETWEEN " + formattedStartTime + " AND " + formattedEndTime + ";";
        return executeSelectQuery(query);
    }

private:
    sqlite3* db;

    void createTables() {
        const char* createTemperaturesTable = "CREATE TABLE IF NOT EXISTS temperatures (id INTEGER PRIMARY KEY AUTOINCREMENT, temperature INTEGER, timestamp DATETIME DEFAULT (datetime('now', 'localtime')));";
        const char* createHourlyAvgTable = "CREATE TABLE IF NOT EXISTS hourly_avg (id INTEGER PRIMARY KEY AUTOINCREMENT, avg_temp INTEGER, timestamp DATETIME DEFAULT (datetime('now', 'localtime')));";
        const char* createDailyAvgTable = "CREATE TABLE IF NOT EXISTS daily_avg (id INTEGER PRIMARY KEY AUTOINCREMENT, avg_temp INTEGER, timestamp DATETIME DEFAULT (datetime('now', 'localtime')));";

        executeQuery(createTemperaturesTable);
        executeQuery(createHourlyAvgTable);
        executeQuery(createDailyAvgTable);
    }

    void executeQuery(const std::string& query) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cout << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    std::vector<int> executeSelectQuery(const std::string& query) {
        std::cout << "Executing query: " << query << std::endl;
        std::vector<int> results;
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                results.push_back(sqlite3_column_int(stmt, 0));
            }
            sqlite3_finalize(stmt);
        } else {
            std::cout << "Failed to execute query: " << query << std::endl;
        }
        return results;
    }

    std::string formatTime(const std::string& time) {
        if (time == "now") {
            return "datetime('now', 'localtime')";  // Текущее время с учётом локального часового пояса
        }
        // Заменяем 'T' на пробел и добавляем секунды, если их нет
        std::string formattedTime = time;
        size_t tPos = formattedTime.find('T');
        if (tPos != std::string::npos) {
            formattedTime.replace(tPos, 1, " ");
        }
        if (formattedTime.length() == 16) {  // Если время в формате "YYYY-MM-DD HH:MM"
            formattedTime += ":00";  // Добавляем секунды
        }
        return '"' + formattedTime + '"';
    }
};

// Класс для HTTP-сервера
class HttpServer {
public:
    HttpServer(int port, Database& db) : port(port), db(db) {
        #ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "WSAStartup failed\n";
                exit(1);
            }
        #endif
    }

    ~HttpServer() {
        #ifdef _WIN32
            WSACleanup();
        #endif
    }

    void start() {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cout << "Error creating socket\n";
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cout << "Error binding socket\n";
            closeSocket(serverSocket);
            return;
        }

        if (listen(serverSocket, 5) < 0) {
            std::cout << "Error listening on socket\n";
            closeSocket(serverSocket);
            return;
        }

        std::cout << "Server started on port " << port << std::endl;

        while (true) {
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
            if (clientSocket < 0) {
                std::cout << "Error accepting connection\n";
                continue;
            }

            handleRequest(clientSocket);
            closeSocket(clientSocket);
        }
    }

private:
    int port;
    Database& db;

    void closeSocket(int socket) {
        #ifdef _WIN32
            closesocket(socket);
        #else
            close(socket);
        #endif
    }

    void handleRequest(int clientSocket) {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0) {
            std::cout << "Error reading from socket\n";
            return;
        }

        std::string request(buffer, bytesRead);
        std::string response;

        // Добавляем CORS-заголовки
        std::string headers = "HTTP/1.1 200 OK\r\n";
        headers += "Access-Control-Allow-Origin: *\r\n";  // Разрешаем доступ с любого источника
        headers += "Content-Type: application/json\r\n\r\n";  // Указываем тип содержимого

        if (request.find("GET /current") != std::string::npos) {
            auto temps = db.getTemperatures("now", "now");
            if (!temps.empty()) {
                response = headers + "{\"temperature\": " + std::to_string(temps.back()) + "}";
            } else {
                response = headers + "{\"error\": \"No data\"}";
            }
        } else if (request.find("GET /stats") != std::string::npos) {
            std::string startTime = extractParam(request, "start");
            std::string endTime = extractParam(request, "end");
            auto temps = db.getTemperatures(startTime, endTime);
            auto hourlyAvgs = db.getHourlyAvg(startTime, endTime);
            auto dailyAvgs = db.getDailyAvg(startTime, endTime);

            std::ostringstream json;
            json << "{\"temperatures\": [";
            for (size_t i = 0; i < temps.size(); ++i) {
                json << temps[i];
                if (i < temps.size() - 1) json << ", ";
            }
            json << "], \"hourly_avg\": [";
            for (size_t i = 0; i < hourlyAvgs.size(); ++i) {
                json << hourlyAvgs[i];
                if (i < hourlyAvgs.size() - 1) json << ", ";
            }
            json << "], \"daily_avg\": [";
            for (size_t i = 0; i < dailyAvgs.size(); ++i) {
                json << dailyAvgs[i];
                if (i < dailyAvgs.size() - 1) json << ", ";
            }
            json << "]}";
            response = headers + json.str();
        } else {
            response = headers + "{\"error\": \"Not Found\"}";
        }

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    std::string extractParam(const std::string& request, const std::string& param) {
        size_t start = request.find(param + "=");
        if (start == std::string::npos) return "";
        start += param.size() + 1;
        size_t end = request.find("&", start);
        if (end == std::string::npos) end = request.find(" ", start);
        return request.substr(start, end - start);
    }
};

int main() {
    std::string port;
    std::cout << "Enter port name: ";
    std::getline(std::cin, port);
    if (port.empty()) {
        #ifdef _WIN32
            port = "COM2";
        #else
            port = "/dev/pts/2";
        #endif
    }
    std::cout << "Port is " << port << std::endl;

    SerialPort serial(port);
    Database db("temperatures.db");

    // Запуск HTTP-сервера в отдельном потоке
    HttpServer httpServer(8080, db);
    std::thread serverThread([&httpServer]() {
        httpServer.start();
    });

    std::vector<int> hourlyTemps;
    std::vector<int> dailyTemps;
    auto lastHour = std::chrono::system_clock::now();
    auto lastDay = std::chrono::system_clock::now();

    while (true) {
        std::string data = serial.read();
        if (!data.empty()) {
            int temperature = std::stoi(data);
            db.logTemperature(temperature);
            hourlyTemps.push_back(temperature);
            dailyTemps.push_back(temperature);

            std::cout<<temperature<<std::endl;

            auto now = std::chrono::system_clock::now();
            if (now - lastHour >= std::chrono::hours(1)) {
                int hourlyAvg = 0;
                for (int temp : hourlyTemps) {
                    hourlyAvg += temp;
                }
                hourlyAvg /= hourlyTemps.size();
                db.logHourlyAvg(hourlyAvg);
                hourlyTemps.clear();
                lastHour = now;
            }

            if (now - lastDay >= std::chrono::hours(24)) {
                int dailyAvg = 0;
                for (int temp : dailyTemps) {
                    dailyAvg += temp;
                }
                dailyAvg /= dailyTemps.size();
                db.logDailyAvg(dailyAvg);
                dailyTemps.clear();
                lastDay = now;
            }
        } else {
            std::cout << "Empty data" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    serverThread.join();
    return 0;
}
