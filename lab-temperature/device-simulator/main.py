import random
import time
import serial
import platform
import serial.tools.list_ports

DEFAULT_BAUDRATE = 9600

def get_default_port():
    """
    Автоматический выбор первого доступного последовательного порта.
    """
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        raise serial.SerialException("Не найдено доступных последовательных портов.")
    for port in ports:
        print(f"Найден порт: {port.device}")
    return ports[0].device  # Возвращает первый найденный порт


def simulate_device_serial(port=None, baudrate=DEFAULT_BAUDRATE):
    """
    Симуляция устройства, отправляющего данные температуры через последовательный порт.
    """
    if port is None:
        port = get_default_port()

    try:
        print(f"Подключаемся к порту {port}")
        # Открытие порта
        with serial.Serial(port, baudrate, timeout=1, exclusive=False) as ser:
            print(f"Устройство подключено к порту {port} с baudrate {baudrate}")
            while True:
                # Генерация случайной температуры
                temperature = round(15 + random.uniform(0, 10), 2)
                # Отправка данных через порт
                message = f"{temperature}\n"
                ser.write(message.encode())
                print(f"Отправлено: {message.strip()}")
                time.sleep(10)
    except serial.SerialException as e:
        print(f"Ошибка последовательного порта: {e}")
    except Exception as e:
        print(f"Ошибка симуляции устройства: {e}")


if __name__ == "__main__":
    baudrate = DEFAULT_BAUDRATE
    try:
        print(f"Определение порта на платформе {platform.system()}")
        simulate_device_serial(baudrate=baudrate)
    except Exception as e:
        print(f"Ошибка: {e}")
