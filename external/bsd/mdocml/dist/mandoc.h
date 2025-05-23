/*	$Id$ */
/*
 * Copyright (c) 2010, 2011, 2014 Kristaps Dzonsons <kristaps@bsd.lv>
 * Copyright (c) 2010-2015 Ingo Schwarze <schwarze@openbsd.org>
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

#define ASCII_NBRSP	 31  /* non-breaking space */
#define	ASCII_HYPH	 30  /* breakable hyphen */
#define	ASCII_BREAK	 29  /* breakable zero-width space */

/*
 * Status level.  This refers to both internal status (i.e., whilst
 * running, when warnings/errors are reported) and an indicator of a
 * threshold of when to halt (when said internal state exceeds the
 * threshold).
 */
enum	mandoclevel {
	MANDOCLEVEL_OK = 0,
	MANDOCLEVEL_RESERVED,
	MANDOCLEVEL_WARNING, /* warnings: syntax, whitespace, etc. */
	MANDOCLEVEL_ERROR, /* input has been thrown away */
	MANDOCLEVEL_UNSUPP, /* input needs unimplemented features */
	MANDOCLEVEL_BADARG, /* bad argument in invocation */
	MANDOCLEVEL_SYSERR, /* system error */
	MANDOCLEVEL_MAX
};

/*
 * All possible things that can go wrong within a parse, be it libroff,
 * libmdoc, or libman.
 */
enum	mandocerr {
	MANDOCERR_OK,

	MANDOCERR_WARNING, /* ===== start of warnings ===== */

	/* related to the prologue */
	MANDOCERR_DT_NOTITLE, /* missing manual title, using UNTITLED: line */
	MANDOCERR_TH_NOTITLE, /* missing manual title, using "": [macro] */
	MANDOCERR_TITLE_CASE, /* lower case character in document title */
	MANDOCERR_MSEC_MISSING, /* missing manual section, using "": macro */
	MANDOCERR_MSEC_BAD, /* unknown manual section: Dt ... section */
	MANDOCERR_DATE_MISSING, /* missing date, using today's date */
	MANDOCERR_DATE_BAD, /* cannot parse date, using it verbatim: date */
	MANDOCERR_OS_MISSING, /* missing Os macro, using "" */
	MANDOCERR_PROLOG_REP, /* duplicate prologue macro: macro */
	MANDOCERR_PROLOG_LATE, /* late prologue macro: macro */
	MANDOCERR_DT_LATE, /* skipping late title macro: Dt args */
	MANDOCERR_PROLOG_ORDER, /* prologue macros out of order: macros */

	/* related to document structure */
	MANDOCERR_SO, /* .so is fragile, better use ln(1): so path */
	MANDOCERR_DOC_EMPTY, /* no document body */
	MANDOCERR_SEC_BEFORE, /* content before first section header: macro */
	MANDOCERR_NAMESEC_FIRST, /* first section is not NAME: Sh title */
	MANDOCERR_NAMESEC_NONM, /* NAME section without name */
	MANDOCERR_NAMESEC_NOND, /* NAME section without description */
	MANDOCERR_NAMESEC_ND, /* description not at the end of NAME */
	MANDOCERR_NAMESEC_BAD, /* bad NAME section content: macro */
	MANDOCERR_ND_EMPTY, /* missing description line, using "" */
	MANDOCERR_SEC_ORDER, /* sections out of conventional order: Sh title */
	MANDOCERR_SEC_REP, /* duplicate section title: Sh title */
	MANDOCERR_SEC_MSEC, /* unexpected section: Sh title for ... only */
	MANDOCERR_XR_ORDER, /* unusual Xr order: ... after ... */
	MANDOCERR_XR_PUNCT, /* unusual Xr punctuation: ... after ... */
	MANDOCERR_AN_MISSING, /* AUTHORS section without An macro */

	/* related to macros and nesting */
	MANDOCERR_MACRO_OBS, /* obsolete macro: macro */
	MANDOCERR_MACRO_CALL, /* macro neither callable nor escaped: macro */
	MANDOCERR_PAR_SKIP, /* skipping paragraph macro: macro ... */
	MANDOCERR_PAR_MOVE, /* moving paragraph macro out of list: macro */
	MANDOCERR_NS_SKIP, /* skipping no-space macro */
	MANDOCERR_BLK_NEST, /* blocks badly nested: macro ... */
	MANDOCERR_BD_NEST, /* nested displays are not portable: macro ... */
	MANDOCERR_BL_MOVE, /* moving content out of list: macro */
	MANDOCERR_VT_CHILD, /* .Vt block has child macro: macro */
	MANDOCERR_FI_SKIP, /* fill mode already enabled, skipping: fi */
	MANDOCERR_NF_SKIP, /* fill mode already disabled, skipping: nf */
	MANDOCERR_BLK_LINE, /* line scope broken: macro breaks macro */

