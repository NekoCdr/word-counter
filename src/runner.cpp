//===--- runner.cpp - words counter runner --------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include "runner.h"

#include "counter.h"
#include "defines.h"
#include "file.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

extern std::mutex words_set_mutex;
extern std::mutex input_file_mutex;

namespace word_counter {

auto get_runner(
    Runner_version rv
) -> Runner {
    switch (rv) {
        case Runner_version::V1:
            return &run_v1;
        case Runner_version::V2:
            return &run_v2;
        case Runner_version::V3:
            return &run_v3;
    }
}

auto run_v1(std::string_view f_path) -> void
{
    Words_set unique_words{};

    std::ifstream input_file{word_counter::open_fstream(f_path)};
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
            static_cast<void (&)(std::ifstream*, Words_set*)>(handle_chunk),
            &input_file,
            &unique_words
        ));
    }

    for (auto& th : threads) {
        th->join();
    }

    input_file.close();

    std::cout << "unique_words in file: " << unique_words.size() << std::endl;
}

auto run_v2(std::string_view f_path) -> void
{
    Words_set unique_words{};

    std::ifstream input_file{word_counter::open_fstream(f_path)};
    uintmax_t file_size{std::filesystem::file_size(f_path)};

    unsigned int threads_quantity{
        file_size > ChunkSizeTrigger
            ? std::thread::hardware_concurrency()
            : 1
    };
    uintmax_t chunk_size_trigger{static_cast<uintmax_t>(std::ceil(
        static_cast<double>(file_size) / threads_quantity
    ))};

    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(static_cast<size_t>(threads_quantity));

    uintmax_t start_input_position{0};
    uintmax_t end_input_position{chunk_size_trigger};
    for (unsigned int i{0}; i < threads_quantity; i++) {
        if (end_input_position > file_size) {
            end_input_position = file_size;
        }
        input_file.seekg(static_cast<long>(end_input_position));
        std::string buff(1, '\0');
        input_file.read(&buff[0], 1);

        if (buff.compare(" ") && !input_file.eof()) {
            std::string s;
            input_file >> s;
            end_input_position += s.size();
        }

        std::ifstream fd{word_counter::open_fstream(f_path)};

        threads.emplace_back(std::make_unique<std::thread>(
            static_cast<void (&)(Words_set*, std::ifstream, uintmax_t, uintmax_t)>(handle_chunk),
            &unique_words,
            std::move(fd),
            start_input_position,
            end_input_position
        ));

        if (input_file.eof()) {
            break;
        }
        start_input_position = end_input_position;
        end_input_position += chunk_size_trigger;
    }

    input_file.close();

    for (auto& th : threads) {
        th->join();
    }

    std::cout << "unique_words in file: " << unique_words.size() << std::endl;
}

auto run_v3(std::string_view f_path) -> void
{
    Words_absl_set unique_words{};

    int file_descriptor{open_fd(f_path)};
    struct stat file_stat{get_stat(file_descriptor)};
    
    unsigned int threads_quantity{
        file_stat.st_size > ChunkSizeTrigger
            ? std::thread::hardware_concurrency()
            : 1
    };
    intmax_t chunk_trigger{static_cast<intmax_t>(std::ceil(
        static_cast<double>(file_stat.st_size) / threads_quantity
    ))};

    std::vector<std::unique_ptr<std::thread>> threads;
    threads.reserve(static_cast<size_t>(threads_quantity));

    char* f_stream{static_cast<char*>(mmap(
        nullptr,
        static_cast<size_t>(file_stat.st_size),
        PROT_READ,
        MAP_PRIVATE,
        file_descriptor,
        0
    ))};
    if (f_stream == MAP_FAILED) {
        throw std::string{
            // NOLINTNEXTLINE(concurrency-mt-unsafe)
            std::format("mmap: {}", strerror(errno))
        };
    }

    intmax_t last_byte_index{file_stat.st_size-1};
    intmax_t start_index{0};
    intmax_t end_index{chunk_trigger-1};

    for (unsigned int i=0; i<threads_quantity; i++) {
        if (end_index > last_byte_index) {
            end_index = last_byte_index;
        }

        while (end_index < last_byte_index) {
            if (
                f_stream[end_index] == SpaceChar
                || f_stream[end_index] == EOLChar
            ) {
                break;
            } else {
                end_index++;
            }
        }

        threads.emplace_back(std::make_unique<std::thread>(
            static_cast<void (&)(Words_absl_set*, char* f_stream, intmax_t, intmax_t)>(handle_chunk),
            &unique_words,
            f_stream,
            start_index,
            end_index
        ));

        start_index = end_index+1;
        end_index += chunk_trigger;
    }

    for (auto& th : threads) {
        th->join();
    }

    munmap(f_stream, static_cast<size_t>(file_stat.st_size));
    close(file_descriptor);

    std::cout << "unique_words in file: " << unique_words.size() << std::endl;
}

} // namespace word_counter
