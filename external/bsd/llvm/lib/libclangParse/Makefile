#	$NetBSD$

LIB=	clangParse

.include <bsd.init.mk>

.PATH: ${CLANG_SRCDIR}/lib/Parse

SRCS+=	ParseAST.cpp \
	ParseCXXInlineMethods.cpp \
	ParseDecl.cpp \
	ParseDeclCXX.cpp \
	ParseExpr.cpp \
	ParseExprCXX.cpp \
	ParseInit.cpp \
	ParseObjc.cpp \
	ParseOpenMP.cpp \
	ParsePragma.cpp \
	ParseStmt.cpp \
	ParseStmtAsm.cpp \
	ParseTemplate.cpp \
	ParseTentative.cpp \
	Parser.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
