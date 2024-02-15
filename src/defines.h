//===--- defines.h - word-counter global defines & constants --------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#ifndef WORD_COUNTER_DEFINES_H
#define WORD_COUNTER_DEFINES_H

#include <absl/container/flat_hash_set.h>

#include <string>
#include <unordered_set>

namespace word_counter {

using Words_set = std::unordered_set<std::string>;
using Words_absl_set = absl::flat_hash_set<std::string>;
using Runner = auto (*)(std::string_view) -> void;
inline constexpr const int ErrorCode{-1};
inline constexpr const int ChunkSizeTrigger{4000000}; // 4 MB
inline constexpr const char SpaceChar{0x20};
inline constexpr const char EOLChar{0x0a};


} // namespace word_counter

#endif // WORD_COUNTER_DEFINES_H
