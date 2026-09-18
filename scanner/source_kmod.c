// SPDX-License-Identifier: GPL-2.0-only
#include "source.h"

#include <errno.h>
#include <stddef.h>

#ifdef __linux__
#include <fcntl.h>
#include <unistd.h>

#include "redoubt_abi.h"

int kmod_get_version(struct rdt_version *v)
{
	int fd = open(RDT_DEVICE_PATH, O_RDONLY | O_CLOEXEC);
	int rc = 0;

	if (fd < 0)
		return -errno;
	if (ioctl(fd, RDT_GET_VERSION, v) != 0)
		rc = -errno;
	close(fd);
	return rc;
}

static int kmod_probe(const struct source *self)
{
	struct rdt_version v;

	(void)self;
	return kmod_get_version(&v);
}
#else
static int kmod_probe(const struct source *self)
{
	(void)self;
	return -ENOSYS;
}
#endif

/* Session 2 adds list_modules / tasks / syscall-table ops backed by ioctls. */
const struct source source_kmod = {
    .name = "kmod",
    .probe = kmod_probe,
};
