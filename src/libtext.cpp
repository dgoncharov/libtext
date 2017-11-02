#include <libtext.h>

#include <iostream>

#include <limits>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

namespace libtext
{

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

// Return 1 if the first character of 'input' is the same as the first
// character of 'sep'. Consider space and tab equal.
// Return 0 otherwise.
static int issep(const char* input, const char* sep)
{
    if (ws(sep))
        return ws(input);
    return *input == *sep;
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
std::cout << "input = " << input << std::endl;
    if (!ws(sep))
        input += strspn(input, " \t");
    if (eol(input)) {
std::cout << "eol" << std::endl;
        // Return 'input' to let the caller detect a possible malformed input
        // should the user expect more fields.
        return input;
    }
    if (!issep(input, sep)) {
std::cerr << "expected separator not found" << std::endl;
        return 0; // Expected separator is not found.
    }
    ++input; // Skip the separator.
    input += strspn(input, " \t");
    if (ws(sep))
        return input;
    if (issep(input, sep)) {
std::cerr << "multiple consecutive separators" << std::endl;
        return 0; // Multiple consecutive separators
    }
    if (eol(input)) {
std::cerr << "string ends with a separator" << std::endl;
        return 0; // String ends with a separator other than ws.
    }
    return input;
}

const char* read(const char* input, const char* sep, std::string* result)
{
std::cout << "input = " << input << std::endl;
    // Skip white space.
    input += strspn(input, " \t");
std::cout << "input = " << input << std::endl;
    const char* s = input;
    // Read a word.
    while (!eol(s) && !issep(s, sep))
        ++s;
std::cout << "s = " << s << std::endl;
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
std::cout << "p = " << p << std::endl;
        result->assign(input, p-input+1);
std::cout << "result = " << *result << std::endl;
    }
std::cout << "s = " << s << std::endl;
    return next(s, sep);
}

template <class T>
static const char* readll(const char* input, const char* sep, T* result)
{
std::cout << "readll input = " << input << std::endl;
    // strtoull skips leading space, \t, \n, \v, \f, \r.
    // Detect malformed input by skipping " \t" and checking if the following
    // char is a space as determined by isspace.
    input += strspn(input, " \t");
    if (isspace(*input))
        return 0;
    char* r;
    errno = 0;
    const long long v = strtoll(input, &r, 0);
std::cout << "v = " << v << std::endl;
    if (errno || r == input)
        return 0;
    if (std::numeric_limits<T>::max() < v)
        return 0; // Overflow.
    if (std::numeric_limits<T>::min() > v)
        return 0; // Underflow.
    if (result) {
        *result = (T) v;
std::cout << "*result = " << *result << std::endl;
    }
    return next(r, sep);
}

template <class T>
static const char* readull(const char* input, const char* sep, T* result)
{
std::cout << "input = " << input << std::endl;
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

std::cout << "result = " << *result << std::endl;
std::cout << "r = " << r << std::endl;
    return next(r, sep);
}

//template <class T>
//static const char* read_int(const char* input, T* result)
//{
//    const char *p = input;
//    // Read a word.
//    T v = 0;
//    for (; *p && isdigit(*p); ++p) {
//        const T b = v * 10 + T(*p) - '0';
//        if (b < v)
//            // Integer overflow.
//            return 0;
//        v = b;
//    }
//    if (p == input)
//        return 0; // Have not read anything.
//    if (result)
//        *result = v;
//    if ('\n' == *p)
//        return p;
//    if (*p && eol(p+1) && !isdigit(*p))
//        return 0; // String ends with a separator other than \n.
//
//    // If \n is found before all parameters are initialized that's malformed
//    // input. If \n is found after all parameters are initialized that's fine.
//    // This code does not know whether all parameters are initialized.
//    // Therefore, don't return 0. The caller has to check if \n is found before
//    // all parameters are initialized.
//    return next(p);
//}
//
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

void strip(std::string* input, const char* reject)
{
    std::string& r = *input;
    const size_t k = strspn(r.c_str(), reject);
    size_t len = r.size();
    assert(len >= k);
    while (len-k && strchr(reject, r[len - 1]))
        --len;
    assert(len >= k);
    assert(len <= r.size());
    if (k)
        r = r.substr(k, len-k);
    else
        r.resize(len); // If len == r.size() then this resize is a noop.
}

char* strip(char* input, const char* reject)
{
    input += strspn(input, reject);
    size_t len = strlen(input);
    while (len && strchr(reject, input[len - 1]))
        --len;
    input[len] = '\0';
    return input;
}
} // libtext