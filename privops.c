#include "privops.h"
#include "session.h"
#include "privsock.h"
#include "tunables.h"
#include "utils.h"
#include "err.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	if (sess->pasv_listen_fd != -1)
		close(sess->pasv_listen_fd);
	sess->pasv_listen_fd = tcp_server(tunable_listen_addr, 20);
	priv_sock_send_res(sess->parent_fd, 20);
}

void priv_op_accept(session_t *sess)
{
	int fd = accept_timeout(sess->pasv_listen_fd, NULL,
		tunable_accept_timeout);
	close(sess->pasv_listen_fd);
	sess->pasv_listen_fd = -1;
	if (fd == -1) {
		priv_sock_send_res(sess->parent_fd, -1);
		ERROR_EXIT("priv_sock_send_res");
	}
	priv_sock_send_res(sess->parent_fd, 0);
	priv_sock_send_fd(sess->parent_fd, fd);
}

void priv_op_get(session_t *sess)
{
	char ip[16] = {0};
	priv_sock_recv_str(sess->parent_fd, ip, sizeof(ip));
	uint16_t port = priv_sock_recv_int(sess->parent_fd);

	int fd = tcp_client(tunable_listen_addr, 20);
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	if (connect_timeout(fd, &saddr, tunable_connect_timeout) < 0) {
		priv_sock_send_res(sess->parent_fd, -1);
		close(fd);
	}
		
	priv_sock_send_res(sess->parent_fd, 0);
	priv_sock_send_fd(sess->parent_fd, fd);
}