	/* related to missing arguments */
	MANDOCERR_REQ_EMPTY, /* skipping empty request: request */
	MANDOCERR_COND_EMPTY, /* conditional request controls empty scope */
	MANDOCERR_MACRO_EMPTY, /* skipping empty macro: macro */
	MANDOCERR_BLK_EMPTY, /* empty block: macro */
	MANDOCERR_ARG_EMPTY, /* empty argument, using 0n: macro arg */
	MANDOCERR_BD_NOTYPE, /* missing display type, using -ragged: Bd */
	MANDOCERR_BL_LATETYPE, /* list type is not the first argument: Bl arg */
	MANDOCERR_BL_NOWIDTH, /* missing -width in -tag list, using 8n */
	MANDOCERR_EX_NONAME, /* missing utility name, using "": Ex */
	MANDOCERR_FO_NOHEAD, /* missing function name, using "": Fo */
	MANDOCERR_IT_NOHEAD, /* empty head in list item: Bl -type It */
	MANDOCERR_IT_NOBODY, /* empty list item: Bl -type It */
	MANDOCERR_BF_NOFONT, /* missing font type, using \fR: Bf */
	MANDOCERR_BF_BADFONT, /* unknown font type, using \fR: Bf font */
	MANDOCERR_PF_SKIP, /* nothing follows prefix: Pf arg */
	MANDOCERR_RS_EMPTY, /* empty reference block: Rs */
	MANDOCERR_ARG_STD, /* missing -std argument, adding it: macro */
	MANDOCERR_OP_EMPTY, /* missing option string, using "": OP */
	MANDOCERR_UR_NOHEAD, /* missing resource identifier, using "": UR */
	MANDOCERR_EQN_NOBOX, /* missing eqn box, using "": op */

	/* related to bad arguments */
	MANDOCERR_ARG_QUOTE, /* unterminated quoted argument */
	MANDOCERR_ARG_REP, /* duplicate argument: macro arg */
	MANDOCERR_AN_REP, /* skipping duplicate argument: An -arg */
	MANDOCERR_BD_REP, /* skipping duplicate display type: Bd -type */
	MANDOCERR_BL_REP, /* skipping duplicate list type: Bl -type */
	MANDOCERR_BL_SKIPW, /* skipping -width argument: Bl -type */
	MANDOCERR_BL_COL, /* wrong number of cells */
	MANDOCERR_AT_BAD, /* unknown AT&T UNIX version: At version */
	MANDOCERR_FA_COMMA, /* comma in function argument: arg */
	MANDOCERR_FN_PAREN, /* parenthesis in function name: arg */
	MANDOCERR_RS_BAD, /* invalid content in Rs block: macro */
	MANDOCERR_SM_BAD, /* invalid Boolean argument: macro arg */
	MANDOCERR_FT_BAD, /* unknown font, skipping request: ft font */
	MANDOCERR_TR_ODD, /* odd number of characters in request: tr char */

	/* related to plain text */
	MANDOCERR_FI_BLANK, /* blank line in fill mode, using .sp */
	MANDOCERR_FI_TAB, /* tab in filled text */
	MANDOCERR_SPACE_EOL, /* whitespace at end of input line */
	MANDOCERR_COMMENT_BAD, /* bad comment style */
	MANDOCERR_ESC_BAD, /* invalid escape sequence: esc */
	MANDOCERR_STR_UNDEF, /* undefined string, using "": name */

	/* related to tables */
	MANDOCERR_TBLLAYOUT_SPAN, /* tbl line starts with span */
	MANDOCERR_TBLLAYOUT_DOWN, /* tbl column starts with span */
	MANDOCERR_TBLLAYOUT_VERT, /* skipping vertical bar in tbl layout */

	MANDOCERR_ERROR, /* ===== start of errors ===== */

