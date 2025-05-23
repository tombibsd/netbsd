/* $NetBSD$ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.49 2013/11/07 19:37:19 njoly Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/osf1/osf1.h>
#include <compat/osf1/osf1_syscallargs.h>
#else /* _KERNEL_OPT */
#include <sys/null.h>
#endif /* _KERNEL_OPT */

const char *const osf1_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"#5 (unimplemented old open)",
	/*   6 */	"close",
	/*   7 */	"wait4",
	/*   8 */	"#8 (unimplemented old creat)",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"#11 (unimplemented execv)",
	/*  12 */	"chdir",
	/*  13 */	"fchdir",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"__posix_chown",
	/*  17 */	"obreak",
	/*  18 */	"getfsstat",
	/*  19 */	"lseek",
	/*  20 */	"getpid_with_ppid",
	/*  21 */	"mount",
	/*  22 */	"unmount",
	/*  23 */	"setuid",
	/*  24 */	"getuid_with_euid",
	/*  25 */	"#25 (unimplemented exec_with_loader)",
	/*  26 */	"#26 (unimplemented ptrace)",
	/*  27 */	"recvmsg_xopen",
	/*  28 */	"sendmsg_xopen",
	/*  29 */	"#29 (unimplemented recvfrom)",
	/*  30 */	"#30 (unimplemented accept)",
	/*  31 */	"#31 (unimplemented getpeername)",
	/*  32 */	"#32 (unimplemented getsockname)",
	/*  33 */	"access",
	/*  34 */	"#34 (unimplemented chflags)",
	/*  35 */	"#35 (unimplemented fchflags)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"#38 (unimplemented old stat)",
	/*  39 */	"setpgid",
	/*  40 */	"#40 (unimplemented old lstat)",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"set_program_attributes",
	/*  44 */	"#44 (unimplemented profil)",
	/*  45 */	"open",
	/*  46 */	"#46 (obsolete sigaction)",
	/*  47 */	"getgid_with_egid",
	/*  48 */	"sigprocmask",
	/*  49 */	"__getlogin",
	/*  50 */	"__setlogin",
	/*  51 */	"acct",
	/*  52 */	"#52 (unimplemented sigpending)",
	/*  53 */	"classcntl",
	/*  54 */	"ioctl",
	/*  55 */	"reboot",
	/*  56 */	"revoke",
	/*  57 */	"symlink",
	/*  58 */	"readlink",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"#62 (unimplemented old fstat)",
	/*  63 */	"getpgrp",
	/*  64 */	"getpagesize",
	/*  65 */	"#65 (unimplemented mremap)",
	/*  66 */	"vfork",
	/*  67 */	"stat",
	/*  68 */	"lstat",
	/*  69 */	"#69 (unimplemented sbrk)",
	/*  70 */	"#70 (unimplemented sstk)",
	/*  71 */	"mmap",
	/*  72 */	"#72 (unimplemented ovadvise)",
	/*  73 */	"munmap",
	/*  74 */	"mprotect",
	/*  75 */	"madvise",
	/*  76 */	"#76 (unimplemented old vhangup)",
	/*  77 */	"#77 (unimplemented kmodcall)",
	/*  78 */	"#78 (unimplemented mincore)",
	/*  79 */	"getgroups",
	/*  80 */	"setgroups",
	/*  81 */	"#81 (unimplemented old getpgrp)",
	/*  82 */	"setpgrp",
	/*  83 */	"setitimer",
	/*  84 */	"#84 (unimplemented old wait)",
	/*  85 */	"#85 (unimplemented table)",
	/*  86 */	"getitimer",
	/*  87 */	"gethostname",
	/*  88 */	"sethostname",
	/*  89 */	"getdtablesize",
	/*  90 */	"dup2",
	/*  91 */	"fstat",
	/*  92 */	"fcntl",
	/*  93 */	"select",
	/*  94 */	"poll",
	/*  95 */	"fsync",
	/*  96 */	"setpriority",
	/*  97 */	"socket",
	/*  98 */	"connect",
	/*  99 */	"accept",
	/* 100 */	"getpriority",
	/* 101 */	"send",
	/* 102 */	"recv",
	/* 103 */	"sigreturn",
	/* 104 */	"bind",
	/* 105 */	"setsockopt",
	/* 106 */	"listen",
	/* 107 */	"#107 (unimplemented plock)",
	/* 108 */	"#108 (unimplemented old sigvec)",
	/* 109 */	"#109 (unimplemented old sigblock)",
	/* 110 */	"#110 (unimplemented old sigsetmask)",
	/* 111 */	"sigsuspend",
	/* 112 */	"sigstack",
	/* 113 */	"#113 (unimplemented old recvmsg)",
	/* 114 */	"#114 (unimplemented old sendmsg)",
	/* 115 */	"#115 (obsolete vtrace)",
	/* 116 */	"gettimeofday",
	/* 117 */	"getrusage",
	/* 118 */	"getsockopt",
	/* 119 */	"#119 (unimplemented)",
	/* 120 */	"readv",
	/* 121 */	"writev",
	/* 122 */	"settimeofday",
	/* 123 */	"__posix_fchown",
	/* 124 */	"fchmod",
	/* 125 */	"recvfrom",
	/* 126 */	"setreuid",
	/* 127 */	"setregid",
	/* 128 */	"__posix_rename",
	/* 129 */	"truncate",
	/* 130 */	"ftruncate",
	/* 131 */	"flock",
	/* 132 */	"setgid",
	/* 133 */	"sendto",
	/* 134 */	"shutdown",
	/* 135 */	"socketpair",
	/* 136 */	"mkdir",
	/* 137 */	"rmdir",
	/* 138 */	"utimes",
	/* 139 */	"#139 (obsolete 4.2 sigreturn)",
	/* 140 */	"#140 (unimplemented adjtime)",
	/* 141 */	"getpeername",
	/* 142 */	"gethostid",
	/* 143 */	"sethostid",
	/* 144 */	"getrlimit",
	/* 145 */	"setrlimit",
	/* 146 */	"#146 (unimplemented old killpg)",
	/* 147 */	"setsid",
	/* 148 */	"#148 (unimplemented quotactl)",
	/* 149 */	"quota",
	/* 150 */	"getsockname",
	/* 151 */	"#151 (unimplemented pread)",
	/* 152 */	"#152 (unimplemented pwrite)",
	/* 153 */	"#153 (unimplemented pid_block)",
	/* 154 */	"#154 (unimplemented pid_unblock)",
	/* 155 */	"#155 (unimplemented signal_urti)",
	/* 156 */	"sigaction",
	/* 157 */	"#157 (unimplemented sigwaitprim)",
	/* 158 */	"#158 (unimplemented nfssvc)",
	/* 159 */	"getdirentries",
	/* 160 */	"statfs",
	/* 161 */	"fstatfs",
	/* 162 */	"#162 (unimplemented)",
	/* 163 */	"#163 (unimplemented async_daemon)",
	/* 164 */	"#164 (unimplemented getfh)",
	/* 165 */	"getdomainname",
	/* 166 */	"setdomainname",
	/* 167 */	"#167 (unimplemented)",
	/* 168 */	"#168 (unimplemented)",
	/* 169 */	"#169 (unimplemented exportfs)",
	/* 170 */	"#170 (unimplemented)",
	/* 171 */	"#171 (unimplemented)",
	/* 172 */	"#172 (unimplemented alt msgctl)",
	/* 173 */	"#173 (unimplemented alt msgget)",
	/* 174 */	"#174 (unimplemented alt msgrcv)",
	/* 175 */	"#175 (unimplemented alt msgsnd)",
	/* 176 */	"#176 (unimplemented alt semctl)",
	/* 177 */	"#177 (unimplemented alt semget)",
	/* 178 */	"#178 (unimplemented alt semop)",
	/* 179 */	"#179 (unimplemented alt uname)",
	/* 180 */	"#180 (unimplemented)",
	/* 181 */	"#181 (unimplemented alt plock)",
	/* 182 */	"#182 (unimplemented lockf)",
	/* 183 */	"#183 (unimplemented)",
	/* 184 */	"#184 (unimplemented getmnt)",
	/* 185 */	"#185 (unimplemented)",
	/* 186 */	"#186 (unimplemented unmount)",
	/* 187 */	"#187 (unimplemented alt sigpending)",
	/* 188 */	"#188 (unimplemented alt setsid)",
	/* 189 */	"#189 (unimplemented)",
	/* 190 */	"#190 (unimplemented)",
	/* 191 */	"#191 (unimplemented)",
	/* 192 */	"#192 (unimplemented)",
	/* 193 */	"#193 (unimplemented)",
	/* 194 */	"#194 (unimplemented)",
	/* 195 */	"#195 (unimplemented)",
	/* 196 */	"#196 (unimplemented)",
	/* 197 */	"#197 (unimplemented)",
	/* 198 */	"#198 (unimplemented)",
	/* 199 */	"#199 (unimplemented swapon)",
	/* 200 */	"#200 (unimplemented msgctl)",
	/* 201 */	"#201 (unimplemented msgget)",
	/* 202 */	"#202 (unimplemented msgrcv)",
	/* 203 */	"#203 (unimplemented msgsnd)",
	/* 204 */	"#204 (unimplemented semctl)",
	/* 205 */	"#205 (unimplemented semget)",
	/* 206 */	"#206 (unimplemented semop)",
	/* 207 */	"uname",
	/* 208 */	"__posix_lchown",
	/* 209 */	"shmat",
	/* 210 */	"shmctl",
	/* 211 */	"shmdt",
	/* 212 */	"shmget",
	/* 213 */	"#213 (unimplemented mvalid)",
	/* 214 */	"#214 (unimplemented getaddressconf)",
	/* 215 */	"#215 (unimplemented msleep)",
	/* 216 */	"#216 (unimplemented mwakeup)",
	/* 217 */	"#217 (unimplemented msync)",
	/* 218 */	"#218 (unimplemented signal)",
	/* 219 */	"#219 (unimplemented utc gettime)",
	/* 220 */	"#220 (unimplemented utc adjtime)",
	/* 221 */	"#221 (unimplemented)",
	/* 222 */	"#222 (unimplemented security)",
	/* 223 */	"#223 (unimplemented kloadcall)",
	/* 224 */	"stat2",
	/* 225 */	"lstat2",
	/* 226 */	"fstat2",
	/* 227 */	"#227 (unimplemented statfs2)",
	/* 228 */	"#228 (unimplemented fstatfs2)",
	/* 229 */	"#229 (unimplemented getfsstat2)",
	/* 230 */	"#230 (unimplemented)",
	/* 231 */	"#231 (unimplemented)",
	/* 232 */	"#232 (unimplemented)",
	/* 233 */	"getpgid",
	/* 234 */	"getsid",
	/* 235 */	"sigaltstack",
	/* 236 */	"#236 (unimplemented waitid)",
	/* 237 */	"#237 (unimplemented priocntlset)",
	/* 238 */	"#238 (unimplemented sigsendset)",
	/* 239 */	"#239 (unimplemented set_speculative)",
	/* 240 */	"#240 (unimplemented msfs_syscall)",
	/* 241 */	"sysinfo",
	/* 242 */	"#242 (unimplemented uadmin)",
	/* 243 */	"#243 (unimplemented fuser)",
	/* 244 */	"#244 (unimplemented proplist_syscall)",
	/* 245 */	"#245 (unimplemented ntp_adjtime)",
	/* 246 */	"#246 (unimplemented ntp_gettime)",
	/* 247 */	"pathconf",
	/* 248 */	"fpathconf",
	/* 249 */	"#249 (unimplemented)",
	/* 250 */	"#250 (unimplemented uswitch)",
	/* 251 */	"usleep_thread",
	/* 252 */	"#252 (unimplemented audcntl)",
	/* 253 */	"#253 (unimplemented audgen)",
	/* 254 */	"#254 (unimplemented sysfs)",
	/* 255 */	"#255 (unimplemented subsys_info)",
	/* 256 */	"getsysinfo",
	/* 257 */	"setsysinfo",
	/* 258 */	"#258 (unimplemented afs_syscall)",
	/* 259 */	"#259 (unimplemented swapctl)",
	/* 260 */	"#260 (unimplemented memcntl)",
	/* 261 */	"#261 (unimplemented fdatasync)",
	/* 262 */	"#262 (unimplemented oflock)",
	/* 263 */	"#263 (unimplemented _F64_readv)",
	/* 264 */	"#264 (unimplemented _F64_writev)",
	/* 265 */	"#265 (unimplemented cdslxlate)",
	/* 266 */	"#266 (unimplemented sendfile)",
	/* 267 */	"# filler",
	/* 268 */	"# filler",
	/* 269 */	"# filler",
	/* 270 */	"# filler",
	/* 271 */	"# filler",
	/* 272 */	"# filler",
	/* 273 */	"# filler",
	/* 274 */	"# filler",
	/* 275 */	"# filler",
	/* 276 */	"# filler",
	/* 277 */	"# filler",
	/* 278 */	"# filler",
	/* 279 */	"# filler",
	/* 280 */	"# filler",
	/* 281 */	"# filler",
	/* 282 */	"# filler",
	/* 283 */	"# filler",
	/* 284 */	"# filler",
	/* 285 */	"# filler",
	/* 286 */	"# filler",
	/* 287 */	"# filler",
	/* 288 */	"# filler",
	/* 289 */	"# filler",
	/* 290 */	"# filler",
	/* 291 */	"# filler",
	/* 292 */	"# filler",
	/* 293 */	"# filler",
	/* 294 */	"# filler",
	/* 295 */	"# filler",
	/* 296 */	"# filler",
	/* 297 */	"# filler",
	/* 298 */	"# filler",
	/* 299 */	"# filler",
	/* 300 */	"# filler",
	/* 301 */	"# filler",
	/* 302 */	"# filler",
	/* 303 */	"# filler",
	/* 304 */	"# filler",
	/* 305 */	"# filler",
	/* 306 */	"# filler",
	/* 307 */	"# filler",
	/* 308 */	"# filler",
	/* 309 */	"# filler",
	/* 310 */	"# filler",
	/* 311 */	"# filler",
	/* 312 */	"# filler",
	/* 313 */	"# filler",
	/* 314 */	"# filler",
	/* 315 */	"# filler",
	/* 316 */	"# filler",
	/* 317 */	"# filler",
	/* 318 */	"# filler",
	/* 319 */	"# filler",
	/* 320 */	"# filler",
	/* 321 */	"# filler",
	/* 322 */	"# filler",
	/* 323 */	"# filler",
	/* 324 */	"# filler",
	/* 325 */	"# filler",
	/* 326 */	"# filler",
	/* 327 */	"# filler",
	/* 328 */	"# filler",
	/* 329 */	"# filler",
	/* 330 */	"# filler",
	/* 331 */	"# filler",
	/* 332 */	"# filler",
	/* 333 */	"# filler",
	/* 334 */	"# filler",
	/* 335 */	"# filler",
	/* 336 */	"# filler",
	/* 337 */	"# filler",
	/* 338 */	"# filler",
	/* 339 */	"# filler",
	/* 340 */	"# filler",
	/* 341 */	"# filler",
	/* 342 */	"# filler",
	/* 343 */	"# filler",
	/* 344 */	"# filler",
	/* 345 */	"# filler",
	/* 346 */	"# filler",
	/* 347 */	"# filler",
	/* 348 */	"# filler",
	/* 349 */	"# filler",
	/* 350 */	"# filler",
	/* 351 */	"# filler",
	/* 352 */	"# filler",
	/* 353 */	"# filler",
	/* 354 */	"# filler",
	/* 355 */	"# filler",
	/* 356 */	"# filler",
	/* 357 */	"# filler",
	/* 358 */	"# filler",
	/* 359 */	"# filler",
	/* 360 */	"# filler",
	/* 361 */	"# filler",
	/* 362 */	"# filler",
	/* 363 */	"# filler",
	/* 364 */	"# filler",
	/* 365 */	"# filler",
	/* 366 */	"# filler",
	/* 367 */	"# filler",
	/* 368 */	"# filler",
	/* 369 */	"# filler",
	/* 370 */	"# filler",
	/* 371 */	"# filler",
	/* 372 */	"# filler",
	/* 373 */	"# filler",
	/* 374 */	"# filler",
	/* 375 */	"# filler",
	/* 376 */	"# filler",
	/* 377 */	"# filler",
	/* 378 */	"# filler",
	/* 379 */	"# filler",
	/* 380 */	"# filler",
	/* 381 */	"# filler",
	/* 382 */	"# filler",
	/* 383 */	"# filler",
	/* 384 */	"# filler",
	/* 385 */	"# filler",
	/* 386 */	"# filler",
	/* 387 */	"# filler",
	/* 388 */	"# filler",
	/* 389 */	"# filler",
	/* 390 */	"# filler",
	/* 391 */	"# filler",
	/* 392 */	"# filler",
	/* 393 */	"# filler",
	/* 394 */	"# filler",
	/* 395 */	"# filler",
	/* 396 */	"# filler",
	/* 397 */	"# filler",
	/* 398 */	"# filler",
	/* 399 */	"# filler",
	/* 400 */	"# filler",
	/* 401 */	"# filler",
	/* 402 */	"# filler",
	/* 403 */	"# filler",
	/* 404 */	"# filler",
	/* 405 */	"# filler",
	/* 406 */	"# filler",
	/* 407 */	"# filler",
	/* 408 */	"# filler",
	/* 409 */	"# filler",
	/* 410 */	"# filler",
	/* 411 */	"# filler",
	/* 412 */	"# filler",
	/* 413 */	"# filler",
	/* 414 */	"# filler",
	/* 415 */	"# filler",
	/* 416 */	"# filler",
	/* 417 */	"# filler",
	/* 418 */	"# filler",
	/* 419 */	"# filler",
	/* 420 */	"# filler",
	/* 421 */	"# filler",
	/* 422 */	"# filler",
	/* 423 */	"# filler",
	/* 424 */	"# filler",
	/* 425 */	"# filler",
	/* 426 */	"# filler",
	/* 427 */	"# filler",
	/* 428 */	"# filler",
	/* 429 */	"# filler",
	/* 430 */	"# filler",
	/* 431 */	"# filler",
	/* 432 */	"# filler",
	/* 433 */	"# filler",
	/* 434 */	"# filler",
	/* 435 */	"# filler",
	/* 436 */	"# filler",
	/* 437 */	"# filler",
	/* 438 */	"# filler",
	/* 439 */	"# filler",
	/* 440 */	"# filler",
	/* 441 */	"# filler",
	/* 442 */	"# filler",
	/* 443 */	"# filler",
	/* 444 */	"# filler",
	/* 445 */	"# filler",
	/* 446 */	"# filler",
	/* 447 */	"# filler",
	/* 448 */	"# filler",
	/* 449 */	"# filler",
	/* 450 */	"# filler",
	/* 451 */	"# filler",
	/* 452 */	"# filler",
	/* 453 */	"# filler",
	/* 454 */	"# filler",
	/* 455 */	"# filler",
	/* 456 */	"# filler",
	/* 457 */	"# filler",
	/* 458 */	"# filler",
	/* 459 */	"# filler",
	/* 460 */	"# filler",
	/* 461 */	"# filler",
	/* 462 */	"# filler",
	/* 463 */	"# filler",
	/* 464 */	"# filler",
	/* 465 */	"# filler",
	/* 466 */	"# filler",
	/* 467 */	"# filler",
	/* 468 */	"# filler",
	/* 469 */	"# filler",
	/* 470 */	"# filler",
	/* 471 */	"# filler",
	/* 472 */	"# filler",
	/* 473 */	"# filler",
	/* 474 */	"# filler",
	/* 475 */	"# filler",
	/* 476 */	"# filler",
	/* 477 */	"# filler",
	/* 478 */	"# filler",
	/* 479 */	"# filler",
	/* 480 */	"# filler",
	/* 481 */	"# filler",
	/* 482 */	"# filler",
	/* 483 */	"# filler",
	/* 484 */	"# filler",
	/* 485 */	"# filler",
	/* 486 */	"# filler",
	/* 487 */	"# filler",
	/* 488 */	"# filler",
	/* 489 */	"# filler",
	/* 490 */	"# filler",
	/* 491 */	"# filler",
	/* 492 */	"# filler",
	/* 493 */	"# filler",
	/* 494 */	"# filler",
	/* 495 */	"# filler",
	/* 496 */	"# filler",
	/* 497 */	"# filler",
	/* 498 */	"# filler",
	/* 499 */	"# filler",
	/* 500 */	"# filler",
	/* 501 */	"# filler",
	/* 502 */	"# filler",
	/* 503 */	"# filler",
	/* 504 */	"# filler",
	/* 505 */	"# filler",
	/* 506 */	"# filler",
	/* 507 */	"# filler",
	/* 508 */	"# filler",
	/* 509 */	"# filler",
	/* 510 */	"# filler",
	/* 511 */	"# filler",
};


