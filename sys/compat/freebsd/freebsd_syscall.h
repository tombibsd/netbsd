/* $NetBSD$ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.67 2013/11/07 19:37:18 njoly Exp
 */

#ifndef _FREEBSD_SYS_SYSCALL_H_
#define	_FREEBSD_SYS_SYSCALL_H_

#define	FREEBSD_SYS_MAXSYSARGS	8

/* syscall: "syscall" ret: "int" args: */
#define	FREEBSD_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	FREEBSD_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	FREEBSD_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	FREEBSD_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	FREEBSD_SYS_write	4

/* syscall: "open" ret: "int" args: "const char *" "int" "int" */
#define	FREEBSD_SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	FREEBSD_SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage50 *" */
#define	FREEBSD_SYS_wait4	7

/* syscall: "ocreat" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_ocreat	8

/* syscall: "link" ret: "int" args: "const char *" "const char *" */
#define	FREEBSD_SYS_link	9

/* syscall: "unlink" ret: "int" args: "const char *" */
#define	FREEBSD_SYS_unlink	10

				/* 11 is obsolete execv */
/* syscall: "chdir" ret: "int" args: "const char *" */
#define	FREEBSD_SYS_chdir	12

/* syscall: "fchdir" ret: "int" args: "int" */
#define	FREEBSD_SYS_fchdir	13

/* syscall: "mknod" ret: "int" args: "const char *" "int" "int" */
#define	FREEBSD_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_chmod	15

/* syscall: "chown" ret: "int" args: "const char *" "int" "int" */
#define	FREEBSD_SYS_chown	16

/* syscall: "break" ret: "int" args: "char *" */
#define	FREEBSD_SYS_break	17

/* syscall: "getfsstat" ret: "int" args: "struct statfs12 *" "long" "int" */
#define	FREEBSD_SYS_getfsstat	18

/* syscall: "olseek" ret: "long" args: "int" "long" "int" */
#define	FREEBSD_SYS_olseek	19

/* syscall: "getpid_with_ppid" ret: "pid_t" args: */
#define	FREEBSD_SYS_getpid_with_ppid	20

/* syscall: "mount" ret: "int" args: "int" "const char *" "int" "void *" */
#define	FREEBSD_SYS_mount	21

/* syscall: "unmount" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_unmount	22

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	FREEBSD_SYS_setuid	23

/* syscall: "getuid_with_euid" ret: "uid_t" args: */
#define	FREEBSD_SYS_getuid_with_euid	24

/* syscall: "geteuid" ret: "uid_t" args: */
#define	FREEBSD_SYS_geteuid	25

/* syscall: "ptrace" ret: "int" args: "int" "pid_t" "void *" "int" */
#define	FREEBSD_SYS_ptrace	26

/* syscall: "recvmsg" ret: "int" args: "int" "struct msghdr *" "int" */
#define	FREEBSD_SYS_recvmsg	27

/* syscall: "sendmsg" ret: "int" args: "int" "void *" "int" */
#define	FREEBSD_SYS_sendmsg	28

/* syscall: "recvfrom" ret: "int" args: "int" "void *" "size_t" "int" "void *" "int *" */
#define	FREEBSD_SYS_recvfrom	29

/* syscall: "accept" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_accept	30

/* syscall: "getpeername" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_getpeername	31

/* syscall: "getsockname" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_getsockname	32

/* syscall: "access" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_access	33

/* syscall: "chflags" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_chflags	34

/* syscall: "fchflags" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_fchflags	35

/* syscall: "sync" ret: "int" args: */
#define	FREEBSD_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_kill	37

/* syscall: "stat43" ret: "int" args: "const char *" "struct stat43 *" */
#define	FREEBSD_SYS_stat43	38

/* syscall: "getppid" ret: "pid_t" args: */
#define	FREEBSD_SYS_getppid	39

/* syscall: "lstat43" ret: "int" args: "const char *" "struct stat43 *" */
#define	FREEBSD_SYS_lstat43	40

