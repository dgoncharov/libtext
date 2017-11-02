#include "libtext.h"
#include "test.h"
#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static int verbose = 0;

template <class T>
static std::string tos(T x)
{
    std::stringstream s;
    s << x;
    return s.str();
}

static std::string tos(int8_t x)
{
    std::stringstream s;
    s << (int) x;
    return s.str();
}

static std::string tos(uint8_t x)
{
    std::stringstream s;
    s << (uint32_t) x;
    return s.str();
}

template <class T>
static void int_overflow(int line)
{
    T k = 7;
    const T max = std::numeric_limits<T>::max();
    // Test that std::numeric_limits<T>::max() can be read.
    std::stringstream ss;
    ss << +max;
    const char* s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == max, k, max, line);

    // Cause overflow.
    // Test that std::numeric_limits<T>::max() + 1 makes libtext::read fail.
    std::string buf = ss.str();
    const int v = *buf.rbegin();
    ASSERT(v == '5' || v == '7', v);
    *buf.rbegin() += 1;
    if (verbose)
        std::cout
            << "max = " << +max << ", buf = " << buf << ", line = " << line
            << std::endl;
    k = 2;
    const char* b = buf.c_str();
    s = libtext::read(b, "", &k);
    ASSERT(!s, b, line);
    ASSERT(k == 2, k, b, line);
}

template <class T>
static void int_underflow(int line)
{
    T k = 7;
    const T min = std::numeric_limits<T>::min();
    // Test that std::numeric_limits<T>::min() can be read.
    std::stringstream ss;
    ss << +min;
    const char* s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == min, k, min, line);

    // Cause underflow.
    // Test that std::numeric_limits<T>::min() - 1 makes libtext::read fail.
    std::string buf = ss.str();
    const int v = *buf.rbegin();
    ASSERT(v == '0' || v == '8', v);
    *buf.rbegin() += 1;
    if (verbose)
        std::cout
            << "min = " << +min << ", buf = " << buf << ", line = " << line
            << std::endl;
    k = 2;
    const char* b = buf.c_str();
    s = libtext::read(b, "", &k);
    ASSERT(!s, b, line);
    ASSERT(k == 2, k, b, line);
}

template <class T>
static void uint_underflow(int line)
{
    T k = 7;
    const T max = std::numeric_limits<T>::max();
    // Test that -std::numeric_limits<T>::max() can be read.
    std::stringstream ss;
    ss << '-' << +max;
    const char* s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == 1, k, max, line);

    // Test that -std::numeric_limits<T>::max()-1 cannot be read.
    std::string buf = ss.str();
    const int v = *buf.rbegin();
    ASSERT(v == '5' || v == '7', v);
    *buf.rbegin() += 1;
    if (verbose)
        std::cout
            << "max = " << +max << ", buf = " << buf << ", line = " << line
            << std::endl;
    k = 2;
    const char* b = buf.c_str();
    s = libtext::read(b, "", &k);
    ASSERT(!s, s, b, line);
    ASSERT(k == 2, k, b, line);
}

template <class T>
static void float_overflow(int line)
{
    T k = 7;
    const T max = std::numeric_limits<T>::max();
    // Test that std::numeric_limits<T>::max() can be read.
    std::stringstream ss;
    ss << std::fixed << max;
    const char* s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == max, k, max, line);
    std::string str = ss.str();
    ASSERT(*str.begin() < '9', *str.begin());
    *str.begin() += 1;
    if (verbose)
        std::cout
            << "str = " << ss.str()
            << ", k = " << *(uint64_t*) &k << ", max = " << *(uint64_t*) &max
            << ", str = " << str << ", line = " << line
            << std::endl;
    k = 2;
    s = libtext::read(str.c_str(), "", &k);
    ASSERT(!s, line);
    ASSERT(k == 2, *(uint64_t*) &k, *(uint64_t*) &max, line);
}

