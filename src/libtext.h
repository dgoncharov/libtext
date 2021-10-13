#ifndef LIBTEXT_INCLUDE_GUARD
#define LIBTEXT_INCLUDE_GUARD

#include <string>
#include <stdint.h>
#undef have_string_view
#if defined __has_include && __has_include(<string_view>)\
                                                    && __cplusplus >= 201703L
#include <string_view>
#define have_string_view 1
#endif

namespace libtext {
const char* read(const char* input, const char* sep, std::string* result);
#ifdef have_string_view
const char* read(const char* input, const char* sep, std::string_view* result);
#endif
const char* read(const char* input, const char* sep, uint8_t* result);
const char* read(const char* input, const char* sep, uint16_t* result);
const char* read(const char* input, const char* sep, uint32_t* result);
const char* read(const char* input, const char* sep, uint64_t* result);
const char* read(const char* input, const char* sep, int8_t* result);
const char* read(const char* input, const char* sep, int16_t* result);
const char* read(const char* input, const char* sep, int32_t* result);
const char* read(const char* input, const char* sep, int64_t* result);
const char* read(const char* input, const char* sep, float* result);
const char* read(const char* input, const char* sep, double* result);
const char* read(const char* input, const char* sep, long double* result);
const char* read(const char* input, const char* sep, int result);
// This definition is not static to have the generated function be an extern
// weak symbol to have only one in the final binary, which reduces the image
// size.  Having this definition static'd result in the generated function be a
// static strong symbol with multiple of these in the final binary (for a
// combination of arguments).
template <class T, class... U>
const char* read(const char* input, const char* sep, T result,  U... u)
{
    const char* s = read(input, sep, result);
    if (!s || !*s || *s == '\n')
        return 0; // The number of arguments exceeds the number of fields.
#ifdef __cpp_fold_expressions
    // Prefer fold expression over recursion, because
    // 1. Recursion consumes more stack. Even when the tail call is optimized.
    // 2. Recursive code generates more symbols.
    ((s = read(s, sep, u)) && ...);
    return s;
#else
    return read(s, sep, u...);
#endif
}
const char *nextline(const char* input);
std::string oneline(const char* input);
} // libtext
#endif

/*
 * Copyright (c) 2017 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
