#include "prelogin.h"
#include "session.h"
#include "ftpcmdio.h"
#include "ftpcode.h"
#include "privsock.h"
#include "err.h"
#include "process.h"
#include <string.h>
#include <unistd.h>

void init_session(session_t *sess)
{
	sess->mode = 0;	// 0 -> bin  1 -> ascii
	sess->userid = -2;	// -1 represent anonymous

	sess->ctrl_fd = -1;
	sess->pasv_listen_fd = -1;
	sess->data_fd = -1;

	// command channel
	sess->parent_fd = -1;
	sess->child_fd = -1;

	sess->is_login = 0;
	sess->is_transmitting = 0;
	sess->is_abor_recved = 0;

	sess->retr_pos = 0;	// retrieve position

	sess->clients_num = 0;
	sess->ip_client_num = 0;

	sess->cliaddr = NULL;

	memset(sess->ftp_cmd_line, 0, sizeof(sess->ftp_cmd_line));
	memset(sess->ftp_cmd_op, 0, sizeof(sess->ftp_cmd_op));
	memset(sess->ftp_cmd_arg, 0, sizeof(sess->ftp_cmd_arg));
}

void start_session(session_t *sess)
{
	printf("start session\n");
	priv_sock_init(sess);
	pid_t pid = fork();
	if (pid == 0) {
		set_child_context(sess);
		handle_ftpcmd(sess);
	} else if (pid > 0) {
		set_parent_context(sess);
		handle_childcmd(sess);
	} else
		ERROR_EXIT("fork");
}