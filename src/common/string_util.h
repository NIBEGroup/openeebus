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
/**
 * @file
 * @brief String utility
 */
#ifndef SRC_COMMON_STRING_UTIL_H_
#define SRC_COMMON_STRING_UTIL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "src/common/eebus_malloc.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Dynamically allocates a buffer and copies the string into it
 * @param src String to be copied
 * @return A copy of src string. Use StringDelete() to deallocate it
 */
char* StringCopy(const char* src);

/**
 * @brief Dynamically allocates a buffer, fills it with '\0' and then copies
 * up to n symbols from the string into it
 * @param src String to be copied
 * @param n Maximal number of number of symbols to be copied
 * @return A copy of src string. Use StringDelete() to deallocate it
 */
char* StringNCopy(const char* src, size_t n);

/**
 * @brief Deallocate the string
 * @param s String to be deallocated
 */
static inline void StringDelete(char* s) {
    if (s == NULL) {
        return;
    }

    EEBUS_FREE(s);
}

/**
 * @brief Check if the string specified is empty (string is NULL or zero length)
 * @param s String to be checked
 */
static inline bool StringIsEmpty(const char* s) {
    return (s == NULL) || (strlen(s) == 0);
}

/**
 * @brief Check if two strings are identical and no longer than n characters
 * @param a First string
 * @param b Second string
 * @param n Maximum allowed length of both strings
 * @return true if both strings are identical and their lengths do not exceed n
 */
bool StringNCompare(const char* a, const char* b, size_t n);

/**
 * @brief Dynamically allocates buffer and performs formatted sprintf() into it
 *
 * Transformed from Go fmt.Sprintf()
 *
 * @param format Format string
 * @return Result string. Use StringDelete() to deallocated it
 */
const char* StringFmtSprintf(const char* format, ...);

/**
 * @brief Remove the token from string
 * @param s String to be processed
 * @param token Token to be removed from string
 */
const char* StringRemoveToken(char* s, const char* token);

/**
 * @brief Converts a byte array into a hexadecimal string representation.
 *
 * This function takes a pointer to a byte array and its length, and returns
 * a dynamically allocated string containing the hexadecimal representation
 * of the byte array. Each byte is represented by two hexadecimal characters.
 *
 * @param data Pointer to the input byte array.
 * @param data_len Length of the input byte array.
 * @return A dynamically allocated string containing the hexadecimal
 *         representation of the input byte array. The caller is responsible
 *         for deallocating the returned string with StringDelete().
 */
char* StringWithHex(const uint8_t* data, size_t data_len);

/**
 * @brief Tokenizes a string based on specified delimiters.
 *
 * This function splits the input string into tokens using the provided
 * delimiters. It maintains context between successive calls to allow
 * continued tokenization of the same string.
 *
 * @param s The input string to be tokenized. On the first call, this should
 *          point to the string to be tokenized. On subsequent calls, this
 *          should be NULL to continue tokenizing the previous string.
 * @param delimiters A string containing all delimiter characters.
 * @param p A pointer to a char* variable that maintains context between
 *          successive calls. On the first call, this should point to a
 *          variable initialized to NULL. On subsequent calls, it should
 *          point to the same variable used in the previous call.
 * @return A pointer to the next token found in the string, or NULL if no
 *         more tokens are available.
 *
 * @note The input string is modified in place by replacing delimiter
 *       characters with null terminators ('\0'). The caller is responsible
 *       for ensuring that the input string is mutable and remains valid
 *       until all tokens have been processed.
 */
char* StringToken(char* s, const char* delimiters, char** p);

/**
 * @brief Converts all characters to uppercase
 * @param s String to be converted to uppercase
 * @return A new dynamically allocated string with all characters converted to uppercase. The caller is responsible for
 * deallocating the returned string with StringDelete().
 */
char* StringToUpper(const char* s);

/**
 * @brief Inserts a space every n characters
 * @param s String to be grouped
 * @param n Number of characters per group
 * @return A new dynamically allocated grouped string. The caller is responsible for deallocating the returned string
 * with StringDelete().
 */
char* StringGroupByN(const char* s, size_t n);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_STRING_UTIL_H_