	/* related to tables */
	MANDOCERR_TBLOPT_ALPHA, /* non-alphabetic character in tbl options */
	MANDOCERR_TBLOPT_BAD, /* skipping unknown tbl option: option */
	MANDOCERR_TBLOPT_NOARG, /* missing tbl option argument: option */
	MANDOCERR_TBLOPT_ARGSZ, /* wrong tbl option argument size: option */
	MANDOCERR_TBLLAYOUT_NONE, /* empty tbl layout */
	MANDOCERR_TBLLAYOUT_CHAR, /* invalid character in tbl layout: char */
	MANDOCERR_TBLLAYOUT_PAR, /* unmatched parenthesis in tbl layout */
	MANDOCERR_TBLDATA_NONE, /* tbl without any data cells */
	MANDOCERR_TBLDATA_SPAN, /* ignoring data in spanned tbl cell: data */
	MANDOCERR_TBLDATA_EXTRA, /* ignoring extra tbl data cells: data */
	MANDOCERR_TBLDATA_BLK, /* data block open at end of tbl: macro */

	/* related to document structure and macros */
	MANDOCERR_FILE, /* cannot open file */
	MANDOCERR_ROFFLOOP, /* input stack limit exceeded, infinite loop? */
	MANDOCERR_CHAR_BAD, /* skipping bad character: number */
	MANDOCERR_MACRO, /* skipping unknown macro: macro */
	MANDOCERR_REQ_INSEC, /* skipping insecure request: request */
	MANDOCERR_IT_STRAY, /* skipping item outside list: It ... */
	MANDOCERR_TA_STRAY, /* skipping column outside column list: Ta */
	MANDOCERR_BLK_NOTOPEN, /* skipping end of block that is not open */
	MANDOCERR_RE_NOTOPEN, /* fewer RS blocks open, skipping: RE arg */
	MANDOCERR_BLK_BROKEN, /* inserting missing end of block: macro ... */
	MANDOCERR_BLK_NOEND, /* appending missing end of block: macro */

	/* related to request and macro arguments */
	MANDOCERR_NAMESC, /* escaped character not allowed in a name: name */
	MANDOCERR_BD_FILE, /* NOT IMPLEMENTED: Bd -file */
	MANDOCERR_BL_NOTYPE, /* missing list type, using -item: Bl */
	MANDOCERR_NM_NONAME, /* missing manual name, using "": Nm */
	MANDOCERR_OS_UNAME, /* uname(3) system call failed, using UNKNOWN */
	MANDOCERR_ST_BAD, /* unknown standard specifier: St standard */
	MANDOCERR_IT_NONUM, /* skipping request without numeric argument */
	MANDOCERR_SO_PATH, /* NOT IMPLEMENTED: .so with absolute path or ".." */
	MANDOCERR_SO_FAIL, /* .so request failed */
	MANDOCERR_ARG_SKIP, /* skipping all arguments: macro args */
	MANDOCERR_ARG_EXCESS, /* skipping excess arguments: macro ... args */
	MANDOCERR_DIVZERO, /* divide by zero */

	MANDOCERR_UNSUPP, /* ===== start of unsupported features ===== */

	MANDOCERR_TOOLARGE, /* input too large */
	MANDOCERR_CHAR_UNSUPP, /* unsupported control character: number */
	MANDOCERR_REQ_UNSUPP, /* unsupported roff request: request */
	MANDOCERR_TBLOPT_EQN, /* eqn delim option in tbl: arg */
	MANDOCERR_TBLLAYOUT_MOD, /* unsupported tbl layout modifier: m */
	MANDOCERR_TBLMACRO, /* ignoring macro in table: macro */

	MANDOCERR_MAX
};

struct	tbl_opts {
	char		  tab; /* cell-separator */
	char		  decimal; /* decimal point */
	int		  opts;
#define	TBL_OPT_CENTRE	 (1 << 0)
#define	TBL_OPT_EXPAND	 (1 << 1)
#define	TBL_OPT_BOX	 (1 << 2)
#define	TBL_OPT_DBOX	 (1 << 3)
#define	TBL_OPT_ALLBOX	 (1 << 4)
#define	TBL_OPT_NOKEEP	 (1 << 5)
#define	TBL_OPT_NOSPACE	 (1 << 6)
#define	TBL_OPT_NOWARN	 (1 << 7)
	int		  cols; /* number of columns */
	int		  lvert; /* width of left vertical line */
	int		  rvert; /* width of right vertical line */
};