/* syscall: "dup" ret: "int" args: "int" */
#define	FREEBSD_SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	FREEBSD_SYS_pipe	42

/* syscall: "getegid" ret: "gid_t" args: */
#define	FREEBSD_SYS_getegid	43

/* syscall: "profil" ret: "int" args: "void *" "u_int" "u_int" "u_int" */
#define	FREEBSD_SYS_profil	44

#ifdef KTRACE
/* syscall: "ktrace" ret: "int" args: "char *" "int" "int" "int" */
#define	FREEBSD_SYS_ktrace	45

#else
				/* 45 is excluded ktrace */
#endif
/* syscall: "sigaction" ret: "int" args: "int" "const struct sigaction13 *" "struct sigaction13 *" */
#define	FREEBSD_SYS_sigaction	46

/* syscall: "getgid_with_egid" ret: "gid_t" args: */
#define	FREEBSD_SYS_getgid_with_egid	47

/* syscall: "sigprocmask" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_sigprocmask	48

/* syscall: "__getlogin" ret: "int" args: "char *" "u_int" */
#define	FREEBSD_SYS___getlogin	49

/* syscall: "__setlogin" ret: "int" args: "char *" */
#define	FREEBSD_SYS___setlogin	50

/* syscall: "acct" ret: "int" args: "char *" */
#define	FREEBSD_SYS_acct	51

/* syscall: "sigpending" ret: "int" args: */
#define	FREEBSD_SYS_sigpending	52

/* syscall: "sigaltstack" ret: "int" args: "const struct sigaltstack13 *" "struct sigaltstack13 *" */
#define	FREEBSD_SYS_sigaltstack	53

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "void *" */
#define	FREEBSD_SYS_ioctl	54

/* syscall: "oreboot" ret: "int" args: "int" */
#define	FREEBSD_SYS_oreboot	55

/* syscall: "revoke" ret: "int" args: "const char *" */
#define	FREEBSD_SYS_revoke	56

/* syscall: "symlink" ret: "int" args: "const char *" "const char *" */
#define	FREEBSD_SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "const char *" "char *" "int" */
#define	FREEBSD_SYS_readlink	58

/* syscall: "execve" ret: "int" args: "const char *" "char **" "char **" */
#define	FREEBSD_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	FREEBSD_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "const char *" */
#define	FREEBSD_SYS_chroot	61

/* syscall: "fstat43" ret: "int" args: "int" "struct stat43 *" */
#define	FREEBSD_SYS_fstat43	62

/* syscall: "ogetkerninfo" ret: "int" args: "int" "char *" "int *" "int" */
#define	FREEBSD_SYS_ogetkerninfo	63

/* syscall: "ogetpagesize" ret: "int" args: */
#define	FREEBSD_SYS_ogetpagesize	64

/* syscall: "msync" ret: "int" args: "void *" "size_t" "int" */
#define	FREEBSD_SYS_msync	65

/* syscall: "vfork" ret: "int" args: */
#define	FREEBSD_SYS_vfork	66

				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
/* syscall: "sbrk" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS_sbrk	69

/* syscall: "sstk" ret: "int" args: "int" */
#define	FREEBSD_SYS_sstk	70

/* syscall: "ommap" ret: "int" args: "void *" "size_t" "int" "int" "int" "long" */
#define	FREEBSD_SYS_ommap	71

/* syscall: "vadvise" ret: "int" args: "int" */
#define	FREEBSD_SYS_vadvise	72

/* syscall: "munmap" ret: "int" args: "void *" "size_t" */
#define	FREEBSD_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "void *" "size_t" "int" */
#define	FREEBSD_SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "void *" "size_t" "int" */
#define	FREEBSD_SYS_madvise	75

				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
/* syscall: "mincore" ret: "int" args: "void *" "size_t" "char *" */
#define	FREEBSD_SYS_mincore	78

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	FREEBSD_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	FREEBSD_SYS_setgroups	80

