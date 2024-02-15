//===--- file.h - file handler --------------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#ifndef WORD_COUNTER_FILE_H
#define WORD_COUNTER_FILE_H

#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>

namespace word_counter {

auto open_fstream(std::string_view path) -> std::ifstream;
auto open_fd(std::string_view path, int mode = O_RDONLY) -> int;
auto get_stat(int fd) -> struct stat;

} // namespace word_counter

#endif // WORD_COUNTER_FILE_H