/* libc style syscall names */
const char *const altosf1_syscallnames[] = {
	/*   0 */	"nosys",
	/*   1 */	NULL, /* exit */
	/*   2 */	NULL, /* fork */
	/*   3 */	NULL, /* read */
	/*   4 */	NULL, /* write */
	/*   5 */	NULL, /* unimplemented old open */
	/*   6 */	NULL, /* close */
	/*   7 */	NULL, /* wait4 */
	/*   8 */	NULL, /* unimplemented old creat */
	/*   9 */	NULL, /* link */
	/*  10 */	NULL, /* unlink */
	/*  11 */	NULL, /* unimplemented execv */
	/*  12 */	NULL, /* chdir */
	/*  13 */	NULL, /* fchdir */
	/*  14 */	NULL, /* mknod */
	/*  15 */	NULL, /* chmod */
	/*  16 */	NULL, /* __posix_chown */
	/*  17 */	NULL, /* obreak */
	/*  18 */	NULL, /* getfsstat */
	/*  19 */	NULL, /* lseek */
	/*  20 */	NULL, /* getpid_with_ppid */
	/*  21 */	NULL, /* mount */
	/*  22 */	NULL, /* unmount */
	/*  23 */	NULL, /* setuid */
	/*  24 */	NULL, /* getuid_with_euid */
	/*  25 */	NULL, /* unimplemented exec_with_loader */
	/*  26 */	NULL, /* unimplemented ptrace */
	/*  27 */	NULL, /* recvmsg_xopen */
	/*  28 */	NULL, /* sendmsg_xopen */
	/*  29 */	NULL, /* unimplemented recvfrom */
	/*  30 */	NULL, /* unimplemented accept */
	/*  31 */	NULL, /* unimplemented getpeername */
	/*  32 */	NULL, /* unimplemented getsockname */
	/*  33 */	NULL, /* access */
	/*  34 */	NULL, /* unimplemented chflags */
	/*  35 */	NULL, /* unimplemented fchflags */
	/*  36 */	NULL, /* sync */
	/*  37 */	NULL, /* kill */
	/*  38 */	NULL, /* unimplemented old stat */
	/*  39 */	NULL, /* setpgid */
	/*  40 */	NULL, /* unimplemented old lstat */
	/*  41 */	NULL, /* dup */
	/*  42 */	NULL, /* pipe */
	/*  43 */	NULL, /* set_program_attributes */
	/*  44 */	NULL, /* unimplemented profil */
	/*  45 */	NULL, /* open */
	/*  46 */	NULL, /* obsolete sigaction */
	/*  47 */	NULL, /* getgid_with_egid */
	/*  48 */	NULL, /* sigprocmask */
	/*  49 */	NULL, /* __getlogin */
	/*  50 */	NULL, /* __setlogin */
	/*  51 */	NULL, /* acct */
	/*  52 */	NULL, /* unimplemented sigpending */
	/*  53 */	NULL, /* classcntl */
	/*  54 */	NULL, /* ioctl */
	/*  55 */	NULL, /* reboot */
	/*  56 */	NULL, /* revoke */
	/*  57 */	NULL, /* symlink */
	/*  58 */	NULL, /* readlink */
	/*  59 */	NULL, /* execve */
	/*  60 */	NULL, /* umask */
	/*  61 */	NULL, /* chroot */
	/*  62 */	NULL, /* unimplemented old fstat */
	/*  63 */	NULL, /* getpgrp */
	/*  64 */	NULL, /* getpagesize */
	/*  65 */	NULL, /* unimplemented mremap */
	/*  66 */	NULL, /* vfork */
	/*  67 */	NULL, /* stat */
	/*  68 */	NULL, /* lstat */
	/*  69 */	NULL, /* unimplemented sbrk */
	/*  70 */	NULL, /* unimplemented sstk */
	/*  71 */	NULL, /* mmap */
	/*  72 */	NULL, /* unimplemented ovadvise */
	/*  73 */	NULL, /* munmap */
	/*  74 */	NULL, /* mprotect */
	/*  75 */	NULL, /* madvise */
	/*  76 */	NULL, /* unimplemented old vhangup */
	/*  77 */	NULL, /* unimplemented kmodcall */
	/*  78 */	NULL, /* unimplemented mincore */
	/*  79 */	NULL, /* getgroups */
	/*  80 */	NULL, /* setgroups */
	/*  81 */	NULL, /* unimplemented old getpgrp */
	/*  82 */	"setpgid",
	/*  83 */	NULL, /* setitimer */
	/*  84 */	NULL, /* unimplemented old wait */
	/*  85 */	NULL, /* unimplemented table */
	/*  86 */	NULL, /* getitimer */
	/*  87 */	NULL, /* gethostname */
	/*  88 */	NULL, /* sethostname */
	/*  89 */	NULL, /* getdtablesize */
	/*  90 */	NULL, /* dup2 */
	/*  91 */	NULL, /* fstat */
	/*  92 */	NULL, /* fcntl */
	/*  93 */	NULL, /* select */
	/*  94 */	NULL, /* poll */
	/*  95 */	NULL, /* fsync */
	/*  96 */	NULL, /* setpriority */
	/*  97 */	NULL, /* socket */
	/*  98 */	NULL, /* connect */
	/*  99 */	NULL, /* accept */
	/* 100 */	NULL, /* getpriority */
	/* 101 */	NULL, /* send */
	/* 102 */	NULL, /* recv */
	/* 103 */	NULL, /* sigreturn */
	/* 104 */	NULL, /* bind */
	/* 105 */	NULL, /* setsockopt */
	/* 106 */	NULL, /* listen */
	/* 107 */	NULL, /* unimplemented plock */
	/* 108 */	NULL, /* unimplemented old sigvec */
	/* 109 */	NULL, /* unimplemented old sigblock */
	/* 110 */	NULL, /* unimplemented old sigsetmask */
	/* 111 */	NULL, /* sigsuspend */
	/* 112 */	NULL, /* sigstack */
	/* 113 */	NULL, /* unimplemented old recvmsg */
	/* 114 */	NULL, /* unimplemented old sendmsg */
	/* 115 */	NULL, /* obsolete vtrace */
	/* 116 */	NULL, /* gettimeofday */
	/* 117 */	NULL, /* getrusage */
	/* 118 */	NULL, /* getsockopt */
	/* 119 */	NULL, /* unimplemented */
	/* 120 */	NULL, /* readv */
	/* 121 */	NULL, /* writev */
	/* 122 */	NULL, /* settimeofday */
	/* 123 */	NULL, /* __posix_fchown */
	/* 124 */	NULL, /* fchmod */
	/* 125 */	NULL, /* recvfrom */
	/* 126 */	NULL, /* setreuid */
	/* 127 */	NULL, /* setregid */
	/* 128 */	NULL, /* __posix_rename */
	/* 129 */	NULL, /* truncate */
	/* 130 */	NULL, /* ftruncate */
	/* 131 */	NULL, /* flock */
	/* 132 */	NULL, /* setgid */
	/* 133 */	NULL, /* sendto */
	/* 134 */	NULL, /* shutdown */
	/* 135 */	NULL, /* socketpair */
	/* 136 */	NULL, /* mkdir */
	/* 137 */	NULL, /* rmdir */
	/* 138 */	NULL, /* utimes */
	/* 139 */	NULL, /* obsolete 4.2 sigreturn */
	/* 140 */	NULL, /* unimplemented adjtime */
	/* 141 */	NULL, /* getpeername */
	/* 142 */	NULL, /* gethostid */
	/* 143 */	NULL, /* sethostid */
	/* 144 */	NULL, /* getrlimit */
	/* 145 */	NULL, /* setrlimit */
	/* 146 */	NULL, /* unimplemented old killpg */
	/* 147 */	NULL, /* setsid */
	/* 148 */	NULL, /* unimplemented quotactl */
	/* 149 */	NULL, /* quota */
	/* 150 */	NULL, /* getsockname */
	/* 151 */	NULL, /* unimplemented pread */
	/* 152 */	NULL, /* unimplemented pwrite */
	/* 153 */	NULL, /* unimplemented pid_block */
	/* 154 */	NULL, /* unimplemented pid_unblock */
	/* 155 */	NULL, /* unimplemented signal_urti */
	/* 156 */	NULL, /* sigaction */
	/* 157 */	NULL, /* unimplemented sigwaitprim */
	/* 158 */	NULL, /* unimplemented nfssvc */
	/* 159 */	NULL, /* getdirentries */
	/* 160 */	NULL, /* statfs */
	/* 161 */	NULL, /* fstatfs */
	/* 162 */	NULL, /* unimplemented */
	/* 163 */	NULL, /* unimplemented async_daemon */
	/* 164 */	NULL, /* unimplemented getfh */
	/* 165 */	NULL, /* getdomainname */
	/* 166 */	NULL, /* setdomainname */
	/* 167 */	NULL, /* unimplemented */
	/* 168 */	NULL, /* unimplemented */
	/* 169 */	NULL, /* unimplemented exportfs */
	/* 170 */	NULL, /* unimplemented */
	/* 171 */	NULL, /* unimplemented */
	/* 172 */	NULL, /* unimplemented alt msgctl */
	/* 173 */	NULL, /* unimplemented alt msgget */
	/* 174 */	NULL, /* unimplemented alt msgrcv */
	/* 175 */	NULL, /* unimplemented alt msgsnd */
	/* 176 */	NULL, /* unimplemented alt semctl */
	/* 177 */	NULL, /* unimplemented alt semget */
	/* 178 */	NULL, /* unimplemented alt semop */
	/* 179 */	NULL, /* unimplemented alt uname */
	/* 180 */	NULL, /* unimplemented */
	/* 181 */	NULL, /* unimplemented alt plock */
	/* 182 */	NULL, /* unimplemented lockf */
	/* 183 */	NULL, /* unimplemented */
	/* 184 */	NULL, /* unimplemented getmnt */
	/* 185 */	NULL, /* unimplemented */
	/* 186 */	NULL, /* unimplemented unmount */
	/* 187 */	NULL, /* unimplemented alt sigpending */
	/* 188 */	NULL, /* unimplemented alt setsid */
	/* 189 */	NULL, /* unimplemented */
	/* 190 */	NULL, /* unimplemented */
	/* 191 */	NULL, /* unimplemented */
	/* 192 */	NULL, /* unimplemented */
	/* 193 */	NULL, /* unimplemented */
	/* 194 */	NULL, /* unimplemented */
	/* 195 */	NULL, /* unimplemented */
	/* 196 */	NULL, /* unimplemented */
	/* 197 */	NULL, /* unimplemented */
	/* 198 */	NULL, /* unimplemented */
	/* 199 */	NULL, /* unimplemented swapon */
	/* 200 */	NULL, /* unimplemented msgctl */
	/* 201 */	NULL, /* unimplemented msgget */
	/* 202 */	NULL, /* unimplemented msgrcv */
	/* 203 */	NULL, /* unimplemented msgsnd */
	/* 204 */	NULL, /* unimplemented semctl */
	/* 205 */	NULL, /* unimplemented semget */
	/* 206 */	NULL, /* unimplemented semop */
	/* 207 */	NULL, /* uname */
	/* 208 */	NULL, /* __posix_lchown */
	/* 209 */	NULL, /* shmat */
	/* 210 */	NULL, /* shmctl */
	/* 211 */	NULL, /* shmdt */
	/* 212 */	NULL, /* shmget */
	/* 213 */	NULL, /* unimplemented mvalid */
	/* 214 */	NULL, /* unimplemented getaddressconf */
	/* 215 */	NULL, /* unimplemented msleep */
	/* 216 */	NULL, /* unimplemented mwakeup */
	/* 217 */	NULL, /* unimplemented msync */
	/* 218 */	NULL, /* unimplemented signal */
	/* 219 */	NULL, /* unimplemented utc gettime */
	/* 220 */	NULL, /* unimplemented utc adjtime */
	/* 221 */	NULL, /* unimplemented */
	/* 222 */	NULL, /* unimplemented security */
	/* 223 */	NULL, /* unimplemented kloadcall */
	/* 224 */	NULL, /* stat2 */
	/* 225 */	NULL, /* lstat2 */
	/* 226 */	NULL, /* fstat2 */
	/* 227 */	NULL, /* unimplemented statfs2 */
	/* 228 */	NULL, /* unimplemented fstatfs2 */
	/* 229 */	NULL, /* unimplemented getfsstat2 */
	/* 230 */	NULL, /* unimplemented */
	/* 231 */	NULL, /* unimplemented */
	/* 232 */	NULL, /* unimplemented */
	/* 233 */	NULL, /* getpgid */
	/* 234 */	NULL, /* getsid */
	/* 235 */	NULL, /* sigaltstack */
	/* 236 */	NULL, /* unimplemented waitid */
	/* 237 */	NULL, /* unimplemented priocntlset */
	/* 238 */	NULL, /* unimplemented sigsendset */
	/* 239 */	NULL, /* unimplemented set_speculative */
	/* 240 */	NULL, /* unimplemented msfs_syscall */
	/* 241 */	NULL, /* sysinfo */
	/* 242 */	NULL, /* unimplemented uadmin */
	/* 243 */	NULL, /* unimplemented fuser */
	/* 244 */	NULL, /* unimplemented proplist_syscall */
	/* 245 */	NULL, /* unimplemented ntp_adjtime */
	/* 246 */	NULL, /* unimplemented ntp_gettime */
	/* 247 */	NULL, /* pathconf */
	/* 248 */	NULL, /* fpathconf */
	/* 249 */	NULL, /* unimplemented */
	/* 250 */	NULL, /* unimplemented uswitch */
	/* 251 */	NULL, /* usleep_thread */
	/* 252 */	NULL, /* unimplemented audcntl */
	/* 253 */	NULL, /* unimplemented audgen */
	/* 254 */	NULL, /* unimplemented sysfs */
	/* 255 */	NULL, /* unimplemented subsys_info */
	/* 256 */	NULL, /* getsysinfo */
	/* 257 */	NULL, /* setsysinfo */
	/* 258 */	NULL, /* unimplemented afs_syscall */
	/* 259 */	NULL, /* unimplemented swapctl */
	/* 260 */	NULL, /* unimplemented memcntl */
	/* 261 */	NULL, /* unimplemented fdatasync */
	/* 262 */	NULL, /* unimplemented oflock */
	/* 263 */	NULL, /* unimplemented _F64_readv */
	/* 264 */	NULL, /* unimplemented _F64_writev */
	/* 265 */	NULL, /* unimplemented cdslxlate */
	/* 266 */	NULL, /* unimplemented sendfile */
	/* 267 */	NULL, /* filler */
	/* 268 */	NULL, /* filler */
	/* 269 */	NULL, /* filler */
	/* 270 */	NULL, /* filler */
	/* 271 */	NULL, /* filler */
	/* 272 */	NULL, /* filler */
	/* 273 */	NULL, /* filler */
	/* 274 */	NULL, /* filler */
	/* 275 */	NULL, /* filler */
	/* 276 */	NULL, /* filler */
	/* 277 */	NULL, /* filler */
	/* 278 */	NULL, /* filler */
	/* 279 */	NULL, /* filler */
	/* 280 */	NULL, /* filler */
	/* 281 */	NULL, /* filler */
	/* 282 */	NULL, /* filler */
	/* 283 */	NULL, /* filler */
	/* 284 */	NULL, /* filler */
	/* 285 */	NULL, /* filler */
	/* 286 */	NULL, /* filler */
	/* 287 */	NULL, /* filler */
	/* 288 */	NULL, /* filler */
	/* 289 */	NULL, /* filler */
	/* 290 */	NULL, /* filler */
	/* 291 */	NULL, /* filler */
	/* 292 */	NULL, /* filler */
	/* 293 */	NULL, /* filler */
	/* 294 */	NULL, /* filler */
	/* 295 */	NULL, /* filler */
	/* 296 */	NULL, /* filler */
	/* 297 */	NULL, /* filler */
	/* 298 */	NULL, /* filler */
	/* 299 */	NULL, /* filler */
	/* 300 */	NULL, /* filler */
	/* 301 */	NULL, /* filler */
	/* 302 */	NULL, /* filler */
	/* 303 */	NULL, /* filler */
	/* 304 */	NULL, /* filler */
	/* 305 */	NULL, /* filler */
	/* 306 */	NULL, /* filler */
	/* 307 */	NULL, /* filler */
	/* 308 */	NULL, /* filler */
	/* 309 */	NULL, /* filler */
	/* 310 */	NULL, /* filler */
	/* 311 */	NULL, /* filler */
	/* 312 */	NULL, /* filler */
	/* 313 */	NULL, /* filler */
	/* 314 */	NULL, /* filler */
	/* 315 */	NULL, /* filler */
	/* 316 */	NULL, /* filler */
	/* 317 */	NULL, /* filler */
	/* 318 */	NULL, /* filler */
	/* 319 */	NULL, /* filler */
	/* 320 */	NULL, /* filler */
	/* 321 */	NULL, /* filler */
	/* 322 */	NULL, /* filler */
	/* 323 */	NULL, /* filler */
	/* 324 */	NULL, /* filler */
	/* 325 */	NULL, /* filler */
	/* 326 */	NULL, /* filler */
	/* 327 */	NULL, /* filler */
	/* 328 */	NULL, /* filler */
	/* 329 */	NULL, /* filler */
	/* 330 */	NULL, /* filler */
	/* 331 */	NULL, /* filler */
	/* 332 */	NULL, /* filler */
	/* 333 */	NULL, /* filler */
	/* 334 */	NULL, /* filler */
	/* 335 */	NULL, /* filler */
	/* 336 */	NULL, /* filler */
	/* 337 */	NULL, /* filler */
	/* 338 */	NULL, /* filler */
	/* 339 */	NULL, /* filler */
	/* 340 */	NULL, /* filler */
	/* 341 */	NULL, /* filler */
	/* 342 */	NULL, /* filler */
	/* 343 */	NULL, /* filler */
	/* 344 */	NULL, /* filler */
	/* 345 */	NULL, /* filler */
	/* 346 */	NULL, /* filler */
	/* 347 */	NULL, /* filler */
	/* 348 */	NULL, /* filler */
	/* 349 */	NULL, /* filler */
	/* 350 */	NULL, /* filler */
	/* 351 */	NULL, /* filler */
	/* 352 */	NULL, /* filler */
	/* 353 */	NULL, /* filler */
	/* 354 */	NULL, /* filler */
	/* 355 */	NULL, /* filler */
	/* 356 */	NULL, /* filler */
	/* 357 */	NULL, /* filler */
	/* 358 */	NULL, /* filler */
	/* 359 */	NULL, /* filler */
	/* 360 */	NULL, /* filler */
	/* 361 */	NULL, /* filler */
	/* 362 */	NULL, /* filler */
	/* 363 */	NULL, /* filler */
	/* 364 */	NULL, /* filler */
	/* 365 */	NULL, /* filler */
	/* 366 */	NULL, /* filler */
	/* 367 */	NULL, /* filler */
	/* 368 */	NULL, /* filler */
	/* 369 */	NULL, /* filler */
	/* 370 */	NULL, /* filler */
	/* 371 */	NULL, /* filler */
	/* 372 */	NULL, /* filler */
	/* 373 */	NULL, /* filler */
	/* 374 */	NULL, /* filler */
	/* 375 */	NULL, /* filler */
	/* 376 */	NULL, /* filler */
	/* 377 */	NULL, /* filler */
	/* 378 */	NULL, /* filler */
	/* 379 */	NULL, /* filler */
	/* 380 */	NULL, /* filler */
	/* 381 */	NULL, /* filler */
	/* 382 */	NULL, /* filler */
	/* 383 */	NULL, /* filler */
	/* 384 */	NULL, /* filler */
	/* 385 */	NULL, /* filler */
	/* 386 */	NULL, /* filler */
	/* 387 */	NULL, /* filler */
	/* 388 */	NULL, /* filler */
	/* 389 */	NULL, /* filler */
	/* 390 */	NULL, /* filler */
	/* 391 */	NULL, /* filler */
	/* 392 */	NULL, /* filler */
	/* 393 */	NULL, /* filler */
	/* 394 */	NULL, /* filler */
	/* 395 */	NULL, /* filler */
	/* 396 */	NULL, /* filler */
	/* 397 */	NULL, /* filler */
	/* 398 */	NULL, /* filler */
	/* 399 */	NULL, /* filler */
	/* 400 */	NULL, /* filler */
	/* 401 */	NULL, /* filler */
	/* 402 */	NULL, /* filler */
	/* 403 */	NULL, /* filler */
	/* 404 */	NULL, /* filler */
	/* 405 */	NULL, /* filler */
	/* 406 */	NULL, /* filler */
	/* 407 */	NULL, /* filler */
	/* 408 */	NULL, /* filler */
	/* 409 */	NULL, /* filler */
	/* 410 */	NULL, /* filler */
	/* 411 */	NULL, /* filler */
	/* 412 */	NULL, /* filler */
	/* 413 */	NULL, /* filler */
	/* 414 */	NULL, /* filler */
	/* 415 */	NULL, /* filler */
	/* 416 */	NULL, /* filler */
	/* 417 */	NULL, /* filler */
	/* 418 */	NULL, /* filler */
	/* 419 */	NULL, /* filler */
	/* 420 */	NULL, /* filler */
	/* 421 */	NULL, /* filler */
	/* 422 */	NULL, /* filler */
	/* 423 */	NULL, /* filler */
	/* 424 */	NULL, /* filler */
	/* 425 */	NULL, /* filler */
	/* 426 */	NULL, /* filler */
	/* 427 */	NULL, /* filler */
	/* 428 */	NULL, /* filler */
	/* 429 */	NULL, /* filler */
	/* 430 */	NULL, /* filler */
	/* 431 */	NULL, /* filler */
	/* 432 */	NULL, /* filler */
	/* 433 */	NULL, /* filler */
	/* 434 */	NULL, /* filler */
	/* 435 */	NULL, /* filler */
	/* 436 */	NULL, /* filler */
	/* 437 */	NULL, /* filler */
	/* 438 */	NULL, /* filler */
	/* 439 */	NULL, /* filler */
	/* 440 */	NULL, /* filler */
	/* 441 */	NULL, /* filler */
	/* 442 */	NULL, /* filler */
	/* 443 */	NULL, /* filler */
	/* 444 */	NULL, /* filler */
	/* 445 */	NULL, /* filler */
	/* 446 */	NULL, /* filler */
	/* 447 */	NULL, /* filler */
	/* 448 */	NULL, /* filler */
	/* 449 */	NULL, /* filler */
	/* 450 */	NULL, /* filler */
	/* 451 */	NULL, /* filler */
	/* 452 */	NULL, /* filler */
	/* 453 */	NULL, /* filler */
	/* 454 */	NULL, /* filler */
	/* 455 */	NULL, /* filler */
	/* 456 */	NULL, /* filler */
	/* 457 */	NULL, /* filler */
	/* 458 */	NULL, /* filler */
	/* 459 */	NULL, /* filler */
	/* 460 */	NULL, /* filler */
	/* 461 */	NULL, /* filler */
	/* 462 */	NULL, /* filler */
	/* 463 */	NULL, /* filler */
	/* 464 */	NULL, /* filler */
	/* 465 */	NULL, /* filler */
	/* 466 */	NULL, /* filler */
	/* 467 */	NULL, /* filler */
	/* 468 */	NULL, /* filler */
	/* 469 */	NULL, /* filler */
	/* 470 */	NULL, /* filler */
	/* 471 */	NULL, /* filler */
	/* 472 */	NULL, /* filler */
	/* 473 */	NULL, /* filler */
	/* 474 */	NULL, /* filler */
	/* 475 */	NULL, /* filler */
	/* 476 */	NULL, /* filler */
	/* 477 */	NULL, /* filler */
	/* 478 */	NULL, /* filler */
	/* 479 */	NULL, /* filler */
	/* 480 */	NULL, /* filler */
	/* 481 */	NULL, /* filler */
	/* 482 */	NULL, /* filler */
	/* 483 */	NULL, /* filler */
	/* 484 */	NULL, /* filler */
	/* 485 */	NULL, /* filler */
	/* 486 */	NULL, /* filler */
	/* 487 */	NULL, /* filler */
	/* 488 */	NULL, /* filler */
	/* 489 */	NULL, /* filler */
	/* 490 */	NULL, /* filler */
	/* 491 */	NULL, /* filler */
	/* 492 */	NULL, /* filler */
	/* 493 */	NULL, /* filler */
	/* 494 */	NULL, /* filler */
	/* 495 */	NULL, /* filler */
	/* 496 */	NULL, /* filler */
	/* 497 */	NULL, /* filler */
	/* 498 */	NULL, /* filler */
	/* 499 */	NULL, /* filler */
	/* 500 */	NULL, /* filler */
	/* 501 */	NULL, /* filler */
	/* 502 */	NULL, /* filler */
	/* 503 */	NULL, /* filler */
	/* 504 */	NULL, /* filler */
	/* 505 */	NULL, /* filler */
	/* 506 */	NULL, /* filler */
	/* 507 */	NULL, /* filler */
	/* 508 */	NULL, /* filler */
	/* 509 */	NULL, /* filler */
	/* 510 */	NULL, /* filler */
	/* 511 */	NULL, /* filler */
};
