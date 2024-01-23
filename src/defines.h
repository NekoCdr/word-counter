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

#include <string>
#include <unordered_set>

namespace word_counter {

using Words_set = std::unordered_set<std::string>;
inline constexpr const int ChunkSizeTrigger{4000000}; // 4 MB

} // namespace word_counter

#endif // WORD_COUNTER_DEFINES_H
