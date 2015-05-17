#include "privsock.h"
#include "session.h"
#include "err.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/socket.h>

void priv_sock_init(session_t *sess)
{
	int fds[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
		ERROR_EXIT("socketpair");
	sess->parent_fd = fds[0];
	sess->child_fd = fds[1];
}

void priv_sock_send_cmd(int fd, char cmd)
{
	if (writen(fd, &cmd, sizeof(cmd)) != sizeof(cmd))
		ERROR_EXIT("writen");
}

char priv_sock_recv_cmd(int fd)
{
	char c;
	int r;
	if ((r = readn(fd, &c, sizeof(c))) == 0)
		exit(0);
	if (r != sizeof(c))
		ERROR_EXIT("readn");
	return c;
}

void priv_sock_send_res(int fd, int res)
{
	if (writen(fd, &res, sizeof(res)) != sizeof(res))
		ERROR_EXIT("writen");
}

int priv_sock_recv_res(int fd)
{
	int r;
	if (readn(fd, &r, sizeof(r)) != sizeof(r))
		ERROR_EXIT("readn");
	return r;
}

void priv_sock_send_int(int fd, int num)
{
	if (writen(fd, &num, sizeof(num)) != sizeof(num))
		ERROR_EXIT("writen");
}

int priv_sock_recv_int(int fd)
{
	int r;
	if (readn(fd, &r, sizeof(r)) != sizeof(r))
		ERROR_EXIT("readn");
	return r;
}

void priv_sock_send_str(int fd, const char *str, int len)
{
	priv_sock_send_int(fd, len);
	if (writen(fd, str, len) != len)
		ERROR_EXIT("writen");
}

void priv_sock_recv_str(int fd, char *buf, int len)
{
	int rlen = priv_sock_recv_int(fd);
	if (rlen > len)
		ERROR_EXIT("priv_sock_recv_str");
	if (readn(fd, buf, rlen) != rlen)
		ERROR_EXIT("priv_sock_recv_str");
}

void priv_sock_send_fd(int fd, int sfd)
{
	send_fd(fd, sfd);
}

int priv_sock_recv_fd(int fd)
{
	return recv_fd(fd);
}