# $NetBSD$

# Option for embedding a splashscreen image.
.if defined(SPLASHSCREEN_IMAGE) 
.include "${S}/dev/splash/splash.mk"
init_main.o: splash_image.o
.endif
