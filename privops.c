#include "privops.h"
#include "session.h"
#include "privsock.h"
#include <stdio.h>

void priv_op_check(session_t *sess)
{
	int p = 0;
	if (sess->cliaddr != NULL)
		p = 2;
	if (sess->pasv_listen_fd != -1)
		p = 1;
	priv_sock_send_res(sess->parent_fd, p);
}

void priv_op_close(session_t *sess)
{}

void priv_op_listen(session_t *sess)
{}

void priv_op_accept(session_t *sess)
{}

void priv_op_get(session_t *sess)
{}