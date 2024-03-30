/* include token.c, rather than token.h, to test static functions.  */
#include "token.c"
#include "ctest.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

static const char quotes[] = "'\"";
static const char *dquote_and_separators = quotes_and_separators + 1;
static int test (long, int, char *[]);
static void test_memecspn (int, const char *, const char *, size_t);
static void test_memecspn_impl (int, const char *, size_t, const char *,
                                size_t);
static void test_strecspn (int, const char *, const char *, size_t);
static void test_next_token (int, const char *, const char *, ...);
static void test_next_token_ws (int, const char *, const char *, ...);
static void test_next_token_ws_impl (int, const char *, const char *, va_list,
                                     char);
static void test_next_token_impl (int, const char *, const char *, va_list,
                                  char, char);
static void test_next_dequoted_token (int, const char *, const char *, ...);
static void test_next_dequoted_token_ws (int, const char *, const char *, ...);
static void test_next_dequoted_token_i (int, const char *, const char *, ...);

static void test_next_dequoted_token_ws_impl (int, const char *, const char *,
                                              va_list, char);
static void test_next_dequoted_token_impl (int, const char *, const char *,
                                           va_list, char, char);
static char *strdup_ (const char *s);
static char *subchr (char *input, char x, char y);
static char *swapchr (char *input, char x, char y);

int main (int argc, char *argv[])
{
  long n = 0;
  if (argc >= 2)
    {
      /* Run the specified test.  */
      char *r;
      errno = 0;
      n = strtol (argv[1], &r, 0);
      if (errno || r == argv[1])
        {
          fprintf (stderr, "usage: %s [test] [test arg]...\n", argv[0]);
          return 1;
        }
    }
  test (n, argc, argv);
  fprintf (test_status > 0 ? stderr : stdout,
           "%d failed tests, %d total tests\n", test_nfailed, test_ntotal);
  return test_status;
}

/* Test the following functions
memecspn
strecspn
next_token
next_dequoted_token

These characters have special powers.
backslash, space, newline, single quote, double quote.

Test parsing and dequoting

1. Outside of quotes.
  empty
  end of token
    backslash immediately followed by the null terminator
    space immediately followed by the null terminator
    newline immediately followed by the null terminator
    single quote immediately followed by the null terminator
    double quote immediately followed by the null terminator

  newline
  other space
  backslash escapes newline,
  backslash escapes space,
  backslash escapes single quote,
  backslash escapes double quote,
  backslash escapes backslash,

2. Inside single quotes.
  empty
  end of single quoted input
    single quote immediately followed by the null terminator
    even number of single quotes embedded w/o adjucent spaces.
    even number of double quotes embedded with adjucent spaces.
    odd number of double quotes embedded w/o adjucent spaces.

  Space (including newline) has no special powers.
  double quotes have no special powers.
  backslash has no special powers.
  Missing closing quote.

3. Inside double quotes.
  empty
  end of double quoted input
    double quote immediately followed by the null terminator
    even number of double quotes embedded w/o adjucent spaces.
    even number of single quotes embedded with adjucent spaces.
    odd number of single quotes embedded w/o adjucent spaces.

  Space (including newline) has no special powers.
  Single quotes have no special powers.

  A backslash immediately followed by a backslash or a double quote escapes
  immediately following backslash or double quote and is removed.
  A backslash-newline pair is replaced with a space.
  A backslash immediately followed by any other character is left intact.

  Missing closing quote.  */
