#include "privops.h"
#include "session.h"
#include "privsock.h"
#include "tunables.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>

void priv_op_check(session_t *sess)
{
	int p = 0;
	if (sess->pasv_listen_fd != -1)
		p = 1;
	priv_sock_send_res(sess->parent_fd, p);
}

void priv_op_close(session_t *sess)
{
	if (sess->pasv_listen_fd != -1) {
		if (close(sess->pasv_listen_fd) < 0)
			priv_sock_send_res(sess->parent_fd, -1);
		else
			priv_sock_send_res(sess->parent_fd, 0);
		sess->pasv_listen_fd = -1;
	}
}

void priv_op_listen(session_t *sess)
{
	sess->pasv_listen_fd = tcp_server(tunable_listen_addr, 20);
	priv_sock_send_res(sess->parent_fd, 20);
}

void priv_op_accept(session_t *sess)
{}

void priv_op_get(session_t *sess)
{}