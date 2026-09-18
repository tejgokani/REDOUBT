// SPDX-License-Identifier: GPL-2.0-only
#include "source.h"

#include <errno.h>

/* Session 2: enumerate /sys/module/<name> directories for hidden-module. */
static int sysfs_probe(const struct source *self)
{
	(void)self;
#ifdef __linux__
	return 0;
#else
	return -ENOSYS;
#endif
}

const struct source source_sysfs = {
    .name = "sysfs",
    .probe = sysfs_probe,
};
