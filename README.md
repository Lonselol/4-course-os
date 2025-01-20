# 4-course-os

### Сборка
Собирается текущая открытая в редакторе лабораторная - launch.json в VSCode запускает build из tasks.json, который, в зависимости от os запускает скрипт build.
Build в качестве аргумента принимает текущую лабу (текущую рабочую директорию) и передаёт её в основной CMakeLists.txt, который добавляет её как subfolder.

### 1 лабораторная (lab-helloworld)
Сборка и отладка проекта "HelloWorld" на cpp. Для проверки работы терминала VSCode вывод сообщения был обёрнут в бесконечный цикл.

### 2 лабораторная (lab-lib)
process_launcher:
В зависимости от ОС запускается процесс (его имя приходит как аргумент) и возвращает код ответа.
main.cpp:
Утилита для проверки работы библиотеки process_launcher. Передаёт в метод run_process() название процесса, которое указывается при запуске исполняемого файла через терминал. Если не указать имя, то будет выбран дефолтный процесс.

### 3 лабораторная (lab-multiple-log)
Исполняемый файл можно запускать несколько раз. Каждый из них запускает основной процесс, который порождает два дочерних каждые 3 секунды (если они есть, то не порождает). Они записывают в лог значение счетчика (общий для всех экземпляров программы). Только программа, обладающая флагом is_master может создавать копии и делает записи в лог.
Также пользователь в терминале может задавать любое значение счетчика. Если закрыть мастера, то какой-нибудь другой экземпляр им станет.

### 4 лабораторная (lab-temperature)
Симуляция устройства - device_simulator.cpp
Чтение - lab_temperature.cpp
В обеих программах вводится имя порта:
Тестировалось на эмулированных COM1 и COM2 для Windows.
На Linux
```
sudo apt install socat
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```
для создания /dev/pts/2 и /dev/pts/3, с которыми проводилось тестирование.

### 5 лабораторная ()
За основу был взят код предыдущей лабораторной. Добавлена запись в базу данных (SQLite).

Для сборки SQLite на Windows:
```
gcc -c sqlite3.c -o sqlite3.o
ar rcs libsqlite3.a sqlite3.o
```

Температура записывается с текущим локальным временем.

```
npm install -g http-server
http-server
```
Для запуска frontend приложения. Отображает график за выбранный промежуток времени.
### 6 лабораторная ()

### 7 лабораторная ()