/* syscall: "getpgrp" ret: "int" args: */
#define	FREEBSD_SYS_getpgrp	81

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_setpgid	82

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval50 *" "struct itimerval50 *" */
#define	FREEBSD_SYS_setitimer	83

/* syscall: "owait" ret: "int" args: */
#define	FREEBSD_SYS_owait	84

/* syscall: "swapon" ret: "int" args: "char *" */
#define	FREEBSD_SYS_swapon	85

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval50 *" */
#define	FREEBSD_SYS_getitimer	86

/* syscall: "ogethostname" ret: "int" args: "char *" "u_int" */
#define	FREEBSD_SYS_ogethostname	87

/* syscall: "osethostname" ret: "int" args: "char *" "u_int" */
#define	FREEBSD_SYS_osethostname	88

/* syscall: "ogetdtablesize" ret: "int" args: */
#define	FREEBSD_SYS_ogetdtablesize	89

/* syscall: "dup2" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_dup2	90

/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	FREEBSD_SYS_fcntl	92

/* syscall: "select" ret: "int" args: "u_int" "fd_set *" "fd_set *" "fd_set *" "struct timeval50 *" */
#define	FREEBSD_SYS_select	93

/* syscall: "fsync" ret: "int" args: "int" */
#define	FREEBSD_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	FREEBSD_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	FREEBSD_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "void *" "int" */
#define	FREEBSD_SYS_connect	98

/* syscall: "oaccept" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_oaccept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_getpriority	100

/* syscall: "osend" ret: "int" args: "int" "void *" "int" "int" */
#define	FREEBSD_SYS_osend	101

/* syscall: "orecv" ret: "int" args: "int" "void *" "int" "int" */
#define	FREEBSD_SYS_orecv	102

/* syscall: "sigreturn" ret: "int" args: "struct freebsd_sigcontext *" */
#define	FREEBSD_SYS_sigreturn	103

/* syscall: "bind" ret: "int" args: "int" "void *" "int" */
#define	FREEBSD_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "void *" "int" */
#define	FREEBSD_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_listen	106

				/* 107 is obsolete vtimes */
/* syscall: "osigvec" ret: "int" args: "int" "struct sigvec *" "struct sigvec *" */
#define	FREEBSD_SYS_osigvec	108

/* syscall: "osigblock" ret: "int" args: "int" */
#define	FREEBSD_SYS_osigblock	109

/* syscall: "osigsetmask" ret: "int" args: "int" */
#define	FREEBSD_SYS_osigsetmask	110

/* syscall: "sigsuspend" ret: "int" args: "int" */
#define	FREEBSD_SYS_sigsuspend	111

/* syscall: "osigstack" ret: "int" args: "struct sigstack *" "struct sigstack *" */
#define	FREEBSD_SYS_osigstack	112

/* syscall: "orecvmsg" ret: "int" args: "int" "struct omsghdr *" "int" */
#define	FREEBSD_SYS_orecvmsg	113

/* syscall: "osendmsg" ret: "int" args: "int" "void *" "int" */
#define	FREEBSD_SYS_osendmsg	114

#ifdef TRACE
/* syscall: "vtrace" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_vtrace	115

#else
				/* 115 is obsolete vtrace */
#endif
/* syscall: "gettimeofday" ret: "int" args: "struct timeval50 *" "struct timezone *" */
#define	FREEBSD_SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage50 *" */
#define	FREEBSD_SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "int *" */
#define	FREEBSD_SYS_getsockopt	118

				/* 119 is obsolete resuba */
/* syscall: "readv" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	FREEBSD_SYS_readv	120

/* syscall: "writev" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	FREEBSD_SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "struct timeval50 *" "struct timezone50 *" */
#define	FREEBSD_SYS_settimeofday	122

/* syscall: "fchown" ret: "int" args: "int" "int" "int" */
#define	FREEBSD_SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_fchmod	124

