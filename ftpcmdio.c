#include "ftpcmdio.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ftp_cmdio_write(int fd, int status, const char *msg)
{
	char buf[1024] = {0};
	snprintf(buf, sizeof(buf), "%d %s\r\n", status, msg);
	writen(fd, buf, strlen(buf));
}

void ftp_cmdio_write_m(int fd, int status, const char *msg)
{
	char buf[1024] = {0};
	snprintf(buf, sizeof(buf), "%d-%s\r\n", status, msg);
	writen(fd, buf, strlen(buf));
}

void ftp_cmdio_write_exit(int fd, int status, const char *msg)
{
	ftp_cmdio_write(fd, status, msg);
	exit(0);
}

int ftp_cmdio_get_cmd_args(int fd, void *buf, int len)
{
	int rd, lf, r, i, total = 0;
	char *ptr;
	lf = len - 1;
	ptr = buf;
	while (lf > 0) {
		r = recv_peek(fd, buf, lf);
		if (r <= 0)
			return r;

		rd = r;
		for (i = 0; i < rd; i++) {
			if (*(ptr + i) == '\n') {
				r = readn(fd, ptr, i + 1);
				if (r != i+1)
					return -1;
				total += r;
				ptr += r;
				*ptr = 0;
				return total;
			}
		}

		r = readn(fd, ptr, rd);
		if (r != rd)
			return -1;
		total += r;
		ptr += r;
		lf -= r;
	}
	*ptr = 0;
	return len - 1;
}