/* This program demonstrates how to use libtoken to tokenize and dequote input
   text, and then allocate and populate an argv with the dequoted tokens.  */

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
