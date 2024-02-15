//===--- helper.cpp -------------------------------------------------------===//
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
#include <random>
#include <string>
#include <vector>

constexpr const long long OutputSize{35000000000};

// NOLINTNEXTLINE
int main()
{
    std::string text_path{"./test.txt"};
    std::string dictionary_path{"./words_alpha.txt"};
    
    std::vector<std::string> words{};
    long long output_size{0};
    long long words_in_text{0};

    try {

        if (!std::filesystem::exists(text_path)) {
            throw std::string{"Text file not exists"};
        }
        if (!std::filesystem::is_regular_file(text_path)) {
            throw std::string{"Text file is not regular"};
        }

        if (!std::filesystem::exists(dictionary_path)) {
            throw std::string{"Dictionary file not exists"};
        }
        if (!std::filesystem::is_regular_file(dictionary_path)) {
            throw std::string{"Dictionary file is not regular"};
        }

        std::ofstream text_file{text_path};
        std::ifstream dictionary_file{dictionary_path};

        if (!text_file.is_open()) {
            throw std::string{"Text file is not open"};
        }

        if (!dictionary_file.is_open()) {
            throw std::string{"Dictionary file is not open"};
        }

        while (!dictionary_file.eof()) {
            std::string buff;

            dictionary_file >> buff;
            if (!buff.empty()) {
                words.push_back(buff);
            }
        }

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(words.size() - 1));

        while (output_size < OutputSize) {
            std::string tmp{words[distrib(gen)]};
            output_size += static_cast<long long>(tmp.size());
            words_in_text++;
            text_file << tmp;
            if (output_size < OutputSize) {
                text_file << " ";
                output_size += 1;
            }
        }

        std::cout << output_size << std::endl;
        std::cout << words_in_text << std::endl;

    } catch (std::string& e) {
        std::cout << e << std::endl;
    }

    return 0;
}
