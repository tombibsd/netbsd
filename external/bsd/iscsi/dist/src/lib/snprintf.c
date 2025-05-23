/*
 * Copyright (c) 1995-2001 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* From heimdal lib/roken/snprintf.c. */

#include "config.h"

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#if 0
RCSID("$Id$");
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if 0
#include <roken.h>
#endif

#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef max
#define max(a,b) ((a) > (b) ? (a) : (b))

enum format_flags {
    minus_flag     =  1,
    plus_flag      =  2,
    space_flag     =  4,
    alternate_flag =  8,
    zero_flag      = 16
};

/*
 * Common state
 */

struct state {
  unsigned char *str;
  unsigned char *s;
  unsigned char *theend;
  size_t sz;
  size_t max_sz;
  void (*append_char)(struct state *, unsigned char);
  /* XXX - methods */
};

#if TEST_SNPRINTF
#include "snprintf-test.h"
#endif /* TEST_SNPRINTF */

#if !defined(HAVE_VSNPRINTF) || defined(TEST_SNPRINTF)
static int
sn_reserve (struct state *state, size_t n)
{
  return state->s + n > state->theend;
}

static void
sn_append_char (struct state *state, unsigned char c)
{
  if (!sn_reserve (state, 1))
    *state->s++ = c;
}
#endif

static int
as_reserve (struct state *state, size_t n)
{
  if (state->s + n > state->theend) {
    int off = state->s - state->str;
    unsigned char *tmp;

    if (state->max_sz && state->sz >= state->max_sz)
      return 1;

    state->sz = max(state->sz * 2, state->sz + n);
    if (state->max_sz)
      state->sz = min(state->sz, state->max_sz);
    tmp = realloc (state->str, state->sz);
    if (tmp == NULL)
      return 1;
    state->str = tmp;
    state->s = state->str + off;
    state->theend = state->str + state->sz - 1;
  }
  return 0;
}

static void
as_append_char (struct state *state, unsigned char c)
{
  if(!as_reserve (state, 1))
    *state->s++ = c;
}

/* longest integer types */

#ifdef HAVE_LONG_LONG
typedef unsigned long long u_longest;
typedef long long longest;
#else
typedef unsigned long u_longest;
typedef long longest;
#endif

/*
 * is # supposed to do anything?
 */

static int
use_alternative (int flags, u_longest num, unsigned base)
{
  return flags & alternate_flag && (base == 16 || base == 8) && num != 0;
}

static int
append_number(struct state *state,
	      u_longest num, unsigned base, char *rep,
	      int width, int prec, int flags, int minusp)
{
  int len = 0;
  int i;
  u_longest n = num;

  /* given precision, ignore zero flag */
  if(prec != -1)
    flags &= ~zero_flag;
  else
    prec = 1;
  /* zero value with zero precision -> "" */
  if(prec == 0 && n == 0)
    return 0;
  do{
    (*state->append_char)(state, rep[n % base]);
    ++len;
    n /= base;
  } while(n);
  prec -= len;
  /* pad with prec zeros */
  while(prec-- > 0){
    (*state->append_char)(state, '0');
    ++len;
  }
  /* add length of alternate prefix (added later) to len */
  if(use_alternative(flags, num, base))
    len += base / 8;
  /* pad with zeros */
  if(flags & zero_flag){
    width -= len;
    if(minusp || (flags & space_flag) || (flags & plus_flag))
      width--;
    while(width-- > 0){
      (*state->append_char)(state, '0');
      len++;
    }
  }
  /* add alternate prefix */
  if(use_alternative(flags, num, base)){
    if(base == 16)
      (*state->append_char)(state, rep[10] + 23); /* XXX */
    (*state->append_char)(state, '0');
  }
  /* add sign */
  if(minusp){
    (*state->append_char)(state, '-');
    ++len;
  } else if(flags & plus_flag) {
    (*state->append_char)(state, '+');
    ++len;
  } else if(flags & space_flag) {
    (*state->append_char)(state, ' ');
    ++len;
  }
  if(flags & minus_flag)
    /* swap before padding with spaces */
    for(i = 0; i < len / 2; i++){
      char c = state->s[-i-1];
      state->s[-i-1] = state->s[-len+i];
      state->s[-len+i] = c;
    }
  width -= len;
  while(width-- > 0){
    (*state->append_char)(state,  ' ');
    ++len;
  }
  if(!(flags & minus_flag))
    /* swap after padding with spaces */
    for(i = 0; i < len / 2; i++){
      char c = state->s[-i-1];
      state->s[-i-1] = state->s[-len+i];
      state->s[-len+i] = c;
    }
  return len;
}

/*
 * return length
 */