template <class T>
static void float_underflow(int line)
{
    T k = 7;
    const T min = std::numeric_limits<T>::min();
    // Test that std::numeric_limits<T>::min() can be read.
    std::stringstream ss;
    ss << std::fixed << min;
    const char* s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == min, k, min, line);
    std::string str = ss.str();
    ASSERT(*str.begin() < '9', *str.begin());
    *str.begin() += 1;
    if (verbose)
        std::cout
            << "str = " << ss.str()
            << ", k = " << *(uint64_t*) &k << ", min = " << *(uint64_t*) &min
            << ", str = " << str << ", line = " << line
            << std::endl;
    k = 2;
    s = libtext::read(str.c_str(), "", &k);
    ASSERT(!s, line);
    ASSERT(k == 2, *(uint64_t*) &k, *(uint64_t*) &min, line);
}

template <class T>
static void wrong_type(int line)
{
    // Cannot init an int with a string.
    T x = 2;
    const char* s = libtext::read("hello", ",", &x);
    ASSERT(!s, s, line);
    ASSERT(x == 2, x, line);
}

template <class T>
static void test_int(int line)
{
    T x, y, z;
    const char* s;
    s = libtext::read("0", "", &x);
    ASSERT(s, line);
    ASSERT(x == 0, x, line);

    x = y = 2;
    s = libtext::read("-0,+0", ",", &x, &y);
    ASSERT(s, line);
    ASSERT(x == 0, x, line);
    ASSERT(y == 0, y, line);

    x = y = 2;
    s = libtext::read("0x0,0x0", ",", &x, &y);
    ASSERT(s, line);
    ASSERT(x == 0, x, line);
    ASSERT(y == 0, y, line);

    x = y = 2;
    s = libtext::read("00,00", ",", &x, &y);
    ASSERT(s, line);
    ASSERT(x == 0, x, line);
    ASSERT(y == 0, y, line);

    s = libtext::read("1", "", &x);
    ASSERT(s, line);
    ASSERT(x == 1, x, line);

    s = libtext::read("01", "", &x);
    ASSERT(s, line);
    ASSERT(x == 1, x, line);

    s = libtext::read("+1", "", &x);
    ASSERT(s, line);
    ASSERT(x == +1, x, line);

    s = libtext::read("0x1", "", &x);
    ASSERT(s, line);
    ASSERT(x == 1, x, line);

    s = libtext::read("+0x1", "", &x);
    ASSERT(s, line);
    ASSERT(x == 1, x, line);

    s = libtext::read("+01", "", &x);
    ASSERT(s, line);
    ASSERT(x == 1, x, line);

    s = libtext::read("0xf", "", &x);
    ASSERT(s, line);
    ASSERT(x == 15, x, line);

    s = libtext::read("07", "", &x);
    ASSERT(s, line);
    ASSERT(x == 7, x, line);

    s = libtext::read("010", "", &x);
    ASSERT(s, line);
    ASSERT(x == 8, x, line);

    s = libtext::read("51,45", ",", &x, &y);
    ASSERT(s, line);
    ASSERT(x == 51, x, line);
    ASSERT(y == 45, y, line);

    // Hex integer.
    x = y = z = 0;
    s = libtext::read("0x51,0x45,0x47", ",", &x, &y, &z);
    ASSERT(s, line);
    ASSERT(x == 0x51, x, line);
    ASSERT(y == 0x45, y, line);
    ASSERT(z == 0x47, z, line);

    // Octal integer.
    x = y = z = 0;
    s = libtext::read("051,045,047", ",", &x, &y, &z);
    ASSERT(s, line);
    ASSERT(x == 051, x, line);
    ASSERT(y == 045, y, line);
    ASSERT(z == 047, z, line);

    s = libtext::read("126", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) 126, x, line);

    s = libtext::read("127", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) 127, x, line);

    // Negative integer.
    s = libtext::read("-1", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -1, x, line);

    s = libtext::read("-0x1", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -1, x, line);

    s = libtext::read("-01", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -1, x, line);

    // Negative integer.
    s = libtext::read("-3", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -3, x, line);

    // Negative integer.
    s = libtext::read("-127", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -127, x, line);

    s = libtext::read("-128", "", &x);
    ASSERT(s, line);
    ASSERT(x == (T) -128, x, line);

    // Negative integer.
    x = y = z = 0;
    s = libtext::read("-51,+45,-47", ",", &x, &y, &z);
    ASSERT(s, line);
    ASSERT(x == (T) -51, x, line);
    ASSERT(y == (T) 45, y, line);
    ASSERT(z == (T) -47, z, line);

    T k = 7;
    const T half = std::numeric_limits<T>::max()/2;
    // Test that -std::numeric_limits<T>::max()/2 can be read.
    std::stringstream ss;
    ss << '-' << +half;
    if (verbose)
        std::cout
            << "half = " << +half << ", buf = " << ss.str()
            << ", line = " << line << "\n";
    s = libtext::read(ss.str().c_str(), "", &k);
    ASSERT(s, line);
    ASSERT(k == (T) -half, k, half, line);

    // Test that -std::numeric_limits<T>::max()/2-1 can be read.
    std::string buf = ss.str();
    const int v = *buf.rbegin();
    ASSERT(v == '3' || v == '7', v);
    *buf.rbegin() += 1;
    if (verbose)
        std::cout
            << "half = " << +half << ", buf = " << buf
            << ", line = " << line << std::endl;
    k = 2;
    const char* b = buf.c_str();
    s = libtext::read(b, "", &k);
    ASSERT(s, b, line);
    ASSERT(k == (T) -half -1, k, b, line);
    // Integer overflow.
    int_overflow<T>(__LINE__);
}

