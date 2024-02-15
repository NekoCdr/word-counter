//===--- runner.h - words counter runner ----------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#ifndef WORD_COUNTER_RUNNER_H
#define WORD_COUNTER_RUNNER_H

#include "defines.h"

#include <string_view>

namespace word_counter {

enum class Runner_version: int {
    V1,
    V2,
    V3,
};

auto get_runner(Runner_version rv) -> Runner;

auto run_v1(std::string_view f_path) -> void;
auto run_v2(std::string_view f_path) -> void;
auto run_v3(std::string_view f_path) -> void;

} // namespace word_counter

#endif // WORD_COUNTER_RUNNER_H
