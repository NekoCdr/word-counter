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

#include <cstdint>
#include <deque>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

auto handle_chunk(
    Words_set* haystack,
    std::ifstream input_file,
    uintmax_t stream_begin, // NOLINT(bugprone-easily-swappable-parameters)
    uintmax_t stream_end
) -> void {
    Words_set unique_words{};

    input_file.seekg(static_cast<intmax_t>(stream_begin));

    intmax_t pos{static_cast<intmax_t>(stream_begin)};

    while (
        pos < static_cast<intmax_t>(stream_end)
        && !input_file.eof()
    ) {
        std::string buff;
        input_file >> buff;
        if (!buff.empty()) {
            unique_words.insert(buff);
            pos += static_cast<intmax_t>(buff.size()) + 1;
        } else {
            pos++;
        }
    }

    input_file.close();

    std::unique_lock<std::mutex> haystack_lock_unique{words_set_mutex};
    haystack->merge(unique_words);
}

auto handle_chunk(
    Words_absl_set* haystack,
    char* f_stream,
    intmax_t stream_begin, // NOLINT(bugprone-easily-swappable-parameters)
    intmax_t stream_end
) -> void {
    Words_absl_set unique_words{};

    std::string buff;
    buff.reserve(30);
    for (intmax_t i=stream_begin; i<=stream_end; i++) {
        char c{f_stream[i]};

        if (c != SpaceChar && c != EOLChar) {
            buff.push_back(c);
        }

        if (
            !buff.empty()
            && (c == SpaceChar || c == EOLChar || i == stream_end)
        ) {
            unique_words.insert(buff);
            buff.clear();
        }
    }

    std::unique_lock<std::mutex> haystack_lock_unique{words_set_mutex};
    haystack->merge(unique_words);
}

} // namespace word_counter
