/*	$NetBSD$	*/
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user or with the express written consent of
 * Sun Microsystems, Inc.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <sys/cdefs.h>
#if defined(__RCSID) && !defined(lint)
#if 0
static char sccsid[] = "@(#)rpc_hout.c 1.12 89/02/22 (C) 1987 SMI";
#else
__RCSID("$NetBSD$");
#endif
#endif

/*
 * rpc_hout.c, Header file outputter for the RPC protocol compiler
 */
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include "rpc_scan.h"
#include "rpc_parse.h"
#include "rpc_util.h"

static void pconstdef(definition *);
static void pargdef(definition *);
static void pstructdef(definition *);
static void puniondef(definition *);
static void pdefine(const char *, const char *);
static void puldefine(const char *, const char *);
static int define_printed(proc_list *, version_list *);
static void pprogramdef(definition *);
static void penumdef(definition *);
static void ptypedef(definition *);
static int undefined2(const char *, const char *);
static void cplusplusstart(void);
static void cplusplusend(void);

/*
 * Print the C-version of an xdr definition
 */
void
print_datadef(definition *def)
{

	if (def->def_kind == DEF_PROGRAM)	/* handle data only */
		return;

	if (def->def_kind != DEF_CONST) {
		f_print(fout, "\n");
	}
	switch (def->def_kind) {
	case DEF_STRUCT:
		pstructdef(def);
		break;
	case DEF_UNION:
		puniondef(def);
		break;
	case DEF_ENUM:
		penumdef(def);
		break;
	case DEF_TYPEDEF:
		ptypedef(def);
		break;
	case DEF_PROGRAM:
		pprogramdef(def);
		break;
	case DEF_CONST:
		pconstdef(def);
		break;
	}
}

void
print_progdef(definition *def)
{
	switch (def->def_kind) {
	case DEF_PROGRAM:
		f_print(fout, "\n");
		pprogramdef(def);
		break;
	case DEF_CONST:
	case DEF_TYPEDEF:
	case DEF_ENUM:
	case DEF_UNION:
	case DEF_STRUCT:
		break;
	}
}

void
print_funcdef(definition *def, int *did)
{
	switch (def->def_kind) {
	case DEF_PROGRAM:
	case DEF_CONST:
		break;
	case DEF_TYPEDEF:
	case DEF_ENUM:
	case DEF_UNION:
	case DEF_STRUCT:
		if (!*did) {
		    f_print(fout, "\n");
		    cplusplusstart();
		    *did = 1;
		}
		pxdrfuncdecl(def->def_name,
		    def->def_kind != DEF_TYPEDEF ||
		    !isvectordef(def->def.ty.old_type, def->def.ty.rel));
		break;
	}
}

void
print_funcend(int did) {
	if (did) {
		cplusplusend();
	}
}

void
pxdrfuncdecl(const char *name, int pointerp)
{

	f_print(fout, "bool_t xdr_%s(XDR *, %s%s);\n", name,
	    name, pointerp ? (" *") : "");
}


static void
pconstdef(definition *def)
{
	pdefine(def->def_name, def->def.co);
}

/* print out the definitions for the arguments of functions in the
   header file
*/
static void
pargdef(definition *def)
{
	decl_list *l;
	version_list *vers;
	char   *name;
	proc_list *plist;
	int did;


	for (vers = def->def.pr.versions; vers != NULL; vers = vers->next) {
		for (plist = vers->procs; plist != NULL; plist = plist->next) {
			if (!newstyle || plist->arg_num < 2) {
				continue;	/* old style or single args */
			}
			name = plist->args.argname;
			f_print(fout, "struct %s {\n", name);
			for (l = plist->args.decls;
			    l != NULL; l = l->next) {
				pdeclaration(name, &l->decl, 1, ";\n");
			}
			f_print(fout, "};\n");
			f_print(fout, "typedef struct %s %s;\n", name, name);
		}
	}
	did = 0;
	for (vers = def->def.pr.versions; vers != NULL; vers = vers->next) {
		for (plist = vers->procs; plist != NULL; plist = plist->next) {
			if (!newstyle || plist->arg_num < 2) {
				continue;	/* old style or single args */
			}

			if (!did) {
				cplusplusstart();
				did = 1;
			}
			pxdrfuncdecl(plist->args.argname, 1);
		}
	}
	if (did) {
		cplusplusend();
	}

}


static void
pstructdef(definition *def)
{
	decl_list *l;
	const char *name = def->def_name;

	f_print(fout, "struct %s {\n", name);
	for (l = def->def.st.decls; l != NULL; l = l->next) {
		pdeclaration(name, &l->decl, 1, ";\n");
	}
	f_print(fout, "};\n");
	f_print(fout, "typedef struct %s %s;\n", name, name);
}

