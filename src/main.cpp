//===--- main.cpp ---------------------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

constexpr const int ChunkSizeTrigger{4000000}; // 4 MB
constexpr const int ThreadsMax{12};

using Words_set = std::unordered_set<std::string>;

static std::shared_mutex words_set_mutex;
static std::mutex input_file_mutex;

auto check_word(Words_set* haystack, std::string* needle)
{
    std::shared_lock<std::shared_mutex> haystack_lock_shared{words_set_mutex};
    if (!haystack->contains(*needle)) {
        haystack_lock_shared.unlock();
        std::unique_lock<std::shared_mutex> haystack_lock_unique{words_set_mutex};
        haystack->insert(*needle);
    }
}

auto handle_chunk(std::ifstream* input_file, Words_set* haystack) -> void
{
    Words_set unique_words{};

    while (!input_file->eof()) {
        int chunk_size{0};
        std::vector<std::string> chunk_words{};

        std::unique_lock<std::mutex> file_lock{input_file_mutex};
        while (!input_file->eof() && chunk_size < ChunkSizeTrigger) {
            std::string buff;
            *input_file >> buff;
            chunk_words.push_back(buff);
            chunk_size += static_cast<int>(buff.size() + 1);
        }
        file_lock.unlock();

        for (std::string word : chunk_words) {
            check_word(haystack, &word);
        }
    }
}

// NOLINTNEXTLINE
int main(int argc, char* argv[])
{
    Words_set unique_words{};

    try {
        if (argc < 2) {
            throw std::string{"File path not specified"};
        }
        std::string f_path{argv[1]};

        if (!std::filesystem::exists(f_path)) {
            throw std::string{"File not exists"};
        }
        if (!std::filesystem::is_regular_file(f_path)) {
            throw std::string{"File is not regular"};
        }

        auto file_size{std::filesystem::file_size(f_path)};
        std::ifstream input_file{f_path};

        if (!input_file.is_open()) {
            throw std::string{"File is not open"};
        }

        int threads_quantity{std::min(
            ThreadsMax,
            static_cast<int>(std::ceil(file_size / ChunkSizeTrigger))
        )};

        std::vector<std::unique_ptr<std::thread>> threads;
        threads.reserve(threads_quantity);

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

        std::cout << unique_words.size() << std::endl;
    } catch (std::string& e) {
        std::cout << e << std::endl;
    }

    return 0;
}
