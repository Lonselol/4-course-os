#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#ifdef _WIN32
#define DEFAULT_PORT "COM3" // Windows
#else
#define DEFAULT_PORT "/dev/ttyUSB0" // Linux
#endif
#define DEFAULT_BAUDRATE 9600

using namespace boost::asio;

void read_temperature(const std::string &port_name, unsigned int baud_rate) {
    try {
        io_service io;
        serial_port serial(io, port_name);

        // Настройка порта
        serial.set_option(serial_port_base::baud_rate(baud_rate));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

        std::cout << "Чтение данных с порта: " << port_name << " со скоростью: " << baud_rate << " бод" << std::endl;

        char buffer[128];
        while (true) {
            boost::system::error_code ec;
            std::size_t len = read(serial,  boost::asio::buffer(buffer, sizeof(buffer) - 1), ec);

            if (ec) {
                std::cerr << "Ошибка чтения: " << ec.message() << std::endl;
                break;
            }

            buffer[len] = '\0';
            std::cout << "Полученные данные: " << buffer << std::endl;

        }
    } catch (const boost::system::system_error &e) {
        std::cerr << "Ошибка последовательного порта: " << e.what() << std::endl;
    }
}

int main() {
    std::string port_name = DEFAULT_PORT;
    unsigned int baud_rate = DEFAULT_BAUDRATE;

    std::cout << "Введите имя порта (по умолчанию: " << DEFAULT_PORT << "): ";
    std::getline(std::cin, port_name);
    if (port_name.empty()) {
        port_name = DEFAULT_PORT;
    }

    std::cout << "Введите скорость (по умолчанию: 9600): ";
    std::string baud_rate_str;
    std::getline(std::cin, baud_rate_str);
    if (!baud_rate_str.empty()) {
        baud_rate = std::stoi(baud_rate_str);
    }

    read_temperature(port_name, baud_rate);

    return 0;
}
