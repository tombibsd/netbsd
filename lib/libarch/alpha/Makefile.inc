#	$NetBSD$

.if (${MACHINE_ARCH} == "alpha")
SRCS+=	alpha_bus_window.c alpha_pci_conf.c alpha_pci_io.c alpha_pci_mem.c
.endif
