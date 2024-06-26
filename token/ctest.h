#ifndef CTEST_INCLUDE_GUARD
#define CTEST_INCLUDE_GUARD

/*
ctest.h is a test tool. ctest.h provides the following facilities.

ASSERT(<expression>[, const char* format[, <parameter>...]]);
ASSERT is a variadic macro that prints an error message to stderr and
increments the value of global variable test_status if the first argument is
false. The error message contains the filename and the line number of the
failed macro invocation followed by the failed expression.
ASSERT takes optional printf format specification and format parameters
after the expression.
If more than one paremeter is passed to ASSERT the second argument has to
be a printf format specification. If first argument evaluates to false these
optional parameters starting from the third are printed to stderr according
to the format specification.
This copy of ctest.h takes up to 5 optional parameters after the format
specification.  This number can be increased in the working copy of ctest.h
by modifying DISPATCH.

test_status is a global variable that captures the number of times ASSERT
failed, but no greater than 64. test_status is restricted to 64, because
test_status can be used as the exit code.

ctest.h is supposed to be copied to your project and included in a test
program.

#include "ctest.h"

int main(int argc, char* argv[])
{
    int x = 1, y = 2;
    ASSERT(x == y);
    ASSERT(x == y, "x is not equal to y\n");
    ASSERT(x == y, "%s: x=%d, y=%d\n", argv[0], x, y);
    ASSERT(x == y, "x=%d, y=%d %d %d %d\n",  x, y, 1, 2, 3);
    ASSERT(x == y, "");
    return test_status;
}

$ ./test ; echo $?
ctest.t.c:6: assertion `x == y' failed
ctest.t.c:7: assertion `x == y' failed x is not equal to y
ctest.t.c:8: assertion `x == y' failed ./tc: x=1, y=2
ctest.t.c:9: assertion `x == y' failed x=1, y=2 1 2 3
ctest.t.c:10: assertion `x == y' failed 5

To invoke ASSERT from multiple threads test_status can be made an atomic int in
the working copy of ctest.h.  */

#include <stdio.h>

#define HEAD(x, ...) (x)
#define SHEAD(x, ...) #x
#define HEADLESS(x, ...) __VA_ARGS__
/* CALL is needed to have HEADLESS expanded before Px is expanded.  */
#define CALL(Px, ...) Px(__VA_ARGS__)
#define P0() fprintf(stderr, "\n")
#define P1(...) fprintf(stderr, __VA_ARGS__)
#define DISPATCH(_1, _2, _3, _4, _5, _6, _7, _8, Px, ...) Px
#define PRINT(...) CALL(DISPATCH(__VA_ARGS__, P1, P1, P1, P1, P1, P1, P1, P0),\
                                                        HEADLESS(__VA_ARGS__))

static int test_status = 0;
static int test_ntotal = 0;
static int test_nfailed = 0;
#define ASSERT(...)\
do {\
    ++test_ntotal;\
    if (!(HEAD(__VA_ARGS__))) {\
        ++test_nfailed;\
        if (test_status < 64)\
            ++test_status;\
        fprintf(stderr, "%s:%d: assertion `%s' failed ",\
                                    __FILE__, __LINE__, SHEAD(__VA_ARGS__));\
        PRINT(__VA_ARGS__);\
    }\
} while (0)
#endif

/*
Copyright (c) 2019-2023, Dmitry Goncharov
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
