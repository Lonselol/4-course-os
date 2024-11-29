#include "process_launcher.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    int exit_code;
    int result;

    std::string program;
    std::vector<std::string> args;

    if (argc < 2) {
        #ifdef _WIN32
            program = "notepad.exe";
        #else
            program = "/bin/ls";
            args.push_back("-l");
        #endif

        std::cout << "No program specified, using default: " << program << "\n";
        result = process_launcher::run_process(program, args, exit_code);
    } else {
        program = argv[1];
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
