//===--- main.cpp ---------------------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

// NOLINTNEXTLINE
int main()
{
    std::string f_path{"./test.txt"};
    std::unordered_set<std::string> words{};

    try {

        if (!std::filesystem::exists(f_path)) {
            throw std::string{"File not exists"};
        }
        if (!std::filesystem::is_regular_file(f_path)) {
            throw std::string{"File is not regular"};
        }

        std::ifstream input_file{f_path};

        if (!input_file.is_open()) {
            throw std::string{"File is not open"};
        }

        while (!input_file.eof()) {
            std::string buff;
            input_file >> buff;
            if (!words.contains(buff)) {
                words.insert(buff);
            }
        }

        std::cout << words.size() << std::endl;
    } catch (std::string& e) {
        std::cout << e << std::endl;
    }

    return 0;
}
