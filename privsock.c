#include "privsock.h"
#include "session.h"
#include "err.h"
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