// SPDX-License-Identifier: GPL-2.0-only
/*
 * redoubt_kmod - read-only kernel introspection for the Redoubt rootkit detector.
 *
 * Session 1 scope: register /dev/redoubt (root-only) and answer RDT_GET_VERSION.
 * This module MUST NOT modify kernel state. See docs/KMOD_ABI.md.
 */
#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <redoubt_abi.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static long rdt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	switch (cmd) {
	case RDT_GET_VERSION: {
		struct rdt_version v = {
			.abi_major = RDT_ABI_MAJOR,
			.abi_minor = RDT_ABI_MINOR,
			.kernel_version_code = LINUX_VERSION_CODE,
			.flags = 0,
		};

		if (copy_to_user((void __user *)arg, &v, sizeof(v)))
			return -EFAULT;
		return 0;
	}
	default:
		return -ENOTTY;
	}
}

static const struct file_operations rdt_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = rdt_ioctl,
	.compat_ioctl = rdt_ioctl,
	.llseek = noop_llseek,
};

static struct miscdevice rdt_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = RDT_DEVICE_NAME,
	.fops = &rdt_fops,
	.mode = 0600,
};

static int __init rdt_init(void)
{
	int ret = misc_register(&rdt_dev);

	if (ret)
		return ret;
	pr_info("loaded (abi %d.%d)\n", RDT_ABI_MAJOR, RDT_ABI_MINOR);
	return 0;
}

static void __exit rdt_exit(void)
{
	misc_deregister(&rdt_dev);
	pr_info("unloaded\n");
}

module_init(rdt_init);
module_exit(rdt_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Redoubt read-only kernel introspection module");
MODULE_AUTHOR("Redoubt contributors");
MODULE_VERSION("0.1.0");
