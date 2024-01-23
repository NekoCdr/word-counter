//===--- counter.h - words handler ----------------------------------------===//
//
// Copyright (c) 2024 Yuri Istomin
//
// Part of the NekoCdr's "word-counter" project
// Licensed under the Apache License, Version 2.0 (the "License").
// See http://www.apache.org/licenses/LICENSE-2.0 for license information.
//
//===----------------------------------------------------------------------===//

#ifndef WORD_COUNTER_COUNTER_H
#define WORD_COUNTER_COUNTER_H

#include "defines.h"

#include <fstream>

namespace word_counter {

auto handle_chunk(std::ifstream* input_file, Words_set* haystack) -> void;

} // namespace word_counter

#endif // WORD_COUNTER_COUNTER_H
