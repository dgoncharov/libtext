#ifndef TEST_INCLUDE_GUARD
#define TEST_INCLUDE_GUARD

// This header is a test tool. This tool provides the following facilities.
// ASSERT is an variadic macro that prints an error message to stderr and
// increments the value of global variable status if the first argument is
// false. The error message contains the filename and the line number of the
// failed macro invocation followed by the failed expression, followed by the
// names and values of the arguments of ASSERT starting from the second
// argument.
// status is a global variable that captures the number of times ASSERT failed.
// PRINT is a variadic macro that prints the name and value of its arguments
// starting from the second.
// This header is supposed to be copied to your project and included in a test
// program.
//
// #include "test.h"
// int main(int argc, char* argv[])
// {
//     int x = 1, y = 2;
//     ASSERT(x == y, x, y);
//     return status;
// }
// Running this program produces
// t.cpp:5: assertion `x == y' failed x = `1', y = `2'
//
// To invoke ASSERT from multiple threads status can be made an atomic int in
// the working copy of test.h.

#include <iostream>

#define HEAD(x, ...) (x)
#define SHEAD(x, ...) #x
#define HEADLESS(x, ...) __VA_ARGS__
// CALL is needed to have HEADLESS expanded before Px is expanded.
#define CALL(Px, ...) Px(__VA_ARGS__)
#define P0()
#define P1(x) << #x << " = " << "`" << (x) << "'"
#define P2(x, y) << #x << " = " << "`" << (x) << "', " P1(y)
#define P3(x, y, z) << #x << " = " << "`" << (x) << "', " P2(y, z)
#define P4(x, y, z, a) << #x << " = " << "`" << (x) << "', " P3(y, z, a)
#define P5(x, y, z, a, b) << #x << " = " << "`" << (x) << "', " P4(y, z, a, b)
#define DISPATCH(_1, _2, _3, _4, _5, _6, Px, ...) Px
#define PRINT(...) CALL(DISPATCH(__VA_ARGS__, P5, P4, P3, P2, P1, P0),\
                                                        HEADLESS(__VA_ARGS__))

static int status = 0;
// We don't want to exit with exit code greater than 64.
#define ASSERT(...)\
do {\
    if (!(HEAD(__VA_ARGS__))) {\
        if (status < 64)\
            ++status;\
        std::cerr\
            << __FILE__ << ":" << __LINE__\
            << ": assertion `" << SHEAD(__VA_ARGS__) << "' failed "\
            PRINT(__VA_ARGS__);\
        std::cerr << std::endl;\
    }\
} while (0)

#endif