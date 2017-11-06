#ifndef TEST_INCLUDE_GUARD
#define TEST_INCLUDE_GUARD

// This header is a test tool. This header provides the following facilities.
//
// ASSERT is an variadic macro that prints an error message to stderr and
// increments the value of global variable status if the first argument is
// false. The error message contains the filename and the line number of the
// failed macro invocation followed by the failed expression, followed by the
// names and values of the arguments of ASSERT starting from the second
// argument.
//
// status is a global variable that captures the number of times ASSERT failed.
// status can reach the max value of 64.
//
// PRINT is a variadic macro that prints the name and value of its arguments
// starting from the second.
//
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
// We don't want status to be greater than 64, because status can be used as
// the exit code.
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

/*
Copyright (c) 2017, Dmitry Goncharov
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Neither the names of the copyright holders nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
