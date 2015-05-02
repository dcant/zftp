#include "tunables.h"

int tunable_anonymous_enable = 1;		/* Allow anonymous login */
int tunable_local_enable = 1;			/* Allow local login */

int tunable_pasv_enable = 1;				/* PASV mode */
int tunable_port_enable = 1;				/* PORT mode */
int tunable_anonymous_upload_enable = 1;	/* Allow anonymoust upload */

int tunable_ascii_mode_enable = 1;			/* Enable ascii mode */
int tunable_bin_mode_enable = 1;			/* Enable bin mode */

int tunable_max_clients = 1000;				/* Max concurrent connection */
int tunable_max_conn_per_ip = 5;			/* Max connection per ip */
int tunable_ctrl_conn_idle_time = 30;		/* Idle time while connected */
int tunable_data_conn_idle_time = 30;		/* Idle time while trans data */
int tunable_accept_timeout = 30;			/* Accept timeout */
int tunable_connect_timeout = 30;			/* Connection timeout */

int tunable_listen_port = 21;				/* Listen port */
const char *tunable_listen_addr;		/* Listen addr */
const char *tunable_dir_root;			/* directory root */