/* syscall: "orecvfrom" ret: "int" args: "int" "void *" "size_t" "int" "void *" "int *" */
#define	FREEBSD_SYS_orecvfrom	125

/* syscall: "setreuid" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_setregid	127

/* syscall: "rename" ret: "int" args: "const char *" "const char *" */
#define	FREEBSD_SYS_rename	128

/* syscall: "otruncate" ret: "int" args: "const char *" "long" */
#define	FREEBSD_SYS_otruncate	129

/* syscall: "oftruncate" ret: "int" args: "int" "long" */
#define	FREEBSD_SYS_oftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_flock	131

/* syscall: "mkfifo" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_mkfifo	132

/* syscall: "sendto" ret: "int" args: "int" "void *" "size_t" "int" "void *" "int" */
#define	FREEBSD_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	FREEBSD_SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "const char *" */
#define	FREEBSD_SYS_rmdir	137

/* syscall: "utimes" ret: "int" args: "char *" "struct timeval50 *" */
#define	FREEBSD_SYS_utimes	138

				/* 139 is obsolete 4.2 sigreturn */
/* syscall: "adjtime" ret: "int" args: "struct timeval50 *" "struct timeval50 *" */
#define	FREEBSD_SYS_adjtime	140

/* syscall: "ogetpeername" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_ogetpeername	141

/* syscall: "ogethostid" ret: "int32_t" args: */
#define	FREEBSD_SYS_ogethostid	142

/* syscall: "osethostid" ret: "int" args: "int32_t" */
#define	FREEBSD_SYS_osethostid	143

/* syscall: "ogetrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	FREEBSD_SYS_ogetrlimit	144

/* syscall: "osetrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	FREEBSD_SYS_osetrlimit	145

/* syscall: "okillpg" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_okillpg	146

/* syscall: "setsid" ret: "int" args: */
#define	FREEBSD_SYS_setsid	147

/* syscall: "quotactl" ret: "int" args: "char *" "int" "int" "void *" */
#define	FREEBSD_SYS_quotactl	148

/* syscall: "oquota" ret: "int" args: */
#define	FREEBSD_SYS_oquota	149

/* syscall: "ogetsockname" ret: "int" args: "int" "void *" "int *" */
#define	FREEBSD_SYS_ogetsockname	150

/* syscall: "ogetdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	FREEBSD_SYS_ogetdirentries	156

/* syscall: "statfs" ret: "int" args: "const char *" "struct statfs12 *" */
#define	FREEBSD_SYS_statfs	157

/* syscall: "fstatfs" ret: "int" args: "int" "struct statfs12 *" */
#define	FREEBSD_SYS_fstatfs	158

/* syscall: "getfh" ret: "int" args: "const char *" "struct compat_30_fhandle *" */
#define	FREEBSD_SYS_getfh	161

/* syscall: "getdomainname" ret: "int" args: "char *" "int" */
#define	FREEBSD_SYS_getdomainname	162

/* syscall: "setdomainname" ret: "int" args: "char *" "int" */
#define	FREEBSD_SYS_setdomainname	163

/* syscall: "uname" ret: "int" args: "struct outsname *" */
#define	FREEBSD_SYS_uname	164

/* syscall: "sysarch" ret: "int" args: "int" "char *" */
#define	FREEBSD_SYS_sysarch	165

/* syscall: "rtprio" ret: "int" args: "int" "pid_t" "struct freebsd_rtprio *" */
#define	FREEBSD_SYS_rtprio	166

#if defined(SYSVSEM) && !defined(_LP64)
/* syscall: "semsys" ret: "int" args: "int" "int" "int" "int" "int" */
#define	FREEBSD_SYS_semsys	169

#else
#endif
#if defined(SYSVMSG) && !defined(_LP64)
/* syscall: "msgsys" ret: "int" args: "int" "int" "int" "int" "int" "int" */
#define	FREEBSD_SYS_msgsys	170