enum	tbl_cellt {
	TBL_CELL_CENTRE, /* c, C */
	TBL_CELL_RIGHT, /* r, R */
	TBL_CELL_LEFT, /* l, L */
	TBL_CELL_NUMBER, /* n, N */
	TBL_CELL_SPAN, /* s, S */
	TBL_CELL_LONG, /* a, A */
	TBL_CELL_DOWN, /* ^ */
	TBL_CELL_HORIZ, /* _, - */
	TBL_CELL_DHORIZ, /* = */
	TBL_CELL_MAX
};

/*
 * A cell in a layout row.
 */
struct	tbl_cell {
	struct tbl_cell	 *next;
	int		  vert; /* width of subsequent vertical line */
	enum tbl_cellt	  pos;
	size_t		  spacing;
	int		  col; /* column number, starting from 0 */
	int		  flags;
#define	TBL_CELL_TALIGN	 (1 << 0) /* t, T */
#define	TBL_CELL_BALIGN	 (1 << 1) /* d, D */
#define	TBL_CELL_BOLD	 (1 << 2) /* fB, B, b */
#define	TBL_CELL_ITALIC	 (1 << 3) /* fI, I, i */
#define	TBL_CELL_EQUAL	 (1 << 4) /* e, E */
#define	TBL_CELL_UP	 (1 << 5) /* u, U */
#define	TBL_CELL_WIGN	 (1 << 6) /* z, Z */
#define	TBL_CELL_WMAX	 (1 << 7) /* x, X */
};

/*
 * A layout row.
 */
struct	tbl_row {
	struct tbl_row	 *next;
	struct tbl_cell	 *first;
	struct tbl_cell	 *last;
	int		  vert; /* width of left vertical line */
};

enum	tbl_datt {
	TBL_DATA_NONE, /* has no data */
	TBL_DATA_DATA, /* consists of data/string */
	TBL_DATA_HORIZ, /* horizontal line */
	TBL_DATA_DHORIZ, /* double-horizontal line */
	TBL_DATA_NHORIZ, /* squeezed horizontal line */
	TBL_DATA_NDHORIZ /* squeezed double-horizontal line */
};

/*
 * A cell within a row of data.  The "string" field contains the actual
 * string value that's in the cell.  The rest is layout.
 */
struct	tbl_dat {
	struct tbl_cell	 *layout; /* layout cell */
	int		  spans; /* how many spans follow */
	struct tbl_dat	 *next;
	char		 *string; /* data (NULL if not TBL_DATA_DATA) */
	enum tbl_datt	  pos;
};

enum	tbl_spant {
	TBL_SPAN_DATA, /* span consists of data */
	TBL_SPAN_HORIZ, /* span is horizontal line */
	TBL_SPAN_DHORIZ /* span is double horizontal line */
};

/*
 * A row of data in a table.
 */
struct	tbl_span {
	struct tbl_opts	 *opts;
	struct tbl_row	 *layout; /* layout row */
	struct tbl_dat	 *first;
	struct tbl_dat	 *last;
	struct tbl_span	 *prev;
	struct tbl_span	 *next;
	int		  line; /* parse line */
	enum tbl_spant	  pos;
};

enum	eqn_boxt {
	EQN_ROOT, /* root of parse tree */
	EQN_TEXT, /* text (number, variable, whatever) */
	EQN_SUBEXPR, /* nested `eqn' subexpression */
	EQN_LIST, /* list (braces, etc.) */
	EQN_LISTONE, /* singleton list */
	EQN_PILE, /* vertical pile */
	EQN_MATRIX /* pile of piles */
};

enum	eqn_fontt {
	EQNFONT_NONE = 0,
	EQNFONT_ROMAN,
	EQNFONT_BOLD,
	EQNFONT_FAT,
	EQNFONT_ITALIC,
	EQNFONT__MAX
};

enum	eqn_post {
	EQNPOS_NONE = 0,
	EQNPOS_SUP,
	EQNPOS_SUBSUP,
	EQNPOS_SUB,
	EQNPOS_TO,
	EQNPOS_FROM,
	EQNPOS_FROMTO,
	EQNPOS_OVER,
	EQNPOS_SQRT,
	EQNPOS__MAX
};

enum	eqn_pilet {
	EQNPILE_NONE = 0,
	EQNPILE_PILE,
	EQNPILE_CPILE,
	EQNPILE_RPILE,
	EQNPILE_LPILE,
	EQNPILE_COL,
	EQNPILE_CCOL,
	EQNPILE_RCOL,
	EQNPILE_LCOL,
	EQNPILE__MAX
};

 /*
 * A "box" is a parsed mathematical expression as defined by the eqn.7
 * grammar.
 */
