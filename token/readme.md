### Iterate over dequoted tokens in a C string.<br>

libtoken is a library written in C.<br>
libtoken provides a function to split an input C string to tokens, dequote the
tokens and iterate over the tokens. libtoken supports double and single quoting
and escaping with a backslash.<br>


### Examples.

The following is an example of a typical use of next_dequoted_token.<br>

```
while ((t = next_dequoted_token (&input, &len, &malformed)))
  argv[k++] = t;
```

This example demonstrates how to use libtoken to tokenize and dequote input
text, and then allocate and populate an argv with the dequoted tokens.<br>


```
#include "token.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <alloca.h>

int main (int ac, char *av[])
{
  long len;
  int status;
  char *input, *token;
  char **argv;
  int argc, k;
  FILE *infile;

  if (ac != 2)
    {
      fprintf (stderr, "%s <file>\n", av[0]);
      return 1;
    }

  infile = fopen (av[1], "r");
  if (!infile)
    {
      fprintf (stderr, "cannot open %s for reading: %s\n", av[1],
               strerror (errno));
      return 1;
    }

  /* Read the number of characters in the file.  */
  fseek (infile, 0, SEEK_END);
  len = ftell(infile);
  rewind(infile);

  input = alloca (len + 1);

  /* Read the contents of infile to a C string.  */
  fread (input, len, 1, infile);
  if (ferror (infile))
    {
      fprintf (stderr, "cannot read from %s: %s\n", av[1], strerror (errno));
      fclose (infile);
      return 1;
    }
  input[len] = '\0';
  fclose (infile);

  /* An input of length 'len' can carry at most 1 + len/2 tokens.  */
  argv = alloca ((len/2 + 3) * sizeof (char *));
  argc = 0;
  argv[argc++] = av[0];

  while ((token = next_dequoted_token (&input, (size_t *) &len, &status)))
    argv[argc++] = token;
  argv[argc] = 0; /* Null terminate argv.  */

  for (k = 0; k < argc; ++k)
    printf ("argv[%d] = %s\n", k, argv[k]);

  return 0;
}
```

Once this program is compiled, it can be used like the following.<br>

```
$ cat input.txt
one 'two three' four
"five six
seven"
eight\ nine
ten\\ eleven\
\
\
twelve\\\ thirteen\\
fourteen 'fifteen
sixteen\ seventeen
eighteen' nineteen
"twenty\
\
\
twentyone"
$ ./argv.tsk <input.txt
argv[0] = ./argv.tsk
argv[1] = one
argv[2] = two three
argv[3] = four
argv[4] = five six
seven
argv[5] = eight nine
argv[6] = ten\
argv[7] = eleven
argv[8] = twelve\ thirteen\
argv[9] = fourteen
argv[10] = fifteen
sixteen\ seventeen
eighteen
argv[11] = nineteen
argv[12] = twenty twentyone
```


### Tokenizing rules.

A not escaped and not quoted space, tab or newline character serves as a
token separator.<br>
A not escaped backslash serves as an escape character.<br>

In the absence of quotes, a token is delimited by token separators.<br>
Beginning of a quoted token is delimited by a pair of a token separator
followed, immediately or not, by a not escaped single or double quote. The
end of a quoted token is delimited by the same single or not escaped double
quote followed, immediately or not, by a token separator.  A space, tab or
newline inside the quoted token does not serve as a token separator.
Multiple adjacent quoted tokens, in other words, quoted tokens without
interleaving separators, are treated as one token.<br>
E.g. hello'world'of'many'tokens is treated as one token.<br>


### Dequoting rules.

Outside of quotes
  - Remove each not escaped backslash.
  - Remove each backslash that escapes a backslash.
  - Replace one or multiple consecutive backslash-newline pairs with a single
    space.<br>

Within double quotes
  - Remove opening and closing double quotes.
  - Remove each backslash escaping a backslash or double quote.
  - Replace one or multiple consecutive backslash-newline pairs with a single
    space.<br>

Within single quotes
  - Remove opening and closing single quotes.


These dequoting rules are the GNU Make dequoting rules and very similar to that
of bash.<br>
The only difference is how a backslashe followed by a newline is dequoted.<br>
Bash removes a backslash followed by a newline.<br>
libtoken collapsed multiple consecutive backslash-newline pairs and replaces a
backslash followed by a newline with a space.<br>
See token.h for details.<br>

Thus, out of two words, separated by a backslash-newline pair, bash produces
one token and libtoken produces two tokens.<br>

E.g. given input<br>

```
hello\
world
```

bash produces "helloworld", libtoken produces "hello world".<br>


### Build.

Create a subdirectory of any name in the libtoken source code directory and
step into this newly created subdirectory.<br>

```
$ mkdir l64
$ cd l64
```

Inside the subdirectory.<br>

To build libtoken<br>
```
$ make -f ../makefile
```

To run tests<br>
```
$ make -f ../makefile check
```

To install libtoken<br>
```
# make -f ../makefile install
```

To uninstall libtoken<br>
```
# make -f ../makefile uninstall
```

To build example argv.tsk<br>
```
$ make -f ../makefile argv.tsk
```

By default 64 bit binaries are built. Set environment variable BITNESS=32 to
build 32 bit binaries.

### License.

libtoken is distributed under the BSD license or GPLv2, your choice.<br>


### Copyright.

2023 Dmitry Goncharov<br>
dgoncharov@users.sf.net.<br>

Distributed under GPL v2 or the BSD License (see accompanying file COPYING),
your choice.