#else
#endif
#if defined(SYSVSHM) && !defined(_LP64)
/* syscall: "shmsys" ret: "int" args: "int" "int" "int" "int" */
#define	FREEBSD_SYS_shmsys	171

#else
#endif
/* syscall: "pread" ret: "ssize_t" args: "int" "void *" "size_t" "int" "off_t" */
#define	FREEBSD_SYS_pread	173

/* syscall: "pwrite" ret: "ssize_t" args: "int" "const void *" "size_t" "int" "off_t" */
#define	FREEBSD_SYS_pwrite	174

#ifdef NTP
/* syscall: "freebsd_ntp_adjtime" ret: "int" args: "struct freebsd_timex *" */
#define	FREEBSD_SYS_freebsd_ntp_adjtime	176

#else
				/* 176 is excluded ntp_adjtime */
#endif
/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	FREEBSD_SYS_setgid	181

/* syscall: "setegid" ret: "int" args: "gid_t" */
#define	FREEBSD_SYS_setegid	182

/* syscall: "seteuid" ret: "int" args: "uid_t" */
#define	FREEBSD_SYS_seteuid	183

/* syscall: "stat" ret: "int" args: "const char *" "struct stat12 *" */
#define	FREEBSD_SYS_stat	188

/* syscall: "fstat" ret: "int" args: "int" "struct stat12 *" */
#define	FREEBSD_SYS_fstat	189

/* syscall: "lstat" ret: "int" args: "const char *" "struct stat12 *" */
#define	FREEBSD_SYS_lstat	190

/* syscall: "pathconf" ret: "int" args: "const char *" "int" */
#define	FREEBSD_SYS_pathconf	191

/* syscall: "fpathconf" ret: "int" args: "int" "int" */
#define	FREEBSD_SYS_fpathconf	192

/* syscall: "getrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	FREEBSD_SYS_getrlimit	194

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	FREEBSD_SYS_setrlimit	195

/* syscall: "getdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	FREEBSD_SYS_getdirentries	196

/* syscall: "mmap" ret: "void *" args: "void *" "size_t" "int" "int" "int" "long" "off_t" */
#define	FREEBSD_SYS_mmap	197

/* syscall: "__syscall" ret: "int" args: */
#define	FREEBSD_SYS___syscall	198

/* syscall: "lseek" ret: "off_t" args: "int" "int" "off_t" "int" */
#define	FREEBSD_SYS_lseek	199

/* syscall: "truncate" ret: "int" args: "const char *" "int" "off_t" */
#define	FREEBSD_SYS_truncate	200

/* syscall: "ftruncate" ret: "int" args: "int" "int" "off_t" */
#define	FREEBSD_SYS_ftruncate	201

/* syscall: "sysctl" ret: "int" args: "int *" "u_int" "void *" "size_t *" "void *" "size_t" */
#define	FREEBSD_SYS_sysctl	202

/* syscall: "mlock" ret: "int" args: "void *" "size_t" */
#define	FREEBSD_SYS_mlock	203

/* syscall: "munlock" ret: "int" args: "void *" "size_t" */
#define	FREEBSD_SYS_munlock	204

#ifdef FREEBSD_BASED_ON_44LITE_R2
/* syscall: "undelete" ret: "int" args: "char *" */
#define	FREEBSD_SYS_undelete	205

#else
#endif
/* syscall: "futimes" ret: "int" args: "int" "const struct timeval50 *" */
#define	FREEBSD_SYS_futimes	206

/* syscall: "getpgid" ret: "pid_t" args: "pid_t" */
#define	FREEBSD_SYS_getpgid	207

#if 0
/* syscall: "reboot" ret: "int" args: "int" "char *" */
#define	FREEBSD_SYS_reboot	208

#else
#endif
/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	FREEBSD_SYS_poll	209

#ifdef SYSVSEM
/* syscall: "__semctl" ret: "int" args: "int" "int" "int" "union __semun *" */
#define	FREEBSD_SYS___semctl	220