static int
append_string (struct state *state,
	       const unsigned char *arg,
	       int width,
	       int prec,
	       int flags)
{
    int len = 0;

    if(arg == NULL)
	arg = (const unsigned char*)"(null)";

    if(prec != -1)
	width -= prec;
    else
	width -= strlen((const char *)arg);
    if(!(flags & minus_flag))
	while(width-- > 0) {
	    (*state->append_char) (state, ' ');
	    ++len;
	}
    if (prec != -1) {
	while (*arg && prec--) {
	    (*state->append_char) (state, *arg++);
	    ++len;
	}
    } else {
	while (*arg) {
	    (*state->append_char) (state, *arg++);
	    ++len;
	}
    }
    if(flags & minus_flag)
	while(width-- > 0) {
	    (*state->append_char) (state, ' ');
	    ++len;
	}
    return len;
}

static int
append_char(struct state *state,
	    unsigned char arg,
	    int width,
	    int flags)
{
  int len = 0;

  while(!(flags & minus_flag) && --width > 0) {
    (*state->append_char) (state, ' ')    ;
    ++len;
  }
  (*state->append_char) (state, arg);
  ++len;
  while((flags & minus_flag) && --width > 0) {
    (*state->append_char) (state, ' ');
    ++len;
  }
  return 0;
}

/*
 * This can't be made into a function...
 */

#ifdef HAVE_LONG_LONG

#define PARSE_INT_FORMAT(res, arg, unsig) \
if (long_long_flag) \
     res = (unsig long long)va_arg(arg, unsig long long); \
else if (long_flag) \
     res = (unsig long)va_arg(arg, unsig long); \
else if (short_flag) \
     res = (unsig short)va_arg(arg, unsig int); \
else \
     res = (unsig int)va_arg(arg, unsig int)

#else

#define PARSE_INT_FORMAT(res, arg, unsig) \
if (long_flag) \
     res = (unsig long)va_arg(arg, unsig long); \
else if (short_flag) \
     res = (unsig short)va_arg(arg, unsig int); \
else \
     res = (unsig int)va_arg(arg, unsig int)

#endif

/*
 * zyxprintf - return length, as snprintf
 */

static int
xyzprintf (struct state *state, const char *char_format, va_list ap)
{
  const unsigned char *format = (const unsigned char *)char_format;
  unsigned char c;
  int len = 0;

  while((c = *format++)) {
    if (c == '%') {
      int flags          = 0;
      int width          = 0;
      int prec           = -1;
      int long_long_flag = 0;
      int long_flag      = 0;
      int short_flag     = 0;

      /* flags */
      while((c = *format++)){
	if(c == '-')
	  flags |= minus_flag;
	else if(c == '+')
	  flags |= plus_flag;
	else if(c == ' ')
	  flags |= space_flag;
	else if(c == '#')
	  flags |= alternate_flag;
	else if(c == '0')
	  flags |= zero_flag;
	else
	  break;
      }
      
      if((flags & space_flag) && (flags & plus_flag))
	flags ^= space_flag;

      if((flags & minus_flag) && (flags & zero_flag))
	flags ^= zero_flag;

      /* width */
      if (isdigit(c))
	do {
	  width = width * 10 + c - '0';
	  c = *format++;
	} while(isdigit(c));
      else if(c == '*') {
	width = va_arg(ap, int);
	c = *format++;
      }

      /* precision */
      if (c == '.') {
	prec = 0;
	c = *format++;
	if (isdigit(c))
	  do {
	    prec = prec * 10 + c - '0';
	    c = *format++;
	  } while(isdigit(c));
	else if (c == '*') {
	  prec = va_arg(ap, int);
	  c = *format++;
	}
      }

      /* size */

      if (c == 'h') {
	short_flag = 1;
	c = *format++;
      } else if (c == 'l') {
	long_flag = 1;
	c = *format++;
	if (c == 'l') {
	    long_long_flag = 1;
	    c = *format++;
	}
      }

      switch (c) {
      case 'c' :
	append_char(state, va_arg(ap, int), width, flags);
	++len;
	break;
      case 's' :
	len += append_string(state,
			     va_arg(ap, unsigned char*),
			     width,
			     prec, 
			     flags);
	break;
      case 'd' :
      case 'i' : {
	longest arg;
	u_longest num;
	int minusp = 0;

	PARSE_INT_FORMAT(arg, ap, signed);

	if (arg < 0) {
	  minusp = 1;
	  num = -arg;
	} else
	  num = arg;

	len += append_number (state, num, 10, "0123456789",
			      width, prec, flags, minusp);
	break;
      }
      case 'u' : {
	u_longest arg;

	PARSE_INT_FORMAT(arg, ap, unsigned);

	len += append_number (state, arg, 10, "0123456789",
			      width, prec, flags, 0);
	break;
      }
      case 'o' : {
	u_longest arg;

	PARSE_INT_FORMAT(arg, ap, unsigned);

	len += append_number (state, arg, 010, "01234567",
			      width, prec, flags, 0);
	break;
      }
      case 'x' : {
	u_longest arg;

	PARSE_INT_FORMAT(arg, ap, unsigned);

	len += append_number (state, arg, 0x10, "0123456789abcdef",
			      width, prec, flags, 0);
	break;
      }
      case 'X' :{
	u_longest arg;

	PARSE_INT_FORMAT(arg, ap, unsigned);

	len += append_number (state, arg, 0x10, "0123456789ABCDEF",
			      width, prec, flags, 0);
	break;
      }
      case 'p' : {
	unsigned long arg = (unsigned long)va_arg(ap, void*);

	len += append_number (state, arg, 0x10, "0123456789ABCDEF",
			      width, prec, flags, 0);
	break;
      }
      case 'n' : {
	int *arg = va_arg(ap, int*);
	*arg = state->s - state->str;
	break;
      }
      case '\0' :
	  --format;
	  /* FALLTHROUGH */
      case '%' :
	(*state->append_char)(state, c);
	++len;
	break;
      default :
	(*state->append_char)(state, '%');
	(*state->append_char)(state, c);
	len += 2;
	break;
      }
    } else {
      (*state->append_char) (state, c);
      ++len;
    }
  }
  return len;
}

