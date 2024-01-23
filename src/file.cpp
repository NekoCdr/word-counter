//===--- file.cpp - file handler ------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#include "file.h"

#include <filesystem>

namespace word_counter {

auto open_file(std::string_view path) -> std::ifstream
{
    if (!std::filesystem::exists(path)) {
        throw std::string{"File not exists"};
    }
    if (!std::filesystem::is_regular_file(path)) {
        throw std::string{"File is not regular"};
    }

    std::ifstream input_file{path.data()};

    if (!input_file.is_open()) {
        throw std::string{"File is not open"};
    }

    return input_file;
}

} // namespace word_counter