/* syscall: "semget" ret: "int" args: "key_t" "int" "int" */
#define	FREEBSD_SYS_semget	221

/* syscall: "semop" ret: "int" args: "int" "struct sembuf *" "u_int" */
#define	FREEBSD_SYS_semop	222

/* syscall: "semconfig" ret: "int" args: "int" */
#define	FREEBSD_SYS_semconfig	223

#else
#endif
#ifdef SYSVMSG
/* syscall: "msgctl" ret: "int" args: "int" "int" "struct msqid14_ds *" */
#define	FREEBSD_SYS_msgctl	224

/* syscall: "msgget" ret: "int" args: "key_t" "int" */
#define	FREEBSD_SYS_msgget	225

/* syscall: "msgsnd" ret: "int" args: "int" "void *" "size_t" "int" */
#define	FREEBSD_SYS_msgsnd	226

/* syscall: "msgrcv" ret: "int" args: "int" "void *" "size_t" "long" "int" */
#define	FREEBSD_SYS_msgrcv	227

#else
#endif
#ifdef SYSVSHM
/* syscall: "shmat" ret: "int" args: "int" "void *" "int" */
#define	FREEBSD_SYS_shmat	228

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct shmid_ds14 *" */
#define	FREEBSD_SYS_shmctl	229

/* syscall: "shmdt" ret: "int" args: "void *" */
#define	FREEBSD_SYS_shmdt	230

/* syscall: "shmget" ret: "int" args: "key_t" "int" "int" */
#define	FREEBSD_SYS_shmget	231

#else
#endif
/* syscall: "clock_gettime" ret: "int" args: "clockid_t" "struct timespec50 *" */
#define	FREEBSD_SYS_clock_gettime	232

/* syscall: "clock_settime" ret: "int" args: "clockid_t" "const struct timespec50 *" */
#define	FREEBSD_SYS_clock_settime	233

/* syscall: "clock_getres" ret: "int" args: "clockid_t" "struct timespec50 *" */
#define	FREEBSD_SYS_clock_getres	234

/* syscall: "nanosleep" ret: "int" args: "const struct timespec50 *" "struct timespec50 *" */
#define	FREEBSD_SYS_nanosleep	240

/* syscall: "minherit" ret: "int" args: "void *" "size_t" "int" */
#define	FREEBSD_SYS_minherit	250

/* syscall: "rfork" ret: "int" args: "int" */
#define	FREEBSD_SYS_rfork	251

/* syscall: "issetugid" ret: "int" args: */
#define	FREEBSD_SYS_issetugid	253

/* syscall: "lchown" ret: "int" args: "const char *" "int" "int" */
#define	FREEBSD_SYS_lchown	254

/* syscall: "getdents" ret: "int" args: "int" "char *" "size_t" */
#define	FREEBSD_SYS_getdents	272

/* syscall: "lchmod" ret: "int" args: "const char *" "mode_t" */
#define	FREEBSD_SYS_lchmod	274

/* syscall: "netbsd_lchown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	FREEBSD_SYS_netbsd_lchown	275

/* syscall: "lutimes" ret: "int" args: "const char *" "const struct timeval50 *" */
#define	FREEBSD_SYS_lutimes	276

/* syscall: "__msync13" ret: "int" args: "void *" "size_t" "int" */
#define	FREEBSD_SYS___msync13	277

/* syscall: "__stat13" ret: "int" args: "const char *" "struct stat13 *" */
#define	FREEBSD_SYS___stat13	278

/* syscall: "__fstat13" ret: "int" args: "int" "struct stat13 *" */
#define	FREEBSD_SYS___fstat13	279

/* syscall: "__lstat13" ret: "int" args: "const char *" "struct stat13 *" */
#define	FREEBSD_SYS___lstat13	280

/* syscall: "fhstatfs" ret: "int" args: "const struct compat_30_fhandle *" "struct statfs12 *" */
#define	FREEBSD_SYS_fhstatfs	297

