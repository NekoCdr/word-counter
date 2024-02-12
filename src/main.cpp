//===--- main.cpp ---------------------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include "counter.h"
#include "defines.h"
#include "file.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace word_counter;

std::mutex words_set_mutex;
std::mutex input_file_mutex;

// NOLINTNEXTLINE
int main(int argc, char* argv[])
{
    Words_set unique_words{};

    try {
        if (argc < 2) {
            throw std::string{"File path not specified"};
        }
        std::string f_path{argv[1]};

        std::ifstream input_file{word_counter::open_file(f_path)};
        uintmax_t file_size{std::filesystem::file_size(f_path)};

        auto chunk_quantity = static_cast<uintmax_t>(std::ceil(
            static_cast<float>(file_size) / ChunkSizeTrigger
        ));

        int threads_quantity{static_cast<int>(std::min(
            static_cast<uintmax_t>(std::thread::hardware_concurrency()),
            chunk_quantity
        ))};

        std::vector<std::unique_ptr<std::thread>> threads;
        threads.reserve(static_cast<size_t>(threads_quantity));

        for (int i=0; i<threads_quantity; i++) {
            threads.emplace_back(std::make_unique<std::thread>(
                handle_chunk,
                &input_file,
                &unique_words
            ));
        }

        for (auto& th : threads) {
            th->join();
        }

        input_file.close();

        std::cout << unique_words.size() << std::endl;
    } catch (std::string& e) {
        std::cout << e << std::endl;
    }

    return 0;
}
