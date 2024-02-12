//===--- counter.cpp - words handler --------------------------------------===//
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

#include <deque>
#include <mutex>
#include <string>

extern std::mutex words_set_mutex;
extern std::mutex input_file_mutex;

namespace word_counter {

auto handle_chunk(std::ifstream* input_file, Words_set* haystack) -> void
{
    Words_set unique_words{};

    while (!input_file->eof()) {
        int chunk_size{0};
        std::deque<std::string> chunk_words{};

        std::unique_lock<std::mutex> file_lock{input_file_mutex};
        while (!input_file->eof() && chunk_size < ChunkSizeTrigger) {
            std::string buff;
            *input_file >> buff;
            if (!buff.empty()) {
                chunk_words.push_back(buff);
                chunk_size += static_cast<int>(buff.size() + 1);
            }
        }
        file_lock.unlock();

        unique_words.insert(chunk_words.begin(), chunk_words.end());
    }

    std::unique_lock<std::mutex> haystack_lock_unique{words_set_mutex};
    haystack->merge(unique_words);
}

} // namespace word_counter