/* syscall: "fhopen" ret: "int" args: "const struct compat_30_fhandle *" "int" */
#define	FREEBSD_SYS_fhopen	298

/* syscall: "fhstat" ret: "int" args: "const struct cocmpat_30_fhandlez *" "struct stat13 *" */
#define	FREEBSD_SYS_fhstat	299

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	FREEBSD_SYS_getsid	310

/* syscall: "yield" ret: "void" args: */
#define	FREEBSD_SYS_yield	321

/* syscall: "mlockall" ret: "int" args: "int" */
#define	FREEBSD_SYS_mlockall	324

/* syscall: "munlockall" ret: "int" args: */
#define	FREEBSD_SYS_munlockall	325

/* syscall: "__getcwd" ret: "int" args: "char *" "size_t" */
#define	FREEBSD_SYS___getcwd	326

/* syscall: "sched_setparam" ret: "int" args: "pid_t" "const struct freebsd_sched_param *" */
#define	FREEBSD_SYS_sched_setparam	327

/* syscall: "sched_getparam" ret: "int" args: "pid_t" "struct freebsd_sched_param *" */
#define	FREEBSD_SYS_sched_getparam	328

/* syscall: "sched_setscheduler" ret: "int" args: "pid_t" "int" "const struct sched_param *" */
#define	FREEBSD_SYS_sched_setscheduler	329

/* syscall: "sched_getscheduler" ret: "int" args: "pid_t" */
#define	FREEBSD_SYS_sched_getscheduler	330

/* syscall: "sched_yield" ret: "int" args: */
#define	FREEBSD_SYS_sched_yield	331

/* syscall: "sched_get_priority_max" ret: "int" args: "int" */
#define	FREEBSD_SYS_sched_get_priority_max	332

/* syscall: "sched_get_priority_min" ret: "int" args: "int" */
#define	FREEBSD_SYS_sched_get_priority_min	333

/* syscall: "utrace" ret: "int" args: "void *" "size_t" */
#define	FREEBSD_SYS_utrace	335

/* syscall: "__sigprocmask14" ret: "int" args: "int" "const sigset_t *" "sigset_t *" */
#define	FREEBSD_SYS___sigprocmask14	340

/* syscall: "__sigsuspend14" ret: "int" args: "const sigset_t *" */
#define	FREEBSD_SYS___sigsuspend14	341

/* syscall: "sigaction4" ret: "int" args: "int" "const struct freebsd_sigaction4 *" "struct freebsd_sigaction4 *" */
#define	FREEBSD_SYS_sigaction4	342

/* syscall: "__sigpending14" ret: "int" args: "const sigset_t *" */
#define	FREEBSD_SYS___sigpending14	343

/* syscall: "lchflags" ret: "int" args: "const char *" "u_long" */
#define	FREEBSD_SYS_lchflags	391

/* syscall: "uuidgen" ret: "int" args: "struct uuid *" "int" */
#define	FREEBSD_SYS_uuidgen	392

/* syscall: "_ksem_close" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS__ksem_close	400

/* syscall: "_ksem_post" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS__ksem_post	401

/* syscall: "_ksem_wait" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS__ksem_wait	402

/* syscall: "_ksem_trywait" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS__ksem_trywait	403

/* syscall: "_ksem_unlink" ret: "int" args: "const char *" */
#define	FREEBSD_SYS__ksem_unlink	406

/* syscall: "_ksem_getvalue" ret: "int" args: "intptr_t" "unsigned int *" */
#define	FREEBSD_SYS__ksem_getvalue	407

/* syscall: "_ksem_destroy" ret: "int" args: "intptr_t" */
#define	FREEBSD_SYS__ksem_destroy	408

#define	FREEBSD_SYS_MAXSYSCALL	440
#define	FREEBSD_SYS_NSYSENT	512
#endif /* _FREEBSD_SYS_SYSCALL_H_ */