#if !defined(HAVE_SNPRINTF) || defined(TEST_SNPRINTF)
int
snprintf (char *str, size_t sz, const char *format, ...)
{
  va_list args;
  int ret;

  va_start(args, format);
  ret = vsnprintf (str, sz, format, args);
  va_end(args);

#ifdef PARANOIA
  {
    int ret2;
    char *tmp;

    tmp = malloc (sz);
    if (tmp == NULL)
      abort ();

    va_start(args, format);
    ret2 = vsprintf (tmp, format, args);
    va_end(args);
    if (ret != ret2 || strcmp(str, tmp))
      abort ();
    free (tmp);
  }
#endif

  return ret;
}
#endif

#if !defined(HAVE_ASPRINTF) || defined(TEST_SNPRINTF)
int
asprintf (char **ret, const char *format, ...)
{
  va_list args;
  int val;

  va_start(args, format);
  val = vasprintf (ret, format, args);

#ifdef PARANOIA
  {
    int ret2;
    char *tmp;
    tmp = malloc (val + 1);
    if (tmp == NULL)
      abort ();

    ret2 = vsprintf (tmp, format, args);
    if (val != ret2 || strcmp(*ret, tmp))
      abort ();
    free (tmp);
  }
#endif

  va_end(args);
  return val;
}
#endif

#if !defined(HAVE_VASPRINTF) || defined(TEST_SNPRINTF)
int
vasprintf (char **ret, const char *format, va_list args)
{
  return vasnprintf (ret, 0, format, args);
}
#endif


#if !defined(HAVE_VASNPRINTF) || defined(TEST_SNPRINTF)
int
vasnprintf (char **ret, size_t max_sz, const char *format, va_list args)
{
  int st;
  struct state state;

  state.max_sz = max_sz;
  state.sz     = 1;
  state.str    = malloc(state.sz);
  if (state.str == NULL) {
    *ret = NULL;
    return -1;
  }
  state.s = state.str;
  state.theend = state.s + state.sz - 1;
  state.append_char = as_append_char;

  st = xyzprintf (&state, format, args);
  if (st > state.sz) {
    free (state.str);
    *ret = NULL;
    return -1;
  } else {
    char *tmp;

    *state.s = '\0';
    tmp = realloc (state.str, st+1);
    if (tmp == NULL) {
      free (state.str);
      *ret = NULL;
      return -1;
    }
    *ret = tmp;
    return st;
  }
}
#endif

#if !defined(HAVE_VSNPRINTF) || defined(TEST_SNPRINTF)
int
vsnprintf (char *str, size_t sz, const char *format, va_list args)
{
  struct state state;
  int ret;
  unsigned char *ustr = (unsigned char *)str;

  state.max_sz = 0;
  state.sz     = sz;
  state.str    = ustr;
  state.s      = ustr;
  state.theend = ustr + sz - (sz > 0);
  state.append_char = sn_append_char;

  ret = xyzprintf (&state, format, args);
  if (state.s != NULL)
    *state.s = '\0';
  return ret;
}
#endif

#if !defined(HAVE_ASNPRINTF) || defined(TEST_SNPRINTF)
int
asnprintf (char **ret, size_t max_sz, const char *format, ...)
{
  va_list args;
  int val;

  va_start(args, format);
  val = vasnprintf (ret, max_sz, format, args);

#ifdef PARANOIA
  {
    int ret2;
    char *tmp;
    tmp = malloc (val + 1);
    if (tmp == NULL)
      abort ();

    ret2 = vsprintf (tmp, format, args);
    if (val != ret2 || strcmp(*ret, tmp))
      abort ();
    free (tmp);
  }
#endif

  va_end(args);
  return val;
}
#endif
