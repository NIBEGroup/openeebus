/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "src/common/string_util.h"

#include <stddef.h>

#include <gtest/gtest.h>

struct StringNCompareParams {
  const char* a;
  const char* b;
  size_t n;
  bool expected;
};

class StringNCompareTest : public ::testing::TestWithParam<StringNCompareParams> {};

TEST_P(StringNCompareTest, ReturnsExpected) {
  const StringNCompareParams& p = GetParam();
  EXPECT_EQ(StringNCompare(p.a, p.b, p.n), p.expected);
}

INSTANTIATE_TEST_SUITE_P(
    StringNCompare,
    StringNCompareTest,
    ::testing::Values(
        // --- Match ---

        // Both strings equal and shorter than n
        StringNCompareParams{"abc", "abc", 5, true},
        // Both strings equal and exactly n characters long
        StringNCompareParams{"abcde", "abcde", 5, true},
        // Both empty, n = 0
        StringNCompareParams{"", "", 0, true},
        // Both empty, n > 0
        StringNCompareParams{"", "", 5, true},
        // Both longer than n and identical through position n are rejected (null-terminator check fails)
        StringNCompareParams{"abcde!X", "abcde!Y", 5, false},

        // --- No match ---

        // Different content, both shorter than n
        StringNCompareParams{"abc", "axc", 5, false},
        // Different content at last position, both exactly n characters long
        StringNCompareParams{"abcde", "abcdx", 5, false},
        // a is a prefix of b (b has extra trailing characters), n = length of a
        StringNCompareParams{"abcde", "abcdefgh", 5, false},
        // b is a prefix of a (a has extra trailing characters), n = length of b
        StringNCompareParams{"abcdefgh", "abcde", 5, false},
        // One is a prefix of the other, n larger than both lengths
        StringNCompareParams{"abc", "abcde", 10, false},
        // Both strings are longer than n; rejected because neither has a NUL terminator at index n
        StringNCompareParams{"abcde!", "abcde?", 5, false},
        // a empty, b non-empty, n = 0
        StringNCompareParams{"", "a", 0, false},
        // a non-empty, b empty, n = 0
        StringNCompareParams{"a", "", 0, false}
    )
);

struct StringBufferEqualsIgnoreCaseParams {
  const char* buf;
  size_t buf_len;
  const char* s;
  bool expected;
};

class StringBufferEqualsIgnoreCaseTest : public ::testing::TestWithParam<StringBufferEqualsIgnoreCaseParams> {};

TEST_P(StringBufferEqualsIgnoreCaseTest, ReturnsExpected) {
  const StringBufferEqualsIgnoreCaseParams& p = GetParam();
  EXPECT_EQ(StringBufferEqualsIgnoreCase(p.buf, p.buf_len, p.s), p.expected);
}

INSTANTIATE_TEST_SUITE_P(
    StringBufferEqualsIgnoreCase,
    StringBufferEqualsIgnoreCaseTest,
    ::testing::Values(
        // --- Match ---

        // Same characters
        StringBufferEqualsIgnoreCaseParams{"txtvers", 7, "txtvers", true},
        // Same characters in another case, either side
        StringBufferEqualsIgnoreCaseParams{"TxtVers", 7, "txtvers", true},
        StringBufferEqualsIgnoreCaseParams{"txtvers", 7, "TXTVERS", true},
        // Only buf_len characters of the buffer count; the buffer need not end there,
        // as with a key followed by "=value" in a TXT record
        StringBufferEqualsIgnoreCaseParams{"txtvers=1", 7, "txtvers", true},
        // Empty buffer and empty string
        StringBufferEqualsIgnoreCaseParams{"", 0, "", true},

        // --- No match ---

        // Different characters
        StringBufferEqualsIgnoreCaseParams{"txtvars", 7, "txtvers", false},
        // The buffer holds only a prefix of the string
        StringBufferEqualsIgnoreCaseParams{"txtver", 6, "txtvers", false},
        // The buffer holds more than the string
        StringBufferEqualsIgnoreCaseParams{"txtvers1", 8, "txtvers", false},
        // Empty buffer, non-empty string
        StringBufferEqualsIgnoreCaseParams{"", 0, "a", false},
        // NULL on either side
        StringBufferEqualsIgnoreCaseParams{nullptr, 0, "", false},
        StringBufferEqualsIgnoreCaseParams{"", 0, nullptr, false}
    )
);