static int
test (long n, int argc, char *argv[])
{
    int retcode;

    (void) argc;
    (void) argv;

    retcode = 0;

    switch (n)
      {
      case 0:
      /* Test memecspn.  */
      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\", "\\", 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\\\", "\\", 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "", separators, 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, " ", separators, 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello", separators, 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello ", separators, 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "\\", separators, 1);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "\\ ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "\\\t ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "\\\\\t ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "\\\\\\\t ", separators, 4);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\ ", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\ \t", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\ world", separators, 12);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\\\ ", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\\\\\ ", separators, 9);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\\\\\ \t", separators, 9);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn (__LINE__ - 1, "hello\\\\\\ world", separators, 14);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_memecspn_impl (__LINE__ - 1, "hello\\", 4, separators, 4);
        if (n)
          break;
        /* Fall through */

      /* Test strecspn.  */
      case __LINE__:
        test_strecspn (__LINE__ - 1, "", separators, 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, " ", separators, 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello", separators, 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello ", separators, 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\", separators, 1);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\ ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\\t ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\\\\t ", separators, 2);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\\\\\\t ", separators, 4);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\ ", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\ \t", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\ world", separators, 12);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\\\ ", separators, 7);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\\\\\ ", separators, 9);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\\\\\ \t", separators, 9);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\\\\\ world", separators, 14);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\", "\\", 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "hello\\\\", "\\", 5);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_strecspn (__LINE__ - 1, "\"", dquote_and_separators, 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash works as escape.  */
      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\\"", "\" \t\n", 2);
        if (n)
          break;
        /* Fall through */

      /* Backslash itself is rejected.  */
      case __LINE__:
        test_strecspn (__LINE__ - 1, "\\\"", "\" \t\n\\", 0);
        if (n)
          break;
        /* Fall through */


      /* No token. */
      /* Empty.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "", 0, 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "", 0, 0);
        if (n)
          break;
        /* Fall through */

      /* A lone separator.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, " ", 0, 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, " ", 0, 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple separators.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "   ", 0, 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "   ", 0, 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple distinct separators.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, " \t\n  ", 0, 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, " \t\n  ", 0, 0);
        if (n)
          break;
        /* Fall through */

      /* Test next_token and next_dequoted_token outside of quotes.
          end of token
            backslash immediately followed by the null terminator
            space immediately followed by the null terminator
            single quote immediately followed by the null terminator
            double quote immediately followed by the null terminator

          newline
          other space
          backslash not escaping anything,
          backslash escapes newline,
          backslash escapes space,
          backslash escapes single quote,
          backslash escapes double quote,
          backslash escapes backslash.  */

      /* Backslash not escaping anything.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\world", "hello\\world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\world", "helloworld", 0);
        if (n)
          break;
        /* Fall through */

      /* A not escaped backslash, followed by null terminator.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\", "\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\", "", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\\", "\\\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\\", "\\", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes newline.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\n", 0, 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\n", 0, 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes newline, followed by a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes newline, followed by a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\n\\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\n\\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\n \\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\n \\\nhello", "hello", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, " hello, world", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  " hello, world", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes space.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\\ hello, world", "\\ hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "\\ hello, world", " hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\\\\ hello, world", "\\\\", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "\\\\ hello, world", "\\",
                                  "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\\\nhello\\\n \\\nworld\\\n", "hello", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "\\\nhello\\\n \\\nworld\\\n ",
                                  "hello", "world", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\\\n \\\nhello\\\n \\\nworld", "hello", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "\\\n \\\nhello\\\n \\\nworld",
                                  "hello", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* A token, followed by a space  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello ", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello ", "hello", 0);
        if (n)
          break;
        /* Fall through */

      /* A token, followed by a backslash escaping newline.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\n", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\n", "hello", 0);
        if (n)
          break;
        /* Fall through */

      /* A token, followed by a backslash escaping backslash, followed by
         newline.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\\n", "hello\\\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\\\n", "hello\\", 0);
        if (n)
          break;
        /* Fall through */

      /* A lone escaped newline is not considered a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\n \\\n", "hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\n \\\n", "hello", 0);
        if (n)
          break;
        /* Fall through */

      /* A lone escaped newline is not considered a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\\n \\\n", "hello\\\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\\\\n \\\n", "hello\\", 0);
        if (n)
          break;
        /* Fall through */

      /* A token, followed by a backslash escaping backslash.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\", "hello\\\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\\", "hello\\", 0);
        if (n)
          break;
        /* Fall through */

      /* A lone escaped newline is not considered a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\ \\\n", "hello\\\\", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\\ \\\n", "hello\\", 0);
        if (n)
          break;
        /* Fall through */

      /* A lone escaped newline is not considered a token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\ \\\n", "hello\\ ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\ \\\n", "hello ", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash, followed by newline. Backslash-newline is replaced with a
         space. Two tokens.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello,\\\nworld", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello,\\\nworld", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash, escapes backslash, followed by newline. Backslash is
         removed and not escaped newline is removed. Two tokens.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "hello,\\\\\nworld", "hello,\\\\", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello,\\\\\nworld", "hello,\\", "world", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "hello,\\\\\\\nworld", "hello,\\\\", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello,\\\\\\\nworld",
                                  "hello,\\", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Two tokens.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello, world", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello, world", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "hello, world\n", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello, world\n", "hello,", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes space. Backslash removed, single token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello,\\ world", "hello,\\ world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello,\\ world", "hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash, followed by two spaces. Backslash removed, two tokens.
         First token contains the escaped space.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "hello,\\  world", "hello,\\ ", "world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello,\\  world", "hello, ", "world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes white space.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a\\ hello, world\\ b",
                         "a\\ hello,", "world\\ b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a\\ hello, world\\ b",
                                  "a hello,", "world b", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\'", "\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\'", "'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash, followed by a quote.
         Malformed token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\\'", "\\\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\\'", "\\\\'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\\\\'", "\\\\\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\\\\'", "\\'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\'hello", "\\'hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\'hello", "'hello", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a \\'hello, world\\' b",
                         "a", "\\'hello,", "world\\'", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a \\'hello, world\\' b",
                                  "a", "'hello,", "world'", "b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a 'hello one\" \"two world' b",
                         "a", "'hello one\" \"two world'", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a 'hello one\" \"two world' b",
                                  "a", "hello one\" \"two world", "b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "a 'hello one\\' \\'two world' b",
                            "a", "'hello one\\'", "\\'two", "world' b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "a \"hello one\\\" \\\"two world\" b",
                            "a", "\"hello one\\\" \\\"two world\"", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "a 'hello one\\' \\'two world b",
                                     "a", "hello one\\", "'two", "world", "b",
                                     0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "a \"hello one\\\" \\\"two world\" b",
                                     "a", "hello one\" \"two world", "b",
                                     0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a \\\"hello one\\\" \\\"two world b",
                                  "a", "\"hello", "one\"", "\"two", "world",
                                  "b", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash, followed by a quote.
         Malformed token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\\'hello", "\\\\'hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\\'hello", "\\\\'hello", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\\\\\\'hello", "\\\\\\'hello", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "\\\\\\'hello", "\\'hello", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\'", "hello\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\'", "hello'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash, followed by a quote.
         Malformed token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\'", "hello\\\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\\'", "hello\\\\'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\\\'", "hello\\\\\\'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "hello\\\\\\'", "hello\\'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\'world", "hello\\'world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\'world", "hello'world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash, followed by a quote.
         Malformed token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello\\\\'world", "hello\\\\'world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\\\'world", "hello\\\\'world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes a quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "hello\\\\\\'world", "hello\\\\\\'world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\\\\\'world", "hello\\'world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash escapes backslash, followed by a quote.
         Malformed token.  */
       case __LINE__:
        test_next_token (__LINE__ - 1,
                         "hello\\\\' world", "hello\\\\' world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello\\\\' world", "hello\\\\' world", 0);
        if (n)
          break;
        /* Fall through */


      /* Quotes.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "''", "''", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "''", "", 0);
        if (n)
          break;
        /* Fall through */




      /* Malformed token. A quote immediately followed by the null terminator.
         Closing quote is missing.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'", "'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'", "'", 0);
        if (n)
          break;
        /* Fall through */

      /* Malformed token. A quote immediately followed by the null terminator.
         Closing quote is missing.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'''", "'''", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'''", "'''", 0);
        if (n)
          break;
        /* Fall through */

      /* Malformed token. Leading separators are skipped.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, " '", "'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, " '", "'", 0);
        if (n)
          break;
        /* Fall through */

      /* Malformed token. Trailing separators stay intact.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "' ", "' ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "' ", "' ", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "'  ", "'  ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'  ", "'  ", 0);
        if (n)
          break;
        /* Fall through */

      /* Malformed token. Trailing separators stay intact.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'\\\n", "'\\\n", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'\\\n", "'\\\n", 0);
        if (n)
          break;
        /* Fall through */

      /* Even number of quotes.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "''''", "''''", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "''''", "", 0);
        if (n)
          break;
        /* Fall through */

      /* A wellformed token in quotes.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'hello, world'", "'hello, world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, world'", "hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* Missing closing quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'hello, world", "'hello, world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, world", "'hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* Missing opening quote.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "hello, world'", "hello,", "world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "hello, world'", "hello,", "world'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in quotes.  */
      /* Backslash in single or double quotes does not escape a space or tab.
         It is simply a lone backslash and it stays.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello,\\ world'", "'hello,\\ world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello,\\ world'", "hello,\\ world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in single quotes does not escape a newline.  It is simply a
         lone backslash and it stays.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello,\\\nworld'", "'hello,\\\nworld'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "'hello,\\\nworld'", "hello,\\\nworld", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in double quotes escapes a newline. A backslash newline pair
         is replaced with a space.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\"hello,\\\nworld\"", "\"hello,\\\nworld\"", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"hello,\\\nworld\"", "hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple consecutive backslash newline pairs along with leading and
         trailng space are all replaced with a single space .  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld\"",
                         "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld\"", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_i (
            __LINE__ - 1,
            "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld\"",
            "hello, world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_token (__LINE__ - 1, "\"a\\\nb\\\nc\"", "\"a\\\nb\\\nc\"", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_i (__LINE__ - 1,
                                    "\"a\\\nb\\\nc\"", "a b c", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld "
                         " bye,\\\n \t \\\n \t \\\n \t   \\\n  \\\nmoon\"",
                         "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld "
                         " bye,\\\n \t \\\n \t \\\n \t   \\\n  \\\nmoon\"",
                         0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_i (
            __LINE__ - 1,
            "\"hello, \t \\\n \t \\\n\t \t\\\n  \t\tworld "
            " bye,\\\n \t \\\n \t \\\n \t   \\\n  \\\nmoon\"",
            "hello, world  bye, moon", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in double quotes escapes a backslash.
         Backslash in single quotes has no special powers at all.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello,\\\\ world'", "'hello,\\\\ world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "'hello,\\\\ world'",
                                     "hello,\\\\ world", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"hello,\\\\ world\"",
                                     "hello,\\ world", 0);
        if (n)
          break;
        /* Fall through */

      /* The backslash does not escape the quote and this string is split into
         a wellformed token and a malformed token.*/
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "'hello,\\' world'", "'hello,\\'", "world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "'hello,\\' world'",
                                     "hello,\\", "world'", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in double quotes escapes a double quote. */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "\"hello,\\\" world\"", "\"hello,\\\" world\"", 0);
        if (n)
          break;
        /* Fall through */
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"hello,\\\" world\"",
                                     "hello,\" world", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in double quotes escapes a double quote, which leaves a
         dangling closing quote and makes a malformed second token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\"hello,\\\\\" world\"",
                         "\"hello,\\\\\"", "world\"", 0);
        if (n)
          break;
        /* Fall through */
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"hello,\\\\\" world\"",
                                     "hello,\\", "world\"", 0);
        if (n)
          break;
        /* Fall through */

      /* Backslash in double quotes escapes a double quote. */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "\"hello,\\\\\\\" world\"",
                            "\"hello,\\\\\\\" world\"", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"hello,\\\\\\\" world\"",
                                     "hello,\\\" world", 0);
        if (n)
          break;
        /* Fall through */

      /* Test that outside quotes a backslash-newline pair is replaced with a
         space, each not escaped backslash is removed and each backslash that
         escapes a backslash is removed.  */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "one\\\\two\\ tree\\\" four\\\nfive",
                            "one\\\\two\\ tree\\\"", "four", "five", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_i (__LINE__ - 1,
                                    "one\\\\two\\ tree\\\" four\\\nfive",
                                    "one\\two tree\"", "four", "five", 0);
        if (n)
          break;
        /* Fall through */


      /* Test that within double quotes a backslash that escapes a backslash or
         double quote is removed, a backslash-newline pair is replaced with a
         space and other backslashes stay.  */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "\"one\\\\two\\ tree\\\" four\\\nfive\"",
                            "\"one\\\\two\\ tree\\\" four\\\nfive\"", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_i (__LINE__ - 1,
                                    "\"one\\\\two\\ tree\\\" four\\\nfive\"",
                                    "one\\two\\ tree\" four five", 0);
        if (n)
          break;
        /* Fall through */

      /* Test that within single quotes only the opening and closing quotes are
         removed.  */
      case __LINE__:
        test_next_token_ws (__LINE__ - 1,
                            "'one\\\\two\\ tree\\\" four\\\nfive'",
                            "'one\\\\two\\ tree\\\" four\\\nfive'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "'one\\\\two\\ tree\\\" four\\\nfive'",
                                     "one\\\\two\\ tree\\\" four\\\nfive", 0);
        if (n)
          break;
        /* Fall through */

      /* Quotes within qoutes.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello, \"one two\" world'",
                         "'hello, \"one two\" world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, \"one two\" world'",
                                  "hello, \"one two\" world", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a 'hello one\\\\' \\\\'two world' b",
                         "a", "'hello one\\\\'", "\\\\'two world'", "b",
                         0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                  "a 'hello one\\\\' \\\\'two world' b",
                                  "a", "hello one\\\\", "\\two world", "b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                  "a \"hello one\\\\\" \\\\\"two world\" b",
                                  "a", "hello one\\", "\\two world", "b", 0);
        if (n)
          break;
        /* Fall through */


      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a 'hello one\\\\\\\\' \\\\\\\\'two world' b",
                         "a", "'hello one\\\\\\\\'",
                         "\\\\\\\\'two world'", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (
            __LINE__ - 1,
            "a 'hello one\\\\\\\\' \\\\\\\\'two world' b",
            "a", "hello one\\\\\\\\", "\\\\two world", "b",
            0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_dequoted_token_ws (
            __LINE__ - 1,
            "a \"hello one\\\\\\\\\" \\\\\\\\\"two world\" b",
            "a", "hello one\\\\", "\\\\two world", "b",
            0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'a \"one 'cd' two\" b'",
                         "'a \"one 'cd' two\" b'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'a \"one 'cd' two\" b'",
                                  "a \"one cd two\" b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'a \"one 'c d' two\" b'",
                         "'a \"one 'c", "d' two\" b'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'a \"one 'c d' two\" b'",
                                  "a \"one c", "d two\" b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a 'hell\"o''w\"orld' b",
                         "a", "'hell\"o''w\"orld'", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a 'hell\"o''w\"orld' b",
                                  "a", "hell\"ow\"orld", "b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "a \\'hell\"o\\'\\'w\"orld\\' b",
                         "a", "\\'hell\"o\\'\\'w\"orld\\'", "b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "a \\'hell\"o\\'\\'w\"orld\\' b",
                                  "a", "'hello\\'\\'world'", "b", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "\\'a \"one 'cd' two\" \\'b",
                         "\\'a", "\"one 'cd' two\"", "\\'b", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "\\'a \"one 'cd' two\" \\'b",
                                  "'a", "one 'cd' two", "'b", 0);
        if (n)
          break;
        /* Fall through */

      /* Trailing separator.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello, world' ", "'hello, world'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, world' ", "hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* Trailing escaped separator.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'hello'\\ ", "'hello'\\ ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'hello'\\ ", "hello ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'hello'\\\n", "hello", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello, world'\\ ", "'hello, world'\\ ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, world'\\ ", "hello, world ", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello, world'\\\n", "hello, world", 0);
        if (n)
          break;
        /* Fall through */

      /* A malformed token.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'a'b'c", "'a'b'c", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'a'b'c", "'a'b'c", 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple adjacent quoted tokens are treated as one.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "'a'b'c'", "'a'b'c'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "'a'b'c'", "abc", 0);
        if (n)
          break;
        /* Fall through */

      /* A malformed token ajacent to a good one.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "a'b'c'd", "a'b'c'd", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "a'b'c'd", "a'b'c'd", 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple adjacent quoted tokens are treated as one.  */
      case __LINE__:
        test_next_token (__LINE__ - 1, "a'b'c'd'", "a'b'c'd'", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1, "a'b'c'd'", "abcd", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "'\\\\'abc''", "'\\\\'abc''", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1, "'\\\\'abc''", "\\\\abc", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"\\\\\"abc\"\"", "\\abc", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "'\\'\\''", "'\\'\\''", 0);
        if (n)
          break;
        /* Fall through */
      /* A malformed token. Backslash has no special powers within single
         quotes.  */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1, "'\\'\\''", "'\\'\\''", 0);
        if (n)
          break;
        /* Fall through */
      /* A wellformed token. A backslash escapes a double quote within double
         quotes.  */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1, "\"\\\"\\\"\"", "\"\"", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1, "'\\\\'\\\\''", "'\\\\'\\\\''", 0);
        if (n)
          break;
        /* Fall through */
      /* A wellformed token. Backslash has no special powers within single
         quotes. The third backslash is outside single quotes and it escapes
         the following (forth) backslash.  */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1, "'\\\\'\\\\''", "\\\\\\", 0);
        if (n)
          break;
        /* Fall through */
      /* A wellformed token. A backslash escapes a backslash within double
         quotes.  */
      case __LINE__:
        test_next_dequoted_token_ws (__LINE__ - 1,
                                     "\"\\\\\"\\\\\"\"", "\\\\", 0);
        if (n)
          break;
        /* Fall through */

      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "  a  '  b  '  c  '  d  '  ",
                         "a",  "'  b  '",  "c",  "'  d  '" , 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "  a  '  b  '  c  '  d  '  ",
                                  "a", "  b  ", "c", "  d  ", 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple adjacent quoted tokens are treated as one.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "'hello'world\\'12''", "'hello'world\\'12''", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "'hello'world\\'12''", "helloworld'12", 0);
        if (n)
          break;
        /* Fall through */

      /* Multiple adjacent quoted tokens are treated as one.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
                         "01'2 3'45\"67 89\"ab\\ cd",
                         "01'2 3'45\"67 89\"ab\\ cd", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
                                  "01'2 3'45\"67 89\"ab\\ cd",
                                  "012 34567 89ab cd", 0);
        if (n)
          break;
        /* Fall through */

      /* An example from gnu make test suite.  */
      case __LINE__:
        test_next_token (__LINE__ - 1,
            "-w -E 'use warnings FATAL => \"all\";' -E",
            "-w", "-E", "'use warnings FATAL => \"all\";'", "-E", 0);
        if (n)
          break;
        /* Fall through */
      case __LINE__:
        test_next_dequoted_token (__LINE__ - 1,
            "-w -E 'use warnings FATAL => \"all\";' -E",
            "-w", "-E", "use warnings FATAL => \"all\";", "-E", 0);
        if (n)
          break;
        /* Fall through */

      /* A very long input.  */
      case __LINE__:
        {
          int k;
          char *input = (char*) malloc (1024 * 65 * 4);
          char *result;
          ASSERT (input, "cannot allocate 256K on heap\n");
          for (k = 0; k < 1024 * 65 * 2; ++k)
            input[k] = 'a' + k % 9;
          input[1024 * 65 * 2 - 1] = ' ';
          for (k = 1024 * 65 * 2; k < 1024 * 65 * 4; ++k)
            input[k] = 'f' + k % 7;
          input[1024 * 65 * 4 - 1] = '\0';
          result = strdup_ (input);
          result[1024 * 65 * 2 - 1] = '\0';
          test_next_token (__LINE__, input, result, result + 1024 * 65 * 2, 0);
          free (result);
          free (input);
          if (n)
            break;
        }
        /* Fall through */

      /* Allocate a long string.
         Populate with printable chars.
         Insert multiple token separators every 128 chars.
         Border with quotes.  */
      case __LINE__:
        {
          int k;
          char *input = (char*) malloc (1024 * 65 * 4);
          char *dequoted;
          ASSERT (input, "cannot allocate 256K on heap\n");
          for (k = 0; k < 1024 * 65 * 2; ++k)
            input[k] = 'a' + k % 9;
          for (k = 1; k < 1024 * 65 * 2; k += 128)
            input[k] = ' ';
          input[0] = '\'';
          input[1024 * 65 * 2 - 2] = '\'';
          input[1024 * 65 * 2 - 1] = ' ';

          for (k = 1024 * 65 * 2; k < 1024 * 65 * 4; ++k)
            input[k] = 'f' + k % 7;
          for (k = 1024 * 65 * 2 + 1; k < 1024 * 65 * 4; k += 128)
            input[k] = ' ';
          input[1024 * 65 * 2] = '\'';
          input[1024 * 65 * 4 - 2] = '\'';
          input[1024 * 65 * 4 - 1] = '\0';
          dequoted = strdup_ (input);
          dequoted[1024 * 65 * 2 - 2] = '\0';
          dequoted[1024 * 65 * 4 - 2] = '\0';
          test_next_dequoted_token (__LINE__, input, dequoted + 1,
                                    dequoted + 1024 * 65 * 2 + 1, 0);
          free (dequoted);
          free (input);
          if (n)
            break;
        }
        /* Fall through */

      default:
        retcode = -1;
        break;
      }
    return retcode;
}

