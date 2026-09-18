/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * redoubt_abi.h - ABI shared by the Redoubt kernel module and userspace CLI.
 *
 * Rules (see docs/KMOD_ABI.md):
 *  - Every struct is fixed-size, naturally aligned, no pointers except as u64.
 *  - Variable-length results use a header + caller-supplied buffer (u64 ptr).
 *  - Additive changes bump RDT_ABI_MINOR; breaking changes bump RDT_ABI_MAJOR.
 */
#ifndef REDOUBT_ABI_H
#define REDOUBT_ABI_H

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
typedef __u32 rdt_u32;
typedef __u64 rdt_u64;
#else
#include <stdint.h>
typedef uint32_t rdt_u32;
typedef uint64_t rdt_u64;
#if defined(__linux__)
#include <sys/ioctl.h>
#else
/* Host-only fallback so unit tests build on non-Linux dev machines. */
#define _IOC_FALLBACK(dir, type, nr, size) \
	(((unsigned long)(dir) << 30) | ((unsigned long)(size) << 16) | \
	 ((unsigned long)(type) << 8) | (unsigned long)(nr))
#define _IOR(type, nr, t) _IOC_FALLBACK(2, type, nr, sizeof(t))
#endif
#endif

#define RDT_ABI_MAJOR 0
#define RDT_ABI_MINOR 1

#define RDT_DEVICE_NAME "redoubt"
#define RDT_DEVICE_PATH "/dev/" RDT_DEVICE_NAME

#define RDT_IOC_MAGIC 'R'

struct rdt_version {
	rdt_u32 abi_major;
	rdt_u32 abi_minor;
	rdt_u32 kernel_version_code; /* LINUX_VERSION_CODE the module was built for */
	rdt_u32 flags;               /* reserved, must be 0 */
};

/* Session 1: only GET_VERSION exists. Later ioctls are reserved in KMOD_ABI.md:
 *   0x02 LIST_MODULES, 0x03 GET_TEXT_RANGE, 0x04 READ_SYSCALL_TABLE,
 *   0x05 LIST_TASKS,   0x06 LIST_FTRACE
 */
#define RDT_GET_VERSION _IOR(RDT_IOC_MAGIC, 0x01, struct rdt_version)

#endif /* REDOUBT_ABI_H */
