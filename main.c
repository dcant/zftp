#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include "hash.h"
#include "utils.h"
#include "session.h"
#include "tunables.h"
#include "err.h"
#include "parseconf.h"
#include "connrec.h"
#include "zsignal.h"

const char *conf = "ftp.conf";

int main()
{
	check_privilege();
//	set_daemon();

	init_conf(conf);

	set_dir_root();

	init_recorder();

	set_signal_child();

	session_t sess;
	init_session(&sess);
	ps = &sess;

	int listenfd = tcp_server(tunable_listen_addr, tunable_listen_port);

	main_loop(&sess, listenfd, tunable_accept_timeout);

	return 0;
}