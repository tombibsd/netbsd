/* $NetBSD$ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.41 2013/11/07 19:37:18 njoly Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <compat/sys/time.h>
#include <compat/ibcs2/ibcs2_types.h>
#include <compat/ibcs2/ibcs2_signal.h>
#include <compat/ibcs2/ibcs2_syscallargs.h>
#include <compat/ibcs2/ibcs2_statfs.h>

#define	s(type)	sizeof(type)
#define	n(type)	(sizeof(type)/sizeof (register_t))
#define	ns(type)	.sy_narg = n(type), .sy_argsize = s(type)

struct sysent ibcs2_sysent[] = {
	{
		.sy_call = (sy_call_t *)sys_nosys
	},		/* 0 = syscall */
	{
		ns(struct sys_exit_args),
		.sy_call = (sy_call_t *)sys_exit
	},		/* 1 = exit */
	{
		.sy_call = (sy_call_t *)sys_fork
	},		/* 2 = fork */
	{
		ns(struct ibcs2_sys_read_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_read
	},		/* 3 = read */
	{
		ns(struct sys_write_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_write
	},		/* 4 = write */
	{
		ns(struct ibcs2_sys_open_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_open
	},		/* 5 = open */
	{
		ns(struct sys_close_args),
		.sy_call = (sy_call_t *)sys_close
	},		/* 6 = close */
	{
		ns(struct ibcs2_sys_waitsys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_waitsys
	},		/* 7 = waitsys */
	{
		ns(struct ibcs2_sys_creat_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_creat
	},		/* 8 = creat */
	{
		ns(struct sys_link_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_link
	},		/* 9 = link */
	{
		ns(struct sys_unlink_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_unlink
	},		/* 10 = unlink */
	{
		ns(struct ibcs2_sys_execv_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_execv
	},		/* 11 = execv */
	{
		ns(struct sys_chdir_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_chdir
	},		/* 12 = chdir */
	{
		ns(struct ibcs2_sys_time_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_time
	},		/* 13 = time */
	{
		ns(struct ibcs2_sys_mknod_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_mknod
	},		/* 14 = mknod */
	{
		ns(struct sys_chmod_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_chmod
	},		/* 15 = chmod */
	{
		ns(struct sys___posix_chown_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys___posix_chown
	},		/* 16 = __posix_chown */
	{
		ns(struct sys_obreak_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_obreak
	},		/* 17 = obreak */
	{
		ns(struct ibcs2_sys_stat_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_stat
	},		/* 18 = stat */
	{
		ns(struct compat_43_sys_lseek_args),
		.sy_call = (sy_call_t *)compat_43_sys_lseek
	},		/* 19 = lseek */
	{
		.sy_call = (sy_call_t *)sys_getpid_with_ppid
	},		/* 20 = getpid_with_ppid */
	{
		ns(struct ibcs2_sys_mount_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_mount
	},		/* 21 = mount */
	{
		ns(struct ibcs2_sys_umount_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_umount
	},		/* 22 = umount */
	{
		ns(struct ibcs2_sys_setuid_args),
		.sy_call = (sy_call_t *)ibcs2_sys_setuid
	},		/* 23 = setuid */
	{
		.sy_call = (sy_call_t *)sys_getuid_with_euid
	},		/* 24 = getuid_with_euid */
	{
		ns(struct ibcs2_sys_stime_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_stime
	},		/* 25 = stime */
	{
		.sy_call = sys_nosys,
	},		/* 26 = filler */
	{
		ns(struct ibcs2_sys_alarm_args),
		.sy_call = (sy_call_t *)ibcs2_sys_alarm
	},		/* 27 = alarm */
	{
		ns(struct ibcs2_sys_fstat_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_fstat
	},		/* 28 = fstat */
	{
		.sy_call = (sy_call_t *)ibcs2_sys_pause
	},		/* 29 = pause */
	{
		ns(struct ibcs2_sys_utime_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_utime
	},		/* 30 = utime */
	{
		.sy_call = sys_nosys,
	},		/* 31 = filler */
	{
		ns(struct ibcs2_sys_gtty_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_gtty
	},		/* 32 = gtty */
	{
		ns(struct ibcs2_sys_access_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_access
	},		/* 33 = access */
	{
		ns(struct ibcs2_sys_nice_args),
		.sy_call = (sy_call_t *)ibcs2_sys_nice
	},		/* 34 = nice */
	{
		ns(struct ibcs2_sys_statfs_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_statfs
	},		/* 35 = statfs */
	{
		.sy_call = (sy_call_t *)sys_sync
	},		/* 36 = sync */
	{
		ns(struct ibcs2_sys_kill_args),
		.sy_call = (sy_call_t *)ibcs2_sys_kill
	},		/* 37 = kill */
	{
		ns(struct ibcs2_sys_fstatfs_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_fstatfs
	},		/* 38 = fstatfs */
	{
		ns(struct ibcs2_sys_pgrpsys_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_pgrpsys
	},		/* 39 = pgrpsys */
	{
		.sy_call = sys_nosys,
	},		/* 40 = filler */
	{
		ns(struct sys_dup_args),
		.sy_call = (sy_call_t *)sys_dup
	},		/* 41 = dup */
	{
		.sy_call = (sy_call_t *)sys_pipe
	},		/* 42 = pipe */
	{
		ns(struct ibcs2_sys_times_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_times
	},		/* 43 = times */
	{
		.sy_call = sys_nosys,
	},		/* 44 = filler */
	{
		ns(struct ibcs2_sys_plock_args),
		.sy_call = (sy_call_t *)ibcs2_sys_plock
	},		/* 45 = plock */
	{
		ns(struct ibcs2_sys_setgid_args),
		.sy_call = (sy_call_t *)ibcs2_sys_setgid
	},		/* 46 = setgid */
	{
		.sy_call = (sy_call_t *)sys_getgid_with_egid
	},		/* 47 = getgid_with_egid */
	{
		ns(struct ibcs2_sys_sigsys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_sigsys
	},		/* 48 = sigsys */
#ifdef SYSVMSG
	{
		ns(struct ibcs2_sys_msgsys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_msgsys
	},		/* 49 = msgsys */
#else
	{
		.sy_call = sys_nosys,
	},		/* 49 = filler */
#endif
	{
		ns(struct ibcs2_sys_sysmachine_args),
		.sy_call = (sy_call_t *)ibcs2_sys_sysmachine
	},		/* 50 = sysmachine */
	{
		.sy_call = sys_nosys,
	},		/* 51 = filler */
#ifdef SYSVSHM
	{
		ns(struct ibcs2_sys_shmsys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_shmsys
	},		/* 52 = shmsys */
#else
	{
		.sy_call = sys_nosys,
	},		/* 52 = filler */
#endif
#ifdef SYSVSEM
	{
		ns(struct ibcs2_sys_semsys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_semsys
	},		/* 53 = semsys */
#else
	{
		.sy_call = sys_nosys,
	},		/* 53 = filler */
#endif
	{
		ns(struct ibcs2_sys_ioctl_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_ioctl
	},		/* 54 = ioctl */
	{
		ns(struct ibcs2_sys_uadmin_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_uadmin
	},		/* 55 = uadmin */
	{
		.sy_call = sys_nosys,
	},		/* 56 = filler */
	{
		ns(struct ibcs2_sys_utssys_args),
		.sy_call = (sy_call_t *)ibcs2_sys_utssys
	},		/* 57 = utssys */
	{
		ns(struct sys_fsync_args),
		.sy_call = (sy_call_t *)sys_fsync
	},		/* 58 = fsync */
	{
		ns(struct ibcs2_sys_execve_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_execve
	},		/* 59 = execve */
	{
		ns(struct sys_umask_args),
		.sy_call = (sy_call_t *)sys_umask
	},		/* 60 = umask */
	{
		ns(struct sys_chroot_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_chroot
	},		/* 61 = chroot */
	{
		ns(struct ibcs2_sys_fcntl_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_fcntl
	},		/* 62 = fcntl */
	{
		ns(struct ibcs2_sys_ulimit_args),
		.sy_call = (sy_call_t *)ibcs2_sys_ulimit
	},		/* 63 = ulimit */
	{
		.sy_call = sys_nosys,
	},		/* 64 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 65 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 66 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 67 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 68 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 69 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 70 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 71 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 72 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 73 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 74 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 75 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 76 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 77 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 78 = filler */
	{
		ns(struct sys_rmdir_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_rmdir
	},		/* 79 = rmdir */
	{
		ns(struct sys_mkdir_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_mkdir
	},		/* 80 = mkdir */
	{
		ns(struct ibcs2_sys_getdents_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_getdents
	},		/* 81 = getdents */
	{
		.sy_call = sys_nosys,
	},		/* 82 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 83 = filler */
	{
		ns(struct ibcs2_sys_sysfs_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sysfs
	},		/* 84 = sysfs */
	{
		ns(struct ibcs2_sys_getmsg_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_getmsg
	},		/* 85 = getmsg */
	{
		ns(struct ibcs2_sys_putmsg_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_putmsg
	},		/* 86 = putmsg */
	{
		ns(struct sys_poll_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_poll
	},		/* 87 = poll */
	{
		.sy_call = sys_nosys,
	},		/* 88 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 89 = filler */
	{
		ns(struct sys_symlink_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_symlink
	},		/* 90 = symlink */
	{
		ns(struct ibcs2_sys_lstat_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_lstat
	},		/* 91 = lstat */
	{
		ns(struct sys_readlink_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_readlink
	},		/* 92 = readlink */
	{
		ns(struct sys_fchmod_args),
		.sy_call = (sy_call_t *)sys_fchmod
	},		/* 93 = fchmod */
	{
		ns(struct sys___posix_fchown_args),
		.sy_call = (sy_call_t *)sys___posix_fchown
	},		/* 94 = fchown */
	{
		.sy_call = sys_nosys,
	},		/* 95 = filler */
	{
		ns(struct compat_16_sys___sigreturn14_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)compat_16_sys___sigreturn14
	},		/* 96 = sigreturn */
	{
		ns(struct ibcs2_sys_sigaltstack_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sigaltstack
	},		/* 97 = sigaltstack */
	{
		.sy_call = sys_nosys,
	},		/* 98 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 99 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 100 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 101 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 102 = filler */
	{
		ns(struct ibcs2_sys_statvfs_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_statvfs
	},		/* 103 = statvfs */
	{
		ns(struct ibcs2_sys_fstatvfs_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_fstatvfs
	},		/* 104 = fstatvfs */
	{
		.sy_call = sys_nosys,
	},		/* 105 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 106 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 107 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 108 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 109 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 110 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 111 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 112 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 113 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 114 = filler */
	{
		ns(struct ibcs2_sys_mmap_args),
		.sy_call = (sy_call_t *)ibcs2_sys_mmap
	},		/* 115 = mmap */
	{
		ns(struct sys_mprotect_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_mprotect
	},		/* 116 = mprotect */
	{
		ns(struct sys_munmap_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_munmap
	},		/* 117 = munmap */
	{
		.sy_call = sys_nosys,
	},		/* 118 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 119 = filler */
	{
		ns(struct sys_fchdir_args),
		.sy_call = (sy_call_t *)sys_fchdir
	},		/* 120 = fchdir */
	{
		ns(struct sys_readv_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_readv
	},		/* 121 = readv */
	{
		ns(struct sys_writev_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys_writev
	},		/* 122 = writev */
	{
		.sy_call = sys_nosys,
	},		/* 123 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 124 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 125 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 126 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 127 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 128 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 129 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 130 = filler */
	{
		ns(struct ibcs2_sys_memcntl_args),
		.sy_call = (sy_call_t *)ibcs2_sys_memcntl
	},		/* 131 = memcntl */
	{
		.sy_call = sys_nosys,
	},		/* 132 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 133 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 134 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 135 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 136 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 137 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 138 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 139 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 140 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 141 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 142 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 143 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 144 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 145 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 146 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 147 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 148 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 149 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 150 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 151 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 152 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 153 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 154 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 155 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 156 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 157 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 158 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 159 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 160 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 161 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 162 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 163 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 164 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 165 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 166 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 167 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 168 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 169 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 170 = filler */
	{
		ns(struct ibcs2_sys_gettimeofday_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_gettimeofday
	},		/* 171 = gettimeofday */
	{
		ns(struct ibcs2_sys_settimeofday_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_settimeofday
	},		/* 172 = settimeofday */
	{
		.sy_call = sys_nosys,
	},		/* 173 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 174 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 175 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 176 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 177 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 178 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 179 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 180 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 181 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 182 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 183 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 184 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 185 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 186 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 187 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 188 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 189 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 190 = filler */
	{
		ns(struct compat_43_sys_truncate_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)compat_43_sys_truncate
	},		/* 191 = truncate */
	{
		ns(struct compat_43_sys_ftruncate_args),
		.sy_call = (sy_call_t *)compat_43_sys_ftruncate
	},		/* 192 = ftruncate */
	{
		.sy_call = sys_nosys,
	},		/* 193 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 194 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 195 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 196 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 197 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 198 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 199 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 200 = filler */
	{
		ns(struct xenix_sys_locking_args),
		.sy_call = (sy_call_t *)xenix_sys_locking
	},		/* 201 = locking */
	{
		.sy_call = sys_nosys,
	},		/* 202 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 203 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 204 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 205 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 206 = filler */
	{
		ns(struct xenix_sys_rdchk_args),
		.sy_call = (sy_call_t *)xenix_sys_rdchk
	},		/* 207 = rdchk */
	{
		.sy_call = sys_nosys,
	},		/* 208 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 209 = filler */
	{
		ns(struct xenix_sys_chsize_args),
		.sy_call = (sy_call_t *)xenix_sys_chsize
	},		/* 210 = chsize */
	{
		ns(struct xenix_sys_ftime_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)xenix_sys_ftime
	},		/* 211 = ftime */
	{
		ns(struct xenix_sys_nap_args),
		.sy_call = (sy_call_t *)xenix_sys_nap
	},		/* 212 = nap */
	{
		.sy_call = sys_nosys,
	},		/* 213 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 214 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 215 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 216 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 217 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 218 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 219 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 220 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 221 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 222 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 223 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 224 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 225 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 226 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 227 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 228 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 229 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 230 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 231 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 232 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 233 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 234 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 235 = filler */
	{
		ns(struct compat_50_sys_select_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)compat_50_sys_select
	},		/* 236 = select */
	{
		ns(struct ibcs2_sys_eaccess_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_eaccess
	},		/* 237 = eaccess */
	{
		.sy_call = sys_nosys,
	},		/* 238 = filler */
	{
		ns(struct ibcs2_sys_sigaction_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sigaction
	},		/* 239 = sigaction */
	{
		ns(struct ibcs2_sys_sigprocmask_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sigprocmask
	},		/* 240 = sigprocmask */
	{
		ns(struct ibcs2_sys_sigpending_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sigpending
	},		/* 241 = sigpending */
	{
		ns(struct ibcs2_sys_sigsuspend_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_sigsuspend
	},		/* 242 = sigsuspend */
	{
		ns(struct ibcs2_sys_getgroups_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_getgroups
	},		/* 243 = getgroups */
	{
		ns(struct ibcs2_sys_setgroups_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_setgroups
	},		/* 244 = setgroups */
	{
		ns(struct ibcs2_sys_sysconf_args),
		.sy_call = (sy_call_t *)ibcs2_sys_sysconf
	},		/* 245 = sysconf */
	{
		ns(struct ibcs2_sys_pathconf_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_pathconf
	},		/* 246 = pathconf */
	{
		ns(struct ibcs2_sys_fpathconf_args),
		.sy_call = (sy_call_t *)ibcs2_sys_fpathconf
	},		/* 247 = fpathconf */
	{
		ns(struct sys___posix_rename_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)sys___posix_rename
	},		/* 248 = __posix_rename */
	{
		.sy_call = sys_nosys,
	},		/* 249 = filler */
	{
		ns(struct ibcs2_sys_scoinfo_args),
		.sy_flags = SYCALL_ARG_PTR,
		.sy_call = (sy_call_t *)ibcs2_sys_scoinfo
	},		/* 250 = scoinfo */
	{
		.sy_call = sys_nosys,
	},		/* 251 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 252 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 253 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 254 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 255 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 256 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 257 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 258 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 259 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 260 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 261 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 262 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 263 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 264 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 265 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 266 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 267 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 268 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 269 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 270 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 271 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 272 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 273 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 274 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 275 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 276 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 277 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 278 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 279 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 280 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 281 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 282 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 283 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 284 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 285 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 286 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 287 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 288 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 289 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 290 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 291 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 292 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 293 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 294 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 295 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 296 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 297 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 298 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 299 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 300 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 301 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 302 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 303 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 304 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 305 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 306 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 307 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 308 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 309 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 310 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 311 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 312 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 313 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 314 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 315 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 316 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 317 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 318 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 319 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 320 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 321 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 322 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 323 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 324 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 325 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 326 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 327 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 328 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 329 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 330 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 331 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 332 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 333 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 334 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 335 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 336 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 337 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 338 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 339 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 340 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 341 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 342 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 343 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 344 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 345 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 346 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 347 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 348 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 349 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 350 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 351 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 352 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 353 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 354 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 355 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 356 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 357 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 358 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 359 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 360 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 361 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 362 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 363 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 364 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 365 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 366 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 367 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 368 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 369 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 370 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 371 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 372 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 373 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 374 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 375 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 376 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 377 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 378 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 379 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 380 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 381 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 382 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 383 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 384 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 385 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 386 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 387 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 388 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 389 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 390 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 391 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 392 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 393 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 394 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 395 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 396 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 397 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 398 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 399 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 400 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 401 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 402 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 403 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 404 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 405 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 406 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 407 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 408 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 409 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 410 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 411 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 412 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 413 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 414 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 415 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 416 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 417 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 418 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 419 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 420 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 421 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 422 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 423 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 424 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 425 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 426 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 427 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 428 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 429 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 430 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 431 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 432 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 433 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 434 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 435 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 436 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 437 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 438 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 439 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 440 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 441 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 442 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 443 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 444 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 445 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 446 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 447 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 448 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 449 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 450 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 451 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 452 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 453 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 454 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 455 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 456 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 457 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 458 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 459 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 460 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 461 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 462 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 463 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 464 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 465 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 466 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 467 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 468 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 469 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 470 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 471 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 472 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 473 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 474 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 475 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 476 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 477 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 478 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 479 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 480 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 481 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 482 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 483 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 484 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 485 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 486 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 487 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 488 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 489 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 490 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 491 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 492 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 493 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 494 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 495 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 496 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 497 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 498 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 499 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 500 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 501 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 502 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 503 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 504 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 505 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 506 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 507 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 508 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 509 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 510 = filler */
	{
		.sy_call = sys_nosys,
	},		/* 511 = filler */
};
