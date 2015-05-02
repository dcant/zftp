#include "connrec.h"
#include "session.h"
#include "hash.h"
#include "err.h"
#include "tunables.h"
#include "ftpcmdio.h"
#include "ftpcode.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

static int clients_num = 0;
static hashdict_t *ip_client_num, *pid_ip;

static int _add_ip_client(uint32_t ip);
static void _add_pid_ip(pid_t pid, uint32_t ip);
static void _add_client(session_t *sess, uint32_t ip);
static void _check_limits(session_t *sess);

static int _add_ip_client(uint32_t ip)
{
	int *pv = (int *)hash_lookup_kv(ip_client_num, &ip, sizeof(ip));
	int num = 0;
	if (pv == NULL) {
		num = 1;
		hash_add_entry(ip_client_num, &ip, sizeof(ip), &num, sizeof(num));
	} else {
		num = *pv;
		num++;
		*pv = num;
	}
	return num;
}

void signal_child_handler(int signum)
{
	pid_t pid;
	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
		clients_num--;
		uint32_t *p = (uint32_t *)hash_lookup_kv(pid_ip, &pid, sizeof(pid));
		int *pv = (int *)hash_lookup_kv(ip_client_num, p, sizeof(p));
		(*pv)--;
		if (*pv == 0)
			hash_remove_entry(ip_client_num, p, sizeof(p));
		hash_remove_entry(pid_ip, &pid, sizeof(pid));
	}
	printf("child exit\n");
}

void init_recorder()
{
	ip_client_num = hash_init(NULL);
	pid_ip = hash_init(NULL);
}

static void _add_pid_ip(pid_t pid, uint32_t ip)
{
	hash_add_entry(pid_ip, &pid, sizeof(pid), &ip, sizeof(ip));
}

static void _add_client(session_t *sess, uint32_t ip)
{
	clients_num++;
	sess->clients_num = clients_num;
	sess->ip_client_num = _add_ip_client(ip);

	printf("%d %d\n", clients_num, sess->ip_client_num);
}


static void _check_limits(session_t *sess)
{
	if (tunable_max_clients > 0 && sess->clients_num > tunable_max_clients)
		ftp_cmdio_write_exit(sess->ctrl_fd, FTP_CLOSE,
			"Too many users, try again later!");
	else if (tunable_max_conn_per_ip > 0 &&
		sess->ip_client_num > tunable_max_conn_per_ip)
		ftp_cmdio_write_exit(sess->ctrl_fd, FTP_CLOSE,
			"Too many connections for one ip!");
}

void main_loop(session_t *sess, int fd, int tunable_accept_timeout)
{
	struct sockaddr_in clientaddr;
	int afd;
	pid_t pid;
	while (1) {
		memset(&clientaddr, 0, sizeof(clientaddr));
		if ((afd = accept_timeout(fd, &clientaddr, tunable_accept_timeout))
			== -1) {
			if (errno == ETIMEDOUT)
				continue;
			else
				ERROR_EXIT("accept_timeout");
		}
		printf("client connected!\n");
		uint32_t ip = clientaddr.sin_addr.s_addr;
		_add_client(sess, ip);
		if ((pid = fork()) == 0) {
			close(fd);
			sess->ctrl_fd = afd;
			_check_limits(sess);
			start_session(sess);
			exit(0);
		} else if (pid > 0) {
			_add_pid_ip(pid, ip);
			close(afd);
		} else
			ERROR_EXIT("fork");
	}
}