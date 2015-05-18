#include "zsignal.h"
#include "connrec.h"
#include "session.h"
#include "tunables.h"
#include "ftpcmdio.h"
#include "ftpcode.h"
#include "err.h"
#include "str.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

session_t *ps;
static void _signal_alarm_ctrl_handler(int);
static void _signal_urg_handler(int);
static void _signal_data_handler(int);

sighandler_t zsignal(int signum, sighandler_t handler)
{
	struct sigaction action;
	struct sigaction oldaction;

	action.sa_handler = handler;
	if (sigemptyset(&action.sa_mask) < 0)
		ERROR_EXIT("sigemptyset");
	action.sa_flags = SA_RESTART;

	if (sigaction(signum, &action, &oldaction) < 0)
		ERROR_EXIT("sigaction");

	return oldaction.sa_handler;
}

void set_signal_child()
{
	zsignal(SIGCHLD, signal_child_handler);
}

void set_signal_alarm_ctrl()
{
	zsignal(SIGALRM, _signal_alarm_ctrl_handler);
}

static void _signal_alarm_ctrl_handler(int sig)
{
	if (tunable_ctrl_conn_idle_time > 0) {
		shutdown(ps->ctrl_fd, SHUT_RD);
		ftp_cmdio_write(ps->ctrl_fd, FTP_CLOSE, "Timeout");
		shutdown(ps->ctrl_fd, SHUT_WR);
		exit(0);
	}
}

void set_signal_urg()
{
	zsignal(SIGURG, _signal_urg_handler);
}

static void _signal_urg_handler(int sig)
{
	if (ftp_cmdio_get_cmd_args(ps->ctrl_fd, ps->ftp_cmd_line, MAXCMD) <= 0)
		ERROR_EXIT("ftp_cmdio_get_cmd_args");
	str_trim_crlf(ps->ftp_cmd_line);
	str_split(ps->ftp_cmd_line, ps->ftp_cmd_op, ps->ftp_cmd_arg, ' ');
	str_toupper(ps->ftp_cmd_op);
	if (strcmp(ps->ftp_cmd_op, "ABOR") == 0) {
		ps->is_abor_recved = 1;
		close(ps->data_fd);
		ps->data_fd = -1;
	} else
		ftp_cmdio_write(ps->ctrl_fd, FTP_CMDE, "Unrecognized cmd");
}

void enable_signal_urg(int fd)
{
	if (fcntl(fd, F_SETOWN, getpid()) < 0)
		ERROR_EXIT("fcntl");
}

void enable_msg_oobinline(int fd)
{
	int oobinline = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_OOBINLINE,
		&oobinline, sizeof(oobinline)) < 0)
		ERROR_EXIT("setsockopt");
}

void start_signal_alarm_ctrl()
{
	alarm(tunable_ctrl_conn_idle_time);
}

static void _signal_data_handler(int sig)
{
	if (tunable_data_conn_idle_time > 0) {
		if (ps->is_transmitting) {
			start_signal_alarm_data();
		} else {
			close(ps->data_fd);
			shutdown(ps->ctrl_fd, SHUT_RD);
			ftp_cmdio_write(ps->ctrl_fd, FTP_CLOSE, "Timeout.");
		}
	}
}

void set_signal_alarm_data()
{
	zsignal(SIGALRM, _signal_data_handler);
}

void start_signal_alarm_data()
{
	alarm(tunable_data_conn_idle_time);
}