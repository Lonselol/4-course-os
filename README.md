# 4-course-os

####Сборка
Собирается текущая открытая в редакторе лабораторная - launch.json в VSCode запускает build из tasks.json, который, в зависимости от os запускает скрипт build.
Build в качестве аргумента принимает текущую лабу (текущую рабочую директорию) и передаёт её в основной CMakeLists.txt, который добавляет её как subfolder.

####1 лабораторная (lab-helloworld)
Сборка и отладка проекта "HelloWorld" на cpp. Для проверки работы терминала VSCode вывод сообщения был обёрнут в бесконечный цикл.

####2 лабораторная (lab-lib)
process_launcher:
В зависимости от ОС запускается процесс (его имя приходит как аргумент) и возвращает код ответа.
main.cpp:
Утиита для проверки работы библиотеки process_launcher. Передаёт в метод run_process() название процесса, которое указывается при запуске исполняемого файла через терминал. Если не указать имя, то будет выбран дефолтный процесс.

####3 лабораторная (lab-multiple-log)

####4 лабораторная ()

####5 лабораторная ()

####6 лабораторная ()

####7 лабораторная ()