static void
puniondef(definition *def)
{
	case_list *l;
	const char *name = def->def_name;
	declaration *decl;

	f_print(fout, "struct %s {\n", name);
	decl = &def->def.un.enum_decl;
	if (streq(decl->type, "bool")) {
		f_print(fout, "\tbool_t %s;\n", decl->name);
	} else {
		f_print(fout, "\t%s %s;\n", decl->type, decl->name);
	}
	f_print(fout, "\tunion {\n");
	for (l = def->def.un.cases; l != NULL; l = l->next) {
		if (l->contflag == 0)
			pdeclaration(name, &l->case_decl, 2, ";\n");
	}
	decl = def->def.un.default_decl;
	if (decl && !streq(decl->type, "void")) {
		pdeclaration(name, decl, 2, ";\n");
	}
	f_print(fout, "\t} %s_u;\n", name);
	f_print(fout, "};\n");
	f_print(fout, "typedef struct %s %s;\n", name, name);
}

static void
pdefine(const char *name, const char *num)
{
	f_print(fout, "#define %s %s\n", name, num);
}

static void
puldefine(const char *name, const char *num)
{
	f_print(fout, "#define %s %s\n", name, num);
}

static int
define_printed(proc_list *stop, version_list *start)
{
	version_list *vers;
	proc_list *proc;

	for (vers = start; vers != NULL; vers = vers->next) {
		for (proc = vers->procs; proc != NULL; proc = proc->next) {
			if (proc == stop) {
				return (0);
			} else
				if (streq(proc->proc_name, stop->proc_name)) {
					return (1);
				}
		}
	}
	errx(1, "Internal error at %s:%d: procedure not found",
	    __FILE__, __LINE__);
	/* NOTREACHED */
}