static void
test_memecspn (int line, const char *s, const char *reject, size_t expected)
{
  test_memecspn_impl (line, s, strlen (s), reject, expected);
}

static void
test_memecspn_impl (int line, const char *s, size_t slen, const char *reject,
                    size_t expected)
{
  size_t r;

  printf ("test %d\n", line);
  r = memecspn (s, reject, slen);
  ASSERT (r == expected,
          "input = \"%s\", reject = \"%s\", expected = %lu, result = %lu, "
          "line = %d\n",
          s, reject, expected, r, line);
}

static void
test_strecspn (int line, const char *s, const char *reject, size_t expected)
{
  size_t r;

  printf ("test %d\n", line);
  r = strecspn (s, reject);
  ASSERT (r == expected,
          "input = \"%s\", reject = \"%s\", expected = %lu, result = %lu, "
          "line = %d\n",
          s, reject, expected, r, line);
}

/* Test next_token with the specified 'input', as well as modified
   'input' where each single quote is replaced with a double quote and each
   space is replaced with a tab and a newline.  */
static void
test_next_token (int line, const char *input, const char *expected, ...)
{
  const char *q;
  char *in;
  va_list ap;

  for (q = quotes; *q; ++q)
    {
      in = strdup_ (input);
      swapchr (in, '\'', *q);
      if ('\'' == *q || strcmp (in, input))
        {
          va_start (ap, expected);
          test_next_token_ws_impl (line, in, expected, ap, *q);
          va_end (ap);
        }
      free (in);
    }
}

