#include "process.h"
#include "session.h"
#include "ftpcode.h"
#include "ftpcmdio.h"
#include "zsignal.h"
#include "tunables.h"
#include "err.h"
#include "str.h"
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <unistd.h>

#include <stdio.h>

static void _reset_session_cmd(session_t *sess);
static void _handle_map(session_t *sess);
static void _handle_user(session_t *sess);
static void _handle_pass(session_t *sess);
static void _handle_port(session_t *sess);
static void _handle_pasv(session_t *sess);
static void _handle_type(session_t *sess);
static void _handle_mode(session_t *sess);
static void _handle_stru(session_t *sess);
static void _handle_retr(session_t *sess);
static void _handle_stor(session_t *sess);
static void _handle_noop(session_t *sess);
static void _handle_abor(session_t *sess);
static void _handle_quit(session_t *sess);


void set_child_context(session_t *sess)
{
	close(sess->parent_fd);
	sess->parent_fd = -1;
	set_signal_alarm_ctrl();
	set_signal_urg();
	enable_signal_urg(sess->ctrl_fd);
	enable_msg_oobinline(sess->ctrl_fd);
}

void set_parent_context(session_t *sess)
{
	close(sess->ctrl_fd);
	close(sess->child_fd);
	sess->child_fd = -1;
}

void handle_ftpcmd(session_t *sess)
{
	char buf[MAXCMD] = {0};
	sprintf(buf, "Welcome to zftp!\r\n%d", FTP_READY);
	ftp_cmdio_write_m(sess->ctrl_fd, FTP_READY, buf);
	while (1) {
		start_signal_alarm_ctrl();
		_reset_session_cmd(sess);

		int ret = ftp_cmdio_get_cmd_args(sess->ctrl_fd, sess->ftp_cmd_line,
			MAXCMD);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			ERROR_EXIT("ftp_cmdio_get_cmd_args");
		} else if (ret == 0)
			exit(0);

		str_trim_crlf(sess->ftp_cmd_line);
		str_split(sess->ftp_cmd_line, sess->ftp_cmd_op,
			sess->ftp_cmd_arg, ' ');
		str_toupper(sess->ftp_cmd_op);

		_handle_map(sess);
	}
}

void handle_childcmd(session_t *sess)
{
	while (1);
}

static void _reset_session_cmd(session_t *sess)
{
	memset(sess->ftp_cmd_line, 0, sizeof(sess->ftp_cmd_line));
	memset(sess->ftp_cmd_op, 0, sizeof(sess->ftp_cmd_op));
	memset(sess->ftp_cmd_arg, 0, sizeof(sess->ftp_cmd_arg));
}

typedef struct cmd {
	const char *op;
	void (*handler)(session_t *sess);
} cmd_t;

static cmd_t map[] =
{
	{"USER", _handle_user},
	{"PASS", _handle_pass},
	{"PORT", _handle_port},
	{"PASV", _handle_pasv},
	{"TYPE", _handle_type},
	{"MODE", _handle_mode},
	{"STRU", _handle_stru},
	{"RETR", _handle_retr},
	{"STOR", _handle_stor},
	{"NOOP", _handle_noop},
	{"ABOR", _handle_abor},
	{"\377\364\377\362ABOR", _handle_abor},
	{"QUIT", _handle_quit}
};

static void _handle_map(session_t *sess)
{
	int i, cnum = sizeof(map) / sizeof(cmd_t);
	for (i = 0; i < cnum; i++) {
		if (strcmp(map[i].op, sess->ftp_cmd_op) == 0) {
			if (map[i].handler != NULL)
				map[i].handler(sess);
			else
				ftp_cmdio_write(sess->ctrl_fd, FTP_NEXEC, "Unimplemented cmd");
			return;
		}
	}
	ftp_cmdio_write(sess->ctrl_fd, FTP_CMDE, "Unrecognized cmd");
}

