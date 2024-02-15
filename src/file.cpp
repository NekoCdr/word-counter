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

#include "defines.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <sys/stat.h>

namespace word_counter {

auto open_fstream(std::string_view path) -> std::ifstream
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

auto open_fd(std::string_view path, int mode) -> int
{
    if (!std::filesystem::exists(path)) {
        throw std::string{"File not exists"};
    }
    if (!std::filesystem::is_regular_file(path)) {
        throw std::string{"File is not regular"};
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    int fd{open(path.data(), mode)};

    if (fd == ErrorCode) {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        throw std::string{std::format("File is not open: {}", strerror(errno))};
    }

    return fd;
}

auto get_stat(int fd) -> struct stat
{
    struct stat fs{};
    fstat(fd, &fs);
    return fs;
}

} // namespace word_counter