/* Test next_token with the specified 'input', as well as modified
   'input' where each space is replaced with a tab and a newline.  */
static void
test_next_token_ws (int line, const char *input, const char *expected, ...)
{
  va_list ap;
  va_start (ap, expected);
  test_next_token_ws_impl (line, input, expected, ap, '\'');
  va_end (ap);
}

/* Test next_token with the specified 'input', as well as modified
   'input' where each space is replaced with a tab and a newline.  */
static void
test_next_token_ws_impl (int line, const char *input, const char *expected,
                         va_list ap, char c)
{
  const char *w;
  char *in;
  va_list aq;

  for (w = separators; *w; ++w)
    {
      /* An escaped space and and escaped newline are handled
         differently. */
      if (strstr (input, "\\ ") && *w == '\n')
        continue;
      in = strdup_ (input);
      subchr (in, ' ', *w);
      /* Avoid running the test again with the same input.  */
      if (' ' == *w || strcmp (in, input))
        {
          va_copy (aq, ap);
          test_next_token_impl (line, in, expected, aq, c, *w);
          va_end (aq);
        }
      free (in);
    }
}

static void
test_next_token_impl (int line, const char *input, const char *expected,
                      va_list ap, char c, char c2)
{
  const char *s = input, *t;
  size_t tlen, elen;
  int status = 0;

  printf ("test %d\n", line);
  do
    {
      elen = expected ? strlen (expected) : 0;
      t = next_token (&s, &tlen, &status);
      ASSERT (tlen == elen,
              "strlen (expected) = %lu, tlen = %lu, line = %d\n",
              elen, tlen, line);

      if (expected && *expected)
        {
          char *e;
          /* An escaped space and and escaped newline are handled
             differently. */
          if (strstr (expected, "\\ ") && c2 == '\n')
            continue;
          e = strdup_ (expected);
          swapchr (e, '\'', c);
          subchr (e, ' ', c2);
          ASSERT (memcmp (t, e, tlen) == 0,
                  "input = '%s', expected = '%s', result = '%.*s', line = %d\n",
                  input, e, (int) tlen, t, line);
          free (e);
        }
      else
        ASSERT (t == 0,
                "input = '%s', expected = '%s', result = '%.*s', line = %d\n",
                input, expected ? "" : "0", (int) tlen, t, line);
      expected = va_arg (ap, const char *);
    } while (expected);
    /* After went through all expected tokens, next next_token call
       should return 0.  This tests that all calls of test_next_token pass all
       expected tokens.  */
    t = next_token (&s, &tlen, &status);
    ASSERT (t == 0,
            "input = '%s', result = '%.*s', line = %d\n",
            input, (int) tlen, t, line);
}