static void _handle_user(session_t *sess)
{
	if (strlen(sess->ftp_cmd_arg) == 0) {
		ftp_cmdio_write(sess->ctrl_fd, FTP_ARGE, "Wrong arg");
		return;
	}
	char buf[MAXCMD] = {0};
	strcpy(buf, sess->ftp_cmd_arg);
	str_toupper(buf);
	if (strcmp(buf, "ANONYMOUS") == 0) {
		sess->userid = -1;
		ftp_cmdio_write(sess->ctrl_fd, FTP_PASS, "Please specify password");
		return;
	}
	struct passwd *pw = getpwnam(sess->ftp_cmd_arg);
	if (pw == NULL) {
		ftp_cmdio_write(sess->ctrl_fd, FTP_NLOGIN, "Login failed");
		return;
	}
	sess->userid = pw->pw_uid;
	ftp_cmdio_write(sess->ctrl_fd, FTP_PASS, "Please specify password");
}

static void _handle_pass(session_t *sess)
{
	char buf[MAXCMD];
	if (sess->userid = -1) {
		sess->is_login = 1;
		memset(buf, 0, MAXCMD);
		sprintf(buf, "\r\n", FTP_LOGIN);
		sprintf(buf, "%s%d-  Max %d connections a IP", buf, FTP_LOGIN,
			tunable_max_conn_per_ip);
		sprintf(buf, "%s\r\n%d-\r\n%d-  Only download permitted.", buf,
			FTP_LOGIN, FTP_LOGIN);
		sprintf(buf, "%s\r\n%d-\r\n%d-  @zftp", buf, FTP_LOGIN, FTP_LOGIN);
		sprintf(buf, "%s\r\n%d-\r\n%d Login successful.", buf, FTP_LOGIN,
			FTP_LOGIN);
		ftp_cmdio_write_m(sess->ctrl_fd, FTP_LOGIN, buf);
		return;
	} else {
		struct passwd *pw = getpwuid(sess->userid);
		if (pw == NULL) {
			sess->userid = -2;
			ftp_cmdio_write(sess->ctrl_fd, FTP_NLOGIN, "Login failed");
			return;
		}
		struct spwd *spw = getspnam(pw->pw_name);
		if (spw == NULL) {
			sess->userid = -2;
			ftp_cmdio_write(sess->ctrl_fd, FTP_NLOGIN, "Login failed");
			return;
		}
		char *encrypt = crypt(sess->ftp_cmd_arg, spw->sp_pwdp);
		if (encrypt == NULL || strcmp(encrypt, spw->sp_pwdp) != 0) {
			sess->userid = -2;
			ftp_cmdio_write(sess->ctrl_fd, FTP_NLOGIN, "Login failed");
			return;
		}
		sess->is_login = 1;
		memset(buf, 0, MAXCMD);
		sprintf(buf, "\r\n", FTP_LOGIN);
		sprintf(buf, "%s%d-  Max %d connections a IP", buf, FTP_LOGIN,
			tunable_max_conn_per_ip);
		sprintf(buf, "%s\r\n%d-\r\n%d-  @zftp", buf, FTP_LOGIN, FTP_LOGIN);
		sprintf(buf, "%s\r\n%d-\r\n%d Login successful.", buf, FTP_LOGIN,
			FTP_LOGIN);
		ftp_cmdio_write_m(sess->ctrl_fd, FTP_LOGIN, buf);
		return;
	}
}

static void _handle_port(session_t *sess)
{

}

static void _handle_pasv(session_t *sess)
{

}

static void _handle_type(session_t *sess)
{

}

static void _handle_mode(session_t *sess)
{

}

static void _handle_stru(session_t *sess)
{

}

static void _handle_retr(session_t *sess)
{

}

static void _handle_stor(session_t *sess)
{

}

static void _handle_noop(session_t *sess)
{

}

static void _handle_abor(session_t *sess)
{

}

static void _handle_quit(session_t *sess)
{
	ftp_cmdio_write(sess->ctrl_fd, FTP_BYE, "Bye.");
	close(sess->ctrl_fd);
	exit(0);
}