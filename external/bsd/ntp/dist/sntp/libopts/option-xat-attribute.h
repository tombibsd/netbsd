/*	$NetBSD$	*/

/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (stdin.h)
 *
 *  It has been AutoGen-ed
 *  From the definitions    stdin
 *  and the template file   str2enum
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Bruce Korb'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * str2enum IS PROVIDED BY Bruce Korb ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Bruce Korb OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Command/Keyword Dispatcher
 */
#ifndef STR2ENUM_OPTION_XAT_ATTRIBUTE_H_GUARD
#define STR2ENUM_OPTION_XAT_ATTRIBUTE_H_GUARD 1
#include <sys/types.h>
#ifndef MISSING_INTTYPES_H
# include <inttypes.h>
#endif

typedef enum {
    XAT_INVALID_CMD = 0,
    XAT_CMD_TYPE     = 1,
    XAT_CMD_WORDS    = 2,
    XAT_CMD_MEMBERS  = 3,
    XAT_CMD_COOKED   = 4,
    XAT_CMD_UNCOOKED = 5,
    XAT_CMD_KEEP     = 6,
    XAT_COUNT_CMD
} option_xat_attribute_enum_t;

extern option_xat_attribute_enum_t
find_option_xat_attribute_cmd(char const * str, size_t len);

#endif /* STR2ENUM_OPTION_XAT_ATTRIBUTE_H_GUARD */
/* end of option-xat-attribute.h */