/* Test next_dequoted_token with the specified 'input', as well as modified
   'input' where each single quote is replaced with a double quote and each
   space is replaced with a tab and a newline.  */
static void
test_next_dequoted_token (int line, const char *input,
                          const char *expected, ...)
{
  const char *q;
  char *i;
  va_list ap;

  for (q = quotes; *q; ++q)
    {
      i = strdup_ (input);
      swapchr (i, '\'', *q);
      if ('\'' == *q || strcmp (i, input))
        {
          va_start (ap, expected);
          test_next_dequoted_token_ws_impl (line, i, expected, ap, *q);
          va_end (ap);
        }
      free (i);
    }
}

/* Test next_dequoted_token with the specified 'input', as well as modified
   'input' where each space is replaced with a tab and a newline.  */
static void
test_next_dequoted_token_ws (int line, const char *input,
                             const char *expected, ...)
{
  va_list ap;
  va_start (ap, expected);
  test_next_dequoted_token_ws_impl (line, input, expected, ap, '\'');
  va_end (ap);
}

/* Test next_dequoted_token with the specified 'input'. Make no modifications
   to 'input'.  */
static void
test_next_dequoted_token_i (int line, const char *input,
                            const char *expected, ...)
{
  va_list ap;
  va_start (ap, expected);
  test_next_dequoted_token_impl (line, input, expected, ap, '\'', ' ');
  va_end (ap);
}

