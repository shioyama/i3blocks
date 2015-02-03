/*
 * io.c - non-blocking I/O operations
 * Copyright (C) 2015  Vivien Didelot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE /* for F_SETSIG */

#include <fcntl.h>
#include <unistd.h>
#include "log.h"

int
io_signal(int fd, int sig)
{
	int flags;

	/* Assign the signal for this file descriptor */
	if (fcntl(fd, F_SETSIG, sig) == -1) {
		errorx("failed to set signal %d on fd %d", sig, fd);
		return 1;
	}

	/* Set owner process that is to receive "I/O possible" signal */
	if (fcntl(fd, F_SETOWN, getpid()) == -1) {
		errorx("failed to set process as owner of fd %d", fd);
		return 1;
	}

	flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
		errorx("failed to get flags of fd %d", fd);
		return 1;
	}

	/* Enable "I/O possible" signaling and make I/O nonblocking */
	if (fcntl(fd, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1) {
		errorx("failed to enable I/O signaling for fd %d", fd);
		return 1;
	}

	return 0;
}
