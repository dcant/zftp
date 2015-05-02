#include "parseconf.h"
#include "tunables.h"
#include <stdio.h>

int main()
{
	init_conf("ftp.conf");
	printf("tunable_anonymous_enable: %d\n", tunable_anonymous_enable);
	printf("tunable_local_enable: %d\n", tunable_local_enable);
	printf("tunable_pasv_enable: %d\n", tunable_pasv_enable);
	printf("tunable_port_enable: %d\n", tunable_port_enable);
	printf("tunable_anonymous_upload_enable: %d\n", tunable_anonymous_upload_enable);
	printf("tunable_ascii_mode_enable: %d\n", tunable_ascii_mode_enable);
	printf("tunable_bin_mode_enable: %d\n", tunable_bin_mode_enable);
	printf("tunable_max_clients: %d\n", tunable_max_clients);
	printf("tunable_max_conn_per_ip: %d\n", tunable_max_conn_per_ip);
	printf("tunable_ctrl_conn_idle_time: %d\n", tunable_ctrl_conn_idle_time);
	printf("tunable_data_conn_idle_time: %d\n", tunable_data_conn_idle_time);
	printf("tunable_accept_timeout: %d\n", tunable_accept_timeout);
	printf("tunable_connect_timeout: %d\n", tunable_connect_timeout);
	printf("tunable_listen_port: %d\n", tunable_listen_port);
	printf("tunable_listen_addr: %s\n", tunable_listen_addr);
	return 0;
}