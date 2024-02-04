#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char *next_dequoted_token (char **s, size_t *tokenlen, int *status);
const char *next_quoted_token (const char **s, size_t *tokenlen, int *status);

#ifdef __cplusplus
}
#endif

#endif

/* Copyright (c) 2023 Dmitry Goncharov
 * dgoncharov@users.sf.net.
 *
 * Distributed under GPL v2 or the BSD License (see accompanying file COPYING),
 * your choice.
 */
