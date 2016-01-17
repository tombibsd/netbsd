#	$NetBSD$

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/lib/libusbhid

DPADD+=		${LIBUSBHID}
LDADD+=		-lusbhid

TESTS_C+=	t_usbhid

FILES=          test_usb_hid_usages
FILESDIR=       ${TESTSDIR}

.include <bsd.test.mk>
