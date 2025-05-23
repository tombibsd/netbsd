/*	$Id$ */
/*
 * Copyright (c) 2009, 2010, 2011, 2012 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2013, 2014 Ingo Schwarze <schwarze@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

enum	rofferr {
	ROFF_CONT, /* continue processing line */
	ROFF_RERUN, /* re-run roff interpreter with offset */
	ROFF_APPEND, /* re-run main parser, appending next line */
	ROFF_REPARSE, /* re-run main parser on the result */
	ROFF_SO, /* include another file */
	ROFF_IGN, /* ignore current line */
	ROFF_TBL, /* a table row was successfully parsed */
	ROFF_EQN /* an equation was successfully parsed */
};

struct	buf {
	char	*buf;
	size_t	 sz;
};

__BEGIN_DECLS

struct	mparse;
struct	mchars;
struct	tbl_span;
struct	eqn;
struct	roff;
struct	mdoc;
struct	man;

void		 mandoc_msg(enum mandocerr, struct mparse *,
			int, int, const char *);
#if __GNUC__ - 0 >= 4
__attribute__((__format__ (__printf__, 5, 6)))
#endif
void		 mandoc_vmsg(enum mandocerr, struct mparse *,
			int, int, const char *, ...);
char		*mandoc_getarg(struct mparse *, char **, int, int *);
char		*mandoc_normdate(struct mparse *, char *, int, int);
int		 mandoc_eos(const char *, size_t);
int		 mandoc_strntoi(const char *, size_t, int);
const char	*mandoc_a2msec(const char*);

void		 mdoc_free(struct mdoc *);
struct	mdoc	*mdoc_alloc(struct roff *, struct mparse *,
			const char *, int);
void		 mdoc_reset(struct mdoc *);
int		 mdoc_parseln(struct mdoc *, int, char *, int);
void		 mdoc_endparse(struct mdoc *);
void		 mdoc_addspan(struct mdoc *, const struct tbl_span *);
void		 mdoc_addeqn(struct mdoc *, const struct eqn *);

void		 man_free(struct man *);
struct	man	*man_alloc(struct roff *, struct mparse *,
			const char *, int);
void		 man_reset(struct man *);
int		 man_parseln(struct man *, int, char *, int);
void		 man_endparse(struct man *);
void		 man_addspan(struct man *, const struct tbl_span *);
void		 man_addeqn(struct man *, const struct eqn *);

int		 preconv_cue(const struct buf *, size_t);
int		 preconv_encode(const struct buf *, size_t *,
			struct buf *, size_t *, int *);

void		 roff_free(struct roff *);
struct roff	*roff_alloc(struct mparse *, const struct mchars *, int);
void		 roff_reset(struct roff *);
enum rofferr	 roff_parseln(struct roff *, int, struct buf *, int *);
void		 roff_endparse(struct roff *);
void		 roff_setreg(struct roff *, const char *, int, char sign);
int		 roff_getreg(const struct roff *, const char *);
char		*roff_strdup(const struct roff *, const char *);
int		 roff_getcontrol(const struct roff *,
			const char *, int *);
int		 roff_getformat(const struct roff *);

const struct tbl_span	*roff_span(const struct roff *);
const struct eqn	*roff_eqn(const struct roff *);

__END_DECLS
