//===--- main.cpp ---------------------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include "defines.h"
#include "runner.h"

#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

using namespace word_counter;
using namespace std::literals;

std::mutex words_set_mutex;
std::mutex input_file_mutex;



// NOLINTNEXTLINE
int main(int argc, char* argv[])
{
    try {
        if (argc < 2) {
            throw std::string{"File path not specified"};
        }
        std::string f_path{argv[1]};

        Runner runner{get_runner(Runner_version::V3)};
        if (argc >= 3) {
            std::string runner_version{argv[2]};
            if (!runner_version.compare("v1")) {
                runner = get_runner(Runner_version::V1);
            } else if (!runner_version.compare("v2")) {
                runner = get_runner(Runner_version::V2);
            } else if (!runner_version.compare("v3")) {
                runner = get_runner(Runner_version::V3);
            }
        }

        runner(f_path);
    } catch (std::string& e) {
        std::cout << e << std::endl;
    }

    return 0;
}