struct	eqn_box {
	int		  size; /* font size of expression */
#define	EQN_DEFSIZE	  INT_MIN
	enum eqn_boxt	  type; /* type of node */
	struct eqn_box	 *first; /* first child node */
	struct eqn_box	 *last; /* last child node */
	struct eqn_box	 *next; /* node sibling */
	struct eqn_box	 *prev; /* node sibling */
	struct eqn_box	 *parent; /* node sibling */
	char		 *text; /* text (or NULL) */
	char		 *left; /* fence left-hand */
	char		 *right; /* fence right-hand */
	char		 *top; /* expression over-symbol */
	char		 *bottom; /* expression under-symbol */
	size_t		  args; /* arguments in parent */
	size_t		  expectargs; /* max arguments in parent */
	enum eqn_post	  pos; /* position of next box */
	enum eqn_fontt	  font; /* font of box */
	enum eqn_pilet	  pile; /* equation piling */
};

/*
 * An equation consists of a tree of expressions starting at a given
 * line and position.
 */
struct	eqn {
	char		 *name; /* identifier (or NULL) */
	struct eqn_box	 *root; /* root mathematical expression */
	int		  ln; /* invocation line */
	int		  pos; /* invocation position */
};

/*
 * Parse options.
 */
#define	MPARSE_MDOC	1  /* assume -mdoc */
#define	MPARSE_MAN	2  /* assume -man */
#define	MPARSE_SO	4  /* honour .so requests */
#define	MPARSE_QUICK	8  /* abort the parse early */
#define	MPARSE_UTF8	16 /* accept UTF-8 input */
#define	MPARSE_LATIN1	32 /* accept ISO-LATIN-1 input */

enum	mandoc_esc {
	ESCAPE_ERROR = 0, /* bail! unparsable escape */
	ESCAPE_IGNORE, /* escape to be ignored */
	ESCAPE_SPECIAL, /* a regular special character */
	ESCAPE_FONT, /* a generic font mode */
	ESCAPE_FONTBOLD, /* bold font mode */
	ESCAPE_FONTITALIC, /* italic font mode */
	ESCAPE_FONTBI, /* bold italic font mode */
	ESCAPE_FONTROMAN, /* roman font mode */
	ESCAPE_FONTPREV, /* previous font mode */
	ESCAPE_NUMBERED, /* a numbered glyph */
	ESCAPE_UNICODE, /* a unicode codepoint */
	ESCAPE_NOSPACE, /* suppress space if the last on a line */
	ESCAPE_SKIPCHAR, /* skip the next character */
	ESCAPE_OVERSTRIKE /* overstrike all chars in the argument */
};

typedef	void	(*mandocmsg)(enum mandocerr, enum mandoclevel,
			const char *, int, int, const char *);

__BEGIN_DECLS

struct	mparse;
struct	mchars;
struct	mdoc;
struct	man;

enum mandoc_esc	  mandoc_escape(const char **, const char **, int *);
struct mchars	 *mchars_alloc(void);
void		  mchars_free(struct mchars *);
int		  mchars_num2char(const char *, size_t);
const char	 *mchars_uc2str(int);
int		  mchars_num2uc(const char *, size_t);
int		  mchars_spec2cp(const struct mchars *,
			const char *, size_t);
const char	 *mchars_spec2str(const struct mchars *,
			const char *, size_t, size_t *);
struct mparse	 *mparse_alloc(int, enum mandoclevel, mandocmsg,
			const struct mchars *, const char *);
void		  mparse_free(struct mparse *);
void		  mparse_keep(struct mparse *);
enum mandoclevel  mparse_open(struct mparse *, int *, const char *);
enum mandoclevel  mparse_readfd(struct mparse *, int, const char *);
enum mandoclevel  mparse_readmem(struct mparse *, const void *, size_t,
			const char *);
void		  mparse_reset(struct mparse *);
void		  mparse_result(struct mparse *,
			struct mdoc **, struct man **, char **);
const char	 *mparse_getkeep(const struct mparse *);
const char	 *mparse_strerror(enum mandocerr);
const char	 *mparse_strlevel(enum mandoclevel);
enum mandoclevel  mparse_wait(struct mparse *);

__END_DECLS
