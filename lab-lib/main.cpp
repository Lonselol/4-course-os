#include "process_launcher.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int exit_code;
    int result;

    if (argc < 2) {
        std::string program =
        #ifdef _WIN32
            "notepad.exe";
        #else
            "/bin/ls";
        #endif
        std::cout << "No program specified, using default: " << program << "\n";
        result = process_launcher::run_process(program, {}, exit_code);
    } else {
        std::string program = argv[1];
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        result = process_launcher::run_process(program, args, exit_code);
    }

    if (result == 0) {
        std::cout << "Program finished with exit code: " << exit_code << std::endl;
    } else {
        std::cerr << "Failed to run program." << std::endl;
    }

    return result;
}