/* Test next_dequoted_token with the specified 'input', as well as modified
   'input' where each space is replaced with each character from the specified
   'ws'.  */
static void
test_next_dequoted_token_ws_impl (int line, const char *input,
                                  const char *expected, va_list ap,
                                  char c)
{
  const char *w;
  char *i;
  va_list aq;

  for (w = separators; *w; ++w)
    {
      /* An escaped space and and escaped newline are handled
         differently. */
      if (strstr (input, "\\ ") && *w == '\n')
        continue;

      i = strdup_ (input);
      subchr (i, ' ', *w);
      /* Avoid running the test again with the same input.  */
      if (' ' == *w || strcmp (i, input))
        {
          va_copy (aq, ap);
          test_next_dequoted_token_impl (line, i, expected, aq, c, *w);
          va_end (aq);
        }
      free (i);
    }
}

/* Test next_dequoted_token with the specified 'input'.  */
static void
test_next_dequoted_token_impl (int line, const char *input,
                               const char *expected, va_list ap, char c,
                               char c2)
{
  char *beg, *s, *t;
  size_t tlen, elen;
  int status = 0;

  printf ("test %d\n", line);

  beg = s = strdup_ (input);
  do
    {
      elen = expected ? strlen (expected) : 0;
      t = next_dequoted_token (&s, &tlen, &status);
      ASSERT (tlen == elen,
              "strlen (expected) = %lu, tlen = %lu, line = %d\n",
              elen, tlen, line);
      if (expected && *expected)
        {
          char *e = strdup_ (expected);
          assert (*input);
          swapchr (e, '\'', c);
          subchr (e, ' ', c2);
          ASSERT (memcmp (t, e, tlen) == 0,
                  "input = '%s', expected = '%s', result = '%.*s', line = %d\n",
                  input, e, (int) tlen, t, line);
          free (e);
        }
      else if (*input && expected && !*expected)
        ASSERT (t && *t == '\0',
                "input = '%s', expected = '%s', result = '%.*s', line = %d\n",
                input, expected, (int) tlen, t, line);
      else
        ASSERT (t == 0,
                "input = '%s', expected = '%s', result = '%.*s', line = %d\n",
                input, expected ? "" : "0", (int) tlen, t, line);
      expected = va_arg (ap, const char *);
    } while (expected);
  free (beg);
}

/* Duplicate a string and assert on success.  */
static char *
strdup_ (const char *s)
{
  char *r = strdup (s);
  assert (r);
  return r;
}

/* Substitute all instances of 'x' in 'input' with 'y'.  */
static char *
subchr (char *input, char x, char y)
{
  for (; *input; ++input)
    if (*input == x)
      *input = y;
  return input;
}

/* Substitute all instances of 'x' in 'input' with 'y'.
   Substitute all instances of 'y' in 'input' with 'x'.  */
static char *
swapchr (char *input, char x, char y)
{
  for (; *input; ++input)
    if (*input == x)
      *input = y;
    else if (*input == y)
      *input = x;
  return input;
}

/* Copyright (c) 2023 Dmitry Goncharov
 * dgoncharov@users.sf.net.
 *
 * Distributed under GPL v2 or the BSD License (see accompanying file COPYING),
 * your choice.
 */
