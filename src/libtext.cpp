#include <libtext.h>
#include <limits>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

// Return 1 if 'input' points at the end of the line or at the end of the string.
// Return 0 otherwise.
static int eol(const char* input)
{
    return !*input || '\n' == *input;
}

static int ws(const char* input)
{
    return ' ' == *input || '\t' == *input;
}

// Return 1 if the first character of x is the same as the first character of
// y. Consider a space to be equal to a tab.
static int same(const char* x, const char* y)
{
    if (ws(x))
        return ws(y);
    return *x == *y;
}

// If 'input' begins with 'sep' on this line then return the address of the
// character immediately following 'sep' on this line. If 'input' does not
// bebing with 'sep' on this line then return 0.
// Consider space and tab equal.
static const char* skipsep(const char* input, const char* sep)
{
    while (*input && !eol(input) && *sep && same(input, sep))
        ++input, ++sep;
    if (*sep)
        return 0; // Strings don't match.
    return input;
}

// Skip the initial optional space of arbitrary length.
// If the first non space character is eol as determined by 'eol' then return
// the address of this first non space character.
// If the following character is not a separator return 0.
// If the following character is a separactor followed by arbitrary amount of
// space followed by eol then return 0.
// Otherwise return the address of the first character after the first
// separator.
static const char* next(const char* input, const char* sep)
{
    if (!ws(sep))
        input += strspn(input, " \t");
    if (eol(input))
        // Return 'input' to let the caller detect a possible malformed input
        // should the user expect more fields.
        return input;
    input = skipsep(input, sep);
    if (!input)
        return 0; // Expected separator is not found.
    input += strspn(input, " \t");
    if (ws(sep))
        return input;
    if (skipsep(input, sep))
        return 0; // Multiple consecutive separators
    if (eol(input))
        return 0; // String ends with a separator other than ws.
    return input;
}

// Return the next occurence of 'sep' in 'input' on this line.
// If 'sep' in not present in 'input' on this line then return the
// address of the end of line character in 'input'. eol is \n or \0.
static const char* nextsep(const char* input, const char* sep)
{
    while (*input && !eol(input) && !skipsep(input, sep))
        ++input;
    return input;
}

template <class T>
static const char* readll(const char* input, const char* sep, T* result)
{
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const long long v = strtoll(input, &r, 0);
    if (errno || r == input)
        return 0;
    if (std::numeric_limits<T>::max() < v)
        return 0; // Overflow.
    if (std::numeric_limits<T>::min() > v)
        return 0; // Underflow.
    if (result) {
        *result = (T) v;
    }
    return next(r, sep);
}

template <class T>
static const char* readull(const char* input, const char* sep, T* result)
{
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const unsigned long long v = strtoull(input, &r, 0);
    if (errno || r == input)
        return 0;
    const unsigned long long ullmax =
        std::numeric_limits<unsigned long long>::max();
    const T tmax = std::numeric_limits<T>::max();
    const int neg = *input == '-';
    if (!neg && tmax < v)
        return 0; // Overflow.
    if (neg && v && v <= ullmax - tmax)
        return 0; // Underflow.
    if (result)
        *result = (T) v;
    return next(r, sep);
}

namespace libtext
{
const char* read(const char* input, const char* sep, std::string* result)
{
    // Skip white space.
    input += strspn(input, " \t");
    // Read a word.
    const char* s = nextsep(input, sep);
    if (s == input)
        return 0; // Have not read anything.
    // Have read something.
    assert(s > input);
    assert(!ws(input));
    if (result) {
        const char* p = s;
        assert(p > input);
        while (ws(--p));
        assert(p >= input);
        assert(!ws(p));
        result->assign(input, p-input+1);
    }
    return next(s, sep);
}

const char* read(const char* input, const char* sep, uint8_t* result)
{
    return readull(input, sep, result);
}

const char* read(const char* input, const char* sep, uint16_t* result)
{
    return readull(input, sep, result);
}

const char* read(const char* input, const char* sep, uint32_t* result)
{
    return readull(input, sep, result);
}

const char* read(const char* input, const char* sep, uint64_t* result)
{
    return readull(input, sep, result);
}

const char* read(const char* input, const char* sep, int8_t* result)
{
    return readll(input, sep, result);
}

const char* read(const char* input, const char* sep, int16_t* result)
{
    return readll(input, sep, result);
}

const char* read(const char* input, const char* sep, int32_t* result)
{
    return readll(input, sep, result);
}

const char* read(const char* input, const char* sep, int64_t* result)
{
    return readll(input, sep, result);
}

const char* read(const char* input, const char* sep, float* result)
{
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const float v = strtof(input, &r);
    if (errno || r == input)
        return 0;
    if (result)
        *result = v;
    return next(r, sep);
}

const char* read(const char* input, const char* sep, double* result)
{
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const double v = strtod(input, &r);
    if (errno || r == input)
        return 0;
    if (result)
        *result = v;
    return next(r, sep);
}

const char* read(const char* input, const char* sep, long double* result)
{
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const long double v = strtold(input, &r);
    if (errno || r == input)
        return 0;
    if (result)
        *result = v;
    return next(r, sep);
}

const char* read(const char* input, const char* sep, int result)
{
    assert(!result);
    return read(input, sep, (std::string*) 0);
}

const char* nextline(const char* input)
{
    while (*input && *input++ != '\n');
    return input;
}

std::string oneline(const char* input)
{
    return std::string(input, strcspn(input, "\n"));
}
} // libtext

/*
 * Copyright (c) 2017 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