template <class T>
static void trailing_ws(const char* sep, int line)
{
    T x, y;

    x = y = T();
    const char* s;
    s = libtext::read("51\t45\t", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line, sep);
    ASSERT(tos(y) == "45", y, line, sep);

    x = y = T();
    s = libtext::read("51 45\t", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);
    x = y = T();
    s = libtext::read("51\t45 ", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51 45 ", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51\t 45\t ", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51 \t 45 \t ", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(!*s, s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51\t45\t\n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51 45\t\n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51\t45 \n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51 45 \n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51\t 45\t \n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);

    x = y = T();
    s = libtext::read("51 \t 45 \t \n", sep, &x, &y);
    ASSERT(s, line);
    ASSERT(*s == '\n', s, line);
    ASSERT(tos(x) == "51", x, line);
    ASSERT(tos(y) == "45", y, line);
}

template <class T>
static void test_parsing(int line)
{
    T x, y;
    const char* s;
    // Ability to skip fields.
    // One field is skipped.
    x = T();
    s = libtext::read("51,45", ",", 0, &x);
    ASSERT(s);
    ASSERT(!*s);
    ASSERT(tos(x) == "45", x);

    // Last field is skipped.
    x = T();
    s = libtext::read("51,45", ",", &x, 0);
    ASSERT(s);
    ASSERT(!*s);
    ASSERT(tos(x) == "51", x);

    // All fields are skipped.
    s = libtext::read("51,45", ",", 0, 0);
    ASSERT(s);
    ASSERT(!*s);

    // More parameters then there are fields.
    s = libtext::read("51,45", ",", 0, 0, 0);
    ASSERT(!s);

    // Even when sep is a \n all fields are expected on the same line.
    x = y = T();
    s = libtext::read("51\n45", "\n", &x, &y);
    ASSERT(!s, s);
    ASSERT(tos(x) == "51", x);
    ASSERT(y == T());

    // Fewer parameters than there are fields.
    x = T();
    s = libtext::read("51,45", ",", &x);
    ASSERT(s);
    ASSERT(tos(x) == "51", x);

    // Fewer parameters than there are fields.
    s = libtext::read("51,45", ",", 0);
    ASSERT(s);
    ASSERT(!strcmp(s, "45"), s);

    // Empty input.
    s = libtext::read("", "", &x);
    ASSERT(!s, s, line);

    s = libtext::read("", ",", &x);
    ASSERT(!s, s, line);

    // Extra args.
    s = libtext::read("51", ",", &x);
    ASSERT(s, line);

    s = libtext::read("51", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51\n45", ",", &x, &y);
    ASSERT(!s, s, line);

    // Parsing still fails even when the user is not interested in the value.
    s = libtext::read("51", ",", &x, 0);
    ASSERT(!s, s, line);

    s = libtext::read("51", ",", 0, &x);
    ASSERT(!s, s, line);

    s = libtext::read("51", ",", 0, 0);
    ASSERT(!s, s, line);

    // Stray separator.
    s = libtext::read(",51,45", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,,45", ",", &x, &y);
    ASSERT(!s, s, line);

    // Line ends with a separator.
    s = libtext::read("51,45,", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45,\t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45, \t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45,\n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45,\t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45, \t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t ,", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t ,", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45,\t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t ,\t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t ,\t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t , \t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t , \t ", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t ,\n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t ,\n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45,\t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t ,\t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t ,\t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45, \t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45\t , \t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    s = libtext::read("51,45 \t , \t \n", ",", &x, &y);
    ASSERT(!s, s, line);

    // One parameter, 1 field, extra separator after the field.
    x = T();
    s = libtext::read("51, \t  \n", ",", &x);
    ASSERT(!s, s, line);
    ASSERT(tos(x) == "51", x, line);

    x = T();
    s = libtext::read("51, \t , \t \n", ",", &x);
    ASSERT(!s, s, line);
    ASSERT(tos(x) == "51", x, line);

    // Line ends with a separator. tab and space is a separator.
    trailing_ws<T>("\t", line);
    trailing_ws<T>(" ", line);

    // \n at the end of the line is fine.
    s = libtext::read("51,45\n", ",", &x, &y);
    ASSERT(s, line);

    s = libtext::read("51,45\t \n", ",", &x, &y);
    ASSERT(s, line);

    s = libtext::read("51,45 \t \n", ",", &x, &y);
    ASSERT(s, line);

    // Incorrect separator.
    s = libtext::read("51,45", ":", &x, &y);
    ASSERT(!s, s, line);
}

static void success(const std::string& input, const char* sep, const char* exp)
{
    std::string h = "test failed";
    const char* s;
    s = libtext::read(input.c_str(), sep, &h);
    ASSERT(s);
    ASSERT(!*s || *s == '\n', int(*s));
    ASSERT(exp == h, input, exp, h);
    h = "test failed";
    int k = 1;
    s = libtext::read(input.c_str(), sep, &h, &k);
    ASSERT(!s, s);
    ASSERT(exp == h, input, exp, h);
    ASSERT(k == 1, k);
}

template <class T>
static void failure(const std::string& input, const char* sep, T exp)
{
    T x = T();
    const char* s;
    s = libtext::read(input.c_str(), sep, &x);
    ASSERT(!s, s);
    ASSERT(x == exp, tos(x));
}

int main(int argc, char* argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    std::cout << "test " << __FILE__ << " case " << test << std::endl;

    const char* s;
    switch (test) {
    case 0: {
        // Simple well defined input.
        std::string host = "test failed";
        s = libtext::read("example.com:80", ":", &host);
        ASSERT(s);
        ASSERT(!strcmp(s, "80"));
        ASSERT("example.com" == host, host);

        host = "test failed";
        s = libtext::read("example.com", ":", &host);
        ASSERT(s);
        ASSERT(!*s);
        ASSERT("example.com" == host, host);

        host = "test failed";
        s = libtext::read("example.com\n", ":", &host);
        ASSERT(s);
        ASSERT(*s == '\n');
        ASSERT("example.com" == host, host);

        host = "test failed";
        uint16_t port = 77;
        s = libtext::read("example.com:80", ":", &host, &port);
        ASSERT(s);
        ASSERT(!*s);
        ASSERT("example.com" == host, host);
        ASSERT(port == 80, port);

        // Empty input.
        s = libtext::read("", ":", &host);
        ASSERT(!s, s);
        s = libtext::read("", ":", 0);
        ASSERT(!s, s);
        break;
    }
    case 1: {
        // Multiple lines.
        const char* input = "example.com:80\n192.168.1.1:8080";
        std::string host = "test failed";
        uint16_t port = 77;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(*input);
        ASSERT("example.com" == host, host);
        ASSERT(port == 80, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input);
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(!*input);
        ASSERT("192.168.1.1" == host, host);
        ASSERT(port == 8080, port);
        // Mix of integers and strings and floats.
        break;
    }
    case 2: {
        // Leading and trailing spaces at the line ends.
        const char* input = " \t example.com:80 \t \n \t 192.168.1.1:8080  \t";
        std::string host = "test failed";
        uint16_t port = 77;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(*input == '\n', input);
        ASSERT("example.com" == host, host);
        ASSERT(port == 80, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input, input);
std::cout << "test input = " << input << std::endl;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT("192.168.1.1" == host, host);
        ASSERT(port == 8080, port);

        // Leading and trailing spaces around separators.
        input =
            " \t example.com  \t : \t  80 \t \n \t 192.168.1.1  :\t  8080  \t";
        host = "test failed";
        port = 77;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(*input == '\n', input);
        ASSERT("example.com" == host, host);
        ASSERT(port == 80, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input, input);
std::cout << "test input = " << input << std::endl;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT("192.168.1.1" == host, host);
        ASSERT(port == 8080, port);

        // Leading and trailing spaces around separators.
        // tab is a separator.
        input =
            " \t example.com  \t   \t  80 \t \n \t 192.168.1.1   \t  8080  \t";
        host = "test failed";
        port = 77;
        input = libtext::read(input, "\t", &host, &port);
        ASSERT(input);
        ASSERT(*input == '\n', input);
        ASSERT("example.com" == host, host);
        ASSERT(port == 80, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input == ' ', input);
std::cout << "test input = " << input << std::endl;
        input = libtext::read(input, "\t", &host, &port);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT("192.168.1.1" == host, host);
        ASSERT(port == 8080, port);

        // Leading and trailing spaces around separators. One character fields.
        input = " \t e  \t : \t  8 \t \n \t 1  :\t  8  \t";
        host = "test failed";
        port = 77;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(*input == '\n', input);
        ASSERT("e" == host, host);
        ASSERT(port == 8, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input, input);
std::cout << "test input = " << input << std::endl;
        input = libtext::read(input, ":", &host, &port);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT("1" == host, host);
        ASSERT(port == 8, port);

        // Leading and trailing spaces around separators. One character fields.
        // Separator is a space.
        input = " \t e  \t   \t  8 \t \n \t 1   \t  8  \t";
        host = "test failed";
        port = 77;
        input = libtext::read(input, " ", &host, &port);
        ASSERT(input);
        ASSERT(*input == '\n', input);
        ASSERT("e" == host, host);
        ASSERT(port == 8, port);
        input = libtext::nextline(input);
        ASSERT(input);
        ASSERT(*input, input);
std::cout << "test input = " << input << std::endl;
        input = libtext::read(input, " ", &host, &port);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT("1" == host, host);
        ASSERT(port == 8, port);

        int8_t i8 = 1;
        uint8_t u8 = 1;
        int16_t i16 = 1;
        uint16_t u16 = 1;
        int32_t i32 = 1;
        uint32_t u32 = 1;
        int64_t i64 = 1;
        uint64_t u64 = 1;
        float f = 1.0;
        double d = 1.0;
        long double ld = 1.0;
        input = "\t -7 \t \t2 -76 \t 3 \t 6 88 -88 7\t 4.5 \t  7.1 \t8.4 \t\t";
        input = libtext::read(input, " ", &i8, &u8, &i16, &u16, &i32, &u32,
                                                    &i64, &u64, &f, &d, &ld);
        ASSERT(input);
        ASSERT(!*input, *input);
        ASSERT(i8 == -7, i8);
        ASSERT(u8 == 2, u8);
        ASSERT(i16 == -76, i16);
        ASSERT(u16 == 3, u16);
        ASSERT(i32 == 6, i32);
        ASSERT(u32 == 88, u32);
        ASSERT(i64 == -88, i64);
        ASSERT(u64 == 7, u64);
        ASSERT(f == 4.5f, f);
        ASSERT(d == 7.1, d);
        ASSERT(ld == 8.4l, ld);
        break;
    }
    case 3:
        test_int<int8_t>(__LINE__);
        test_int<uint8_t>(__LINE__);
        test_int<int16_t>(__LINE__);
        test_int<uint16_t>(__LINE__);
        test_int<int32_t>(__LINE__);
        test_int<uint32_t>(__LINE__);
        test_int<int64_t>(__LINE__);
        test_int<uint64_t>(__LINE__);
        break;
    case 4:
        int_underflow<int8_t>(__LINE__);
        int_underflow<int16_t>(__LINE__);
        int_underflow<int32_t>(__LINE__);
        int_underflow<int64_t>(__LINE__);

        uint_underflow<uint8_t>(__LINE__);
        uint_underflow<uint16_t>(__LINE__);
        uint_underflow<uint32_t>(__LINE__);
        uint_underflow<uint64_t>(__LINE__);
    case 5: {
            // float.
            float x = 7.7f, y = 8.8f;
            s = libtext::read("1234.2,2876.3", ",", &x, &y);
            ASSERT(s);
            ASSERT(x == 1234.2f, x);
            ASSERT(y == 2876.3f, y);

            // libtext::read does not skip \f.
            x = 7.7f, y = 8.8f;
            s = libtext::read("1234.2,\f2876.3", ",", &x, &y);
            ASSERT(!s);
            ASSERT(x == 1234.2f, x);
            ASSERT(y == 8.8f, y);
        }
        {
            double x = 7.7, y = 8.8;
            s = libtext::read("1234.2,2876.3", ",", &x, &y);
            ASSERT(s);
            ASSERT(x == 1234.2, x);
            ASSERT(y == 2876.3, y);

            // libtext::read does not skip \r.
            x = 7.7, y = 8.8;
            s = libtext::read("1234.2\r,2876.3", ",", &x, &y);
            ASSERT(!s);
            ASSERT(x == 1234.2, x);
            ASSERT(y == 8.8, y);
        }
        {
            long double x = 7.7l, y = 8.8l;
            s = libtext::read("1234.2,2876.3", ",", &x, &y);
            ASSERT(s);
            ASSERT(x == 1234.2l, x);
            ASSERT(y == 2876.3l, y);

            // libtext::read does not skip \v.
            x = 7.7l, y = 8.8l;
            s = libtext::read("\v1234.2,2876.3", ",", &x, &y);
            ASSERT(!s);
            ASSERT(x == 7.7l, x);
            ASSERT(y == 8.8l, y);
        }
        break;
    case 6:
        float_overflow<float>(__LINE__);
        float_overflow<double>(__LINE__);
        float_overflow<long double>(__LINE__);
        break;
    case 7:
//        float_underflow<float>(__LINE__);
//        float_underflow<double>(__LINE__);
//        float_underflow<long double>(__LINE__);
        break;
    case 8:
        test_parsing<std::string>(__LINE__);
        test_parsing<int8_t>(__LINE__);
        test_parsing<uint8_t>(__LINE__);
        test_parsing<int16_t>(__LINE__);
        test_parsing<uint16_t>(__LINE__);
        test_parsing<int32_t>(__LINE__);
        test_parsing<uint32_t>(__LINE__);
        test_parsing<int64_t>(__LINE__);
        test_parsing<uint64_t>(__LINE__);
        test_parsing<float>(__LINE__);
        test_parsing<double>(__LINE__);
        test_parsing<long double>(__LINE__);
        break;
    case 9:
        // Cannot initialize an integer with a string.
        wrong_type<int8_t>(__LINE__);
        wrong_type<uint8_t>(__LINE__);
        wrong_type<int16_t>(__LINE__);
        wrong_type<uint16_t>(__LINE__);
        wrong_type<int32_t>(__LINE__);
        wrong_type<uint32_t>(__LINE__);
        wrong_type<int64_t>(__LINE__);
        wrong_type<uint64_t>(__LINE__);
        wrong_type<float>(__LINE__);
        wrong_type<double>(__LINE__);
        wrong_type<long double>(__LINE__);
        break;
    case 10: {
        // nextline.
        const char* input = "hello\nworld";
        input = libtext::nextline(input);
        ASSERT(*input == 'w', input);
        input = "hello world";
        input = libtext::nextline(input);
        ASSERT(!*input);
        input = libtext::nextline("");
        ASSERT(!*input);
        break;
    }
    case 11: {
        // oneline.
        const char* input = "hello\nworld";
        std::string s = libtext::oneline(input);
        ASSERT(s == "hello", s);
        input = "hello world";
        s = libtext::oneline(input);
        ASSERT(s == input, s, input);
        s = libtext::oneline("");
        ASSERT(s.empty(), s);
        break;
    }
    case 12: {
        // If separator is \0, \n or "a character missing in a line"
        // then read reads the whole line with trailing and leading ' ' and \t
        // stripped.
        const char* seps[] = {"", "\n", ";", 0};
        const char* input[] = {"45 \t : \t51", " \t 45 \t : \t51 \t ",
                                            " \t 45 \t : \t51\n\t47; 50", 0};
        for (const char** sep = seps; *sep; ++sep) {
            for (const char** in = input; *in; ++in) {
                success(*in, *sep, input[0]);
                failure<int8_t>(*in, *sep, 45);
                failure<uint8_t>(*in, *sep, 45);
                failure<int16_t>(*in, *sep, 45);
                failure<uint16_t>(*in, *sep, 45);
                failure<int32_t>(*in, *sep, 45);
                failure<uint32_t>(*in, *sep, 45);
                failure<int64_t>(*in, *sep, 45);
                failure<uint64_t>(*in, *sep, 45);
                failure<float>(*in, *sep, 45.0f);
                failure<double>(*in, *sep, 45.0);
                failure<long double>(*in, *sep, 45.0l);
            }
        }
        break;
    }
    case 13: {
        // strip.
        struct t {
            static void p(const std::string& v, const std::string& exp,
                                                            const char* reject)
            {
                std::string r = v;
                libtext::strip(&r, reject);
                ASSERT(r == exp, r, exp, v, reject);
                if (verbose)
                    std::cout
                        << "input = " << v << ", result = " << r
                        << ", reject = " << reject
                        << std::endl;

                char b[64];
                memcpy(b, v.c_str(), v.size()+1);
                ASSERT(b == v, b, v);
                const char* s = libtext::strip(b, reject);
                ASSERT(s == exp, s, exp, v, b, reject);
                if (verbose)
                    std::cout
                        << "input = " << v << ", result = " << s
                        << ", reject = " << reject
                        << '\n' << std::endl;
            }
        };
        const std::string v = "192.169.1.1";
        t::p(v, v, ".");
        t::p(v, v, "");
        t::p("." + v + ".", v, ".");
        t::p("." + v, v, ".");
        t::p("..," + v, v, ".,");
        t::p(v + ".,.", v, ",.");
        t::p(".." + v + "..", v, ".");
        t::p("....", "", ".");
        t::p("", "", ".");
        t::p("", "", "");
        t::p(",." + v + ".,", v, ".,");
        t::p(",..," + v + ".;", v, ".;,");
        break;
   }
   default:
        std::cerr << "case " << test << " not found" << std::endl;
        status = 65;
    }
    if (verbose)
        std::cout << "status = " << status << std::endl;
    return status;
}