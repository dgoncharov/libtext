#include "token.h"
#include <string.h>
#include <stddef.h>
#include <assert.h>

static const char quotes_and_separators[] = "'\" \t\n";
static const char *separators = quotes_and_separators + 2;
static char *dequote (char *s, size_t *slen);
static char *quote_removal_in_double_quotes (char *s, size_t *slen);
static char *quote_removal_in_single_quotes (char *s, size_t *slen);
static int whitespace (char c);
static const char *skip_separators (const char *s);
static const char *skip_until_separator (const char *s);
static size_t strecspn (const char *s, const char *reject);
static size_t memecspn (const char *s, const char *reject, size_t slen);
static char *collapse_escaped_newlines (char *s, char *beg, const char **endp);


/* Find, validate and dequote the first token.

   If no token is found,
     - Keep *STATUS intact.
     - Point *S to the null terminator.
     - Set *TOKENLEN to 0.
     - Return 0.

   Otherwise, if a not escaped opening quote is present in the first token, but
   a corresponding closing quote is missing, then,
     - Set *STATUS to 1.
     - Point *S to the null terminator.
     - Set *TOKENLEN to the length of first token.
     - Return the beginning of the first token.

   Otherwise,
     - Keep *STATUS intact.
     - Point *S to the beginning of the next token.
     - Dequote S in place.
     - Set *TOKENLEN to the length of the dequoted first token.
     - Return the beginning of the first token.

   Dequoting rules.
   Outside of quotes
       - remove each not escaped backslash.
       - remove each backslash that escapes a backslash.
       - replace backslash-newline pair with a space.
   Within double quotes
       - remove the opening and closing double quotes.
       - remove each backslash escaping a backslash or double quote.
       - replace each backslash-newline pair with a space.
   Within single quotes.
       - remove the opening and closing single quotes.

   S is mutable, because next_dequoted_token dequotes the token in place.  */
char *
next_dequoted_token (char **s, size_t *tokenlen, int *status)
{
  int st = 0;
  char *token = (char*) next_quoted_token ((const char **)s, tokenlen, &st);
  *status |= st;
  if (token && st == 0)
    dequote (token, tokenlen);
  return token;
}

/* A not escaped and not quoted space, tab or newline character serves as a
   token separator.
   A not escaped backslash serves as an escape character.

   In the absence of quotes, a token is delimited by token separators.
   Beginning of a quoted token is delimited by a pair of a token separator
   followed, immediately or not, by a not escaped single or double quote. The
   end of a quoted token is delimited by the same single or not escaped double
   quote followed, immediately or not, by a token separator.  A space, tab or
   newline inside the quoted token does not serve as a token separator.
   Multiple adjacent quoted tokens, in other words, quoted tokens without
   interleaving separators, are treated as one token.
   E.g. hello'world'of'many'tokens is treated as one token.  */

/* Return the beginning of the first token in the specified string S.
   Set *TOKENLEN to the length of the token.  The quotes that delimit a quoted
   token are themself a part of the token and included to *TOKENLEN.
   Set *S to the beginning of the next token.
   If an opening quote is present and the closing quote is missing, set *STATUS
   to 1.  Otherwise, keep *STATUS intact.
   If no token is found, return 0.

   STATUS is a bitmask.
   The current implementation only sets bit 0. However, it is possible to
   modify next_quoted_token to detect $ or ` and set other bits in STATUS.  */
const char *
next_quoted_token (const char **input, size_t *tokenlen, int *status)
{
  const char *token, *s;

  assert (*input);

  *tokenlen = 0;
  *input = skip_separators (*input);
  if (**input == '\0')
    /* Only separators in this input.  */
    return 0;

  /* Point TOKEN at the beginning of the token.  */
  token = s = *input;

  /* An escaped newline is not treated like some other escaped space.
     An escaped newline is substituted with a space during quote removal phase,
     which serves as a token separator.  */

  for (;;)
    {
      /* Skip until a separator or quote.  */
      s = skip_until_separator (s);
      if (*s == '\0')
        /* Found the end of the token.  */
        break;

      if (*s == '\\' && s[1] == '\n')
        break;

      if (*s == '\'')
        {
          /* Advance past the opening quote.  */
          ++s;
          /* Skip until the next single quote, which is the closing quote.  */
          s += strcspn (s, "'");
          if (*s == '\0')
            {
              /* Closing quote is missing.  */
              *status |= 1;
              break;
            }
          assert (*s == '\'');
          /* Advance past the closing quote.  */
          ++s;
          if (whitespace (*s))
            /* Found the end of the token.  */
            break;
          continue;
        }

      if (*s == '"')
        {
          /* Advance past the opening quote.  */
          ++s;
          /* Skip until a not escaped double quote, which is the closing quote.  */
          s += strecspn (s, "\"");
          if (*s == '\0')
            {
              /* Closing quote is missing.  */
              *status |= 1;
              break;
            }
          assert (*s == '"');
          /* Advance past the closing quote.  */
          ++s;
          if (whitespace (*s))
            /* Found the end of the token.  */
            break;
          continue;
        }

      assert (strchr (separators, *s));
      /* Found the end of the token.  */
      break;
    }

  /* TOKEN points at the begininng of the token. S points immediately
     after the end of the token.  */
  *tokenlen = s - token;

  /* Move S to the beginning of the next token.  */
  s = skip_separators (s);
  *input = s;

  return token;
}

/* Dequote the initial portion of *SLEN characters of string S.
   Store the lenght of the dequoted portion to *SLEN.
   Null terminate the dequoted portion.
   Return S.  */
static char *
dequote (char *s, size_t *slen)
{
  char *beg = s;
  const char *end = s + *slen;
  size_t len = *slen;

  s += memecspn (s, "\\'\"", *slen);
  for (s = beg; s < end; )
    {
      len = end - s;
      switch (*s)
        {
        case '\'':
          s = quote_removal_in_single_quotes (s, &len);
          end = s + len;
          break;
        case '"':
          s = quote_removal_in_double_quotes (s, &len);
          end = s + len;
          break;
        case '\\':
          if (s[1] == '\n')
            {
              /* This backslash escapes the newline in S[1].  */
              /* Remove the backslash and the newline.  */
              memmove (s, s + 2, len - 2);
              end -= 2;
              break;
            }
          if (strchr ("'\"\\ \t", s[1]))
            {
              /* This backslash escapes S[1].  */
              /* Remove the backslash.  */
              memmove (s, s + 1, len - 1);
              --end;

              /* Keep intact the escaped character.  */
              ++s;
              break;
            }

          /* This is a lone backslash that does not escape anything
             interesting. Remove the backslash.  */
          memmove (s, s + 1, len - 1);
          --end;
          break;
        default:
          /* Keep intact a regualar character.  */
          ++s;
          break;
        }
    }

  assert (beg <= end);
  *slen = end - beg;
  beg[*slen] = '\0';

  return beg;
}

/* Within substring [S, S + SLEN) remove the opening and closing single quotes.

  Store the number of characters bewtween the end of the
  token and the end of the substring, remaining after quote removal.
  Return the end of the token.
 */
static char *
quote_removal_in_single_quotes (char *s, size_t *slen)
{
  const char *end = s + *slen;
  char *close;

  assert (*slen > 1); /* Due to STATUS == 0.  */
  assert (*s == '\'');

  close = (char*)  memchr (s + 1, '\'', *slen - 1);
  assert (close);
  assert (close > s);

  /* Remove closing single quote.  */
  memmove (close, close + 1, end - close);
  --end;

  /* Remove opening single quote.  */
  memmove (s, s + 1, end - s);
  --end;
  --close;
  *slen = end - close;

  return close;
}

/* Within substring [S, S + SLEN)
 - Remove the opening and closing double quotes.
 - Remove each backslash which escapes a double quote or backslash.
 - Replace each group of consecutive backslash-newline pairs along with
   surrounding space with a single space.

  Store the number of characters bewtween the end of the
  token and the end of the substring, remaining after quote removal.
  Return the end of the token.

  Because [S, S + SLEN) is one token, there is no need to care about single
  quotes.  */
static char *
quote_removal_in_double_quotes (char *s, size_t *slen)
{
  const char qe[] = {'\\', '"', '\0'};
  char *beg = s;
  const char *end = s + *slen;

  assert (*slen > 1); /* Due to STATUS == 0.  */
  assert (*s == '"');

  /* Remove the opening quote.  */
  memmove (s, s + 1, end - s);
  --end;

  for (;;)
    {
      s += memecspn (s, qe, end - s);
      if (s >= end)
        break;

      if (*s == '"')
        {
          /* Remove the closing quote.  */
          memmove (s, s + 1, end - s);
          --end;
          /* A not escaped double quote is the end of the double quoted
             substring.  */
          break;
        }

      assert (*s == '\\');

      if (s + 1 >= end)
        /* There is nothing to escape.  */
        break;

      if (s[1] == '\\' || s[1] == '"')
        {
          /* A backslash is escaping either a backslash or a quote.  */

          /* Remove the backslash.  */
          memmove (s, s + 1, end - s);
          --end;

          /* Keep intact the escaped character.  */
          ++s;

          continue;
        }

      if (s[1] == '\n')
        {
          s = collapse_escaped_newlines (s, beg, &end);
          continue;
        }

        /* Keep intact a lone backslash.  */
        ++s;
    }

  *slen = end - s;

  return s;
}

/* Return 1 if C is a newline, space or tab.
   Various shells, posix and gnu make do not honor form-feed, carriage return
   or vertical tab as token delimiters, even though isspace returns true for
   those characters.  */
static int
whitespace (char c)
{
    return c == '\n' || c == ' ' || c == '\t';
}

/* Skip separators and escaped newlines.  */
static const char *
skip_separators (const char *s)
{
  const char *beg;
  for (beg = 0; beg != s; )
    {
      beg = s;
      /* Skip separators.  */
      s += strspn (s, separators);

      /* Skip an escaped newline.  */
      if (*s == '\\' && s[1] == '\n')
        s += 2;
    }
  return s;
}

/* Skip until a separator or a newline or a not escaped newline.  */
static const char *
skip_until_separator (const char *s)
{
  const char escape = '\\';
  int n;

  for (n = 0; *s; ++s)
    {
      /* A backslash followed by newline is replaced with a space and thus
         serves as a separator. Therefore, any newline, escaped or not is a
         separator.  */
      if (*s == '\n')
        {
          s -= n % 2;
          break;
        }
      if (*s == escape)
        {
          ++n;
          continue;
        }
      if ((n % 2) == 0 && strchr (quotes_and_separators, (unsigned char) *s))
        break;
      n = 0;
    }

  return s;
}

/* Return the index of the first character in S that is present in REJECT and
   not escaped with a backslash.
   If backslash itself is present in REJECT, then nothing can be escaped,
   because lookup returns the index of that very backslash and strecspn behaves
   as strcspn.  */
static size_t
strecspn (const char *s, const char *reject)
{
  size_t result;
  int n = 0;
  const char escape = '\\';
  const char *escape_rejected = strchr (reject, (unsigned char) escape);

  for (result = 0; *s; ++s, ++result)
    {
      if (*s == escape)
        {
          ++n;
          if (escape_rejected)
            break;
          else
            continue;
        }

      if ((n % 2) == 0 && strchr (reject, (unsigned char) *s))
        break;

      n = 0;
    }

  return result;
}

/* Same as strecspn within the first SLEN characters of S.  */
static size_t
memecspn (const char *s, const char *reject, size_t slen)
{
  size_t result;
  int n = 0;
  const char escape = '\\';
  /* memchr along with RLEN, rather than strchr, is used to ensure
     that a \0 in [S, S+SLEN) does not match the null terminator in REJECT.  */
  size_t rlen = strlen (reject);
  const char *escape_rejected =
                         (char*) memchr (reject, (unsigned char) escape, rlen);

  for (result = 0; slen && *s; ++s, ++result, --slen)
    {
      if (*s == escape)
        {
          ++n;
          if (escape_rejected)
            break;
          else
            continue;
        }

      if ((n % 2) == 0 && memchr (reject, (unsigned char) *s, rlen))
        break;

      n = 0;
    }

  return result;
}

/* Within substring [BEG, *ENDP) replace all consecutive backslash-newline
  pairs along with surrounding space with a single space.
  Return the address immediately after the newly inserted space.
  Store the new end of the substring in *ENDP.  */
static char *
collapse_escaped_newlines (char *s, char *beg, const char **endp)
{
  char *p;
  const char *end = *endp;

  assert (beg < end);
  assert (beg <= s);
  assert (s < end);
  assert (*s == '\\');
  assert (s[1] == '\n');

  /* Convert all consecutive backslash-newline pairs along with
   * surrouding space to a single space.  */

  /* Walk back optional leading space.  */
  for (p = s - 1; beg < p && (*p == '\t' || *p == ' '); --p)
    ;
  ++p;

  /* Walk forward optional trailing space along with more
     backslash-newline pairs.  */
  while (s < end && ((*s == '\t' || *s == ' ') || (*s == '\\' && s[1] == '\n')))
    {
      for (; s < end && (*s == '\t' || *s == ' '); ++s)
        ;
      for (; s < end && (*s == '\\' && s[1] == '\n'); s += 2)
        ;
    }

  /* P points at the beginning of the leading space.
     S points immediately after trailing space and all consecutive
     backslash-newline pairs.
     Replace [P, S) with a single space.  */
  *p = ' ';
  ++p;
  memmove (p, s, end - s);
  *endp -= s - p;

  return p;
}

/* Copyright (c) 2023 Dmitry Goncharov
 * dgoncharov@users.sf.net.
 *
 * Distributed under GPL v2 or the BSD License (see accompanying file COPYING),
 * your choice.
 */