static void
cplusplusstart(void)
{
	if (BSDflag)
		f_print(fout, "__BEGIN_DECLS\n");
	else
		f_print(fout, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n");
}

static void
cplusplusend(void)
{
	if (BSDflag)
		f_print(fout, "__END_DECLS\n");
	else
		f_print(fout, "#ifdef __cplusplus\n};\n#endif\n");
}

static void
pprogramdef(definition *def)
{
	version_list *vers;
	proc_list *proc;

	pargdef(def);

	puldefine(def->def_name, def->def.pr.prog_num);
	for (vers = def->def.pr.versions; vers != NULL; vers = vers->next) {
		if (tblflag) {
			f_print(fout, "extern struct rpcgen_table %s_%s_table[];\n",
			    locase(def->def_name), vers->vers_num);
			f_print(fout, "extern %s_%s_nproc;\n",
			    locase(def->def_name), vers->vers_num);
		}
		puldefine(vers->vers_name, vers->vers_num);
		for (proc = vers->procs; proc != NULL; proc = proc->next) {
			if (!define_printed(proc, def->def.pr.versions)) {
				puldefine(proc->proc_name, proc->proc_num);
			}
		}
	}

	/*
	 * Print out 3 definitions, one for ANSI-C, another for C++, a
	 * third for old style C
	 */
	f_print(fout, "\n");
	cplusplusstart();
	for (vers = def->def.pr.versions; vers != NULL; vers = vers->next) {
		for (proc = vers->procs; proc != NULL; proc = proc->next) {
			pprocdef(proc, vers, "CLIENT *", 0);
			pprocdef(proc, vers, "struct svc_req *", 1);
		}
	}
	cplusplusend();
}

void
pprocdef(proc_list *proc, version_list *vp, const char *addargtype,
     int server_p)
{
	decl_list *dl;

	if (Mflag) {
		if (server_p)
			f_print(fout, "bool_t ");
		else
			f_print(fout, "enum clnt_stat ");
	} else {
		ptype(proc->res_prefix, proc->res_type, 1);
		f_print(fout, "*");
	}
	if (server_p)
		pvname_svc(proc->proc_name, vp->vers_num);
	else
		pvname(proc->proc_name, vp->vers_num);

	f_print(fout, "(");
	if (proc->arg_num < 2 && newstyle &&
	    streq(proc->args.decls->decl.type, "void")) {
		/* 0 argument in new style:  do nothing */
	} else {
		for (dl = proc->args.decls; dl != NULL; dl = dl->next) {
			ptype(dl->decl.prefix, dl->decl.type, 1);
			if (!newstyle)
				f_print(fout, "*");
			f_print(fout, ", ");
		}
	}
	if (Mflag) {
		if (streq(proc->res_type, "void"))
			f_print(fout, "char");
		else
			ptype(proc->res_prefix, proc->res_type, 0);
		if (!isvectordef(proc->res_type, REL_ALIAS))
			f_print(fout, "*");
		f_print(fout, ", ");
	}
	f_print(fout, "%s);\n", addargtype);
}


static void
penumdef(definition *def)
{
	const char *name = def->def_name;
	enumval_list *l;
	const char *last = NULL;
	int count = 0;
	const char *first = "";

	f_print(fout, "enum %s {\n", name);
	for (l = def->def.en.vals; l != NULL; l = l->next) {
		f_print(fout, "%s\t%s", first, l->name);
		if (l->assignment) {
			f_print(fout, " = %s", l->assignment);
			last = l->assignment;
			count = 1;
		} else {
			if (last == NULL) {
				f_print(fout, " = %d", count++);
			} else {
				f_print(fout, " = %s + %d", last, count++);
			}
		}
		first = ",\n";
	}
	f_print(fout, "\n};\n");
	f_print(fout, "typedef enum %s %s;\n", name, name);
}

static void
ptypedef(definition *def)
{
	const char *name = def->def_name;
	const char *old = def->def.ty.old_type;
	char    prefix[8];	/* enough to contain "struct ", including NUL */
	relation rel = def->def.ty.rel;


	if (!streq(name, old)) {
		if (streq(old, "string")) {
			old = "char";
			rel = REL_POINTER;
		} else
			if (streq(old, "opaque")) {
				old = "char";
			} else
				if (streq(old, "bool")) {
					old = "bool_t";
				}
		if (undefined2(old, name) && def->def.ty.old_prefix) {
			s_print(prefix, "%s ", def->def.ty.old_prefix);
		} else {
			prefix[0] = 0;
		}
		f_print(fout, "typedef ");
		switch (rel) {
		case REL_ARRAY:
			f_print(fout, "struct {\n");
			f_print(fout, "\tunsigned int %s_len;\n", name);
			f_print(fout, "\t%s%s *%s_val;\n", prefix, old, name);
			f_print(fout, "} %s", name);
			break;
		case REL_POINTER:
			f_print(fout, "%s%s *%s", prefix, old, name);
			break;
		case REL_VECTOR:
			f_print(fout, "%s%s %s[%s]", prefix, old, name,
			    def->def.ty.array_max);
			break;
		case REL_ALIAS:
			f_print(fout, "%s%s %s", prefix, old, name);
			break;
		}
		f_print(fout, ";\n");
	}
}

void
pdeclaration(const char *name, declaration *dec, int tab,
	     const char *separator)
{
	char    buf[8];		/* enough to hold "struct ", include NUL */
	const char *prefix;
	const char *type;

	if (streq(dec->type, "void")) {
		return;
	}
	tabify(fout, tab);
	if (streq(dec->type, name) && !dec->prefix) {
		f_print(fout, "struct ");
	}
	if (streq(dec->type, "string")) {
		f_print(fout, "char *%s", dec->name);
	} else {
		prefix = "";
		if (streq(dec->type, "bool")) {
			type = "bool_t";
		} else
			if (streq(dec->type, "opaque")) {
				type = "char";
			} else {
				if (dec->prefix) {
					s_print(buf, "%s ", dec->prefix);
					prefix = buf;
				}
				type = dec->type;
			}
		switch (dec->rel) {
		case REL_ALIAS:
			f_print(fout, "%s%s %s", prefix, type, dec->name);
			break;
		case REL_VECTOR:
			f_print(fout, "%s%s %s[%s]", prefix, type, dec->name,
			    dec->array_max);
			break;
		case REL_POINTER:
			f_print(fout, "%s%s *%s", prefix, type, dec->name);
			break;
		case REL_ARRAY:
			f_print(fout, "struct {\n");
			tabify(fout, tab);
			f_print(fout, "\tunsigned int %s_len;\n", dec->name);
			tabify(fout, tab);
			f_print(fout, "\t%s%s *%s_val;\n", prefix, type, dec->name);
			tabify(fout, tab);
			f_print(fout, "} %s", dec->name);
			break;
		}
	}
	f_print(fout, "%s", separator);
}

static int
undefined2(const char *type, const char *stop)
{
	list   *l;
	definition *def;

	for (l = defined; l != NULL; l = l->next) {
		def = (definition *) l->val;
		if (def->def_kind != DEF_PROGRAM) {
			if (streq(def->def_name, stop)) {
				return (1);
			} else
				if (streq(def->def_name, type)) {
					return (0);
				}
		}
	}
	return (1);
}
