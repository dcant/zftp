#ifndef _TUNABLES_H_
#define _TUNABLES_H_

extern int tunable_anonymous_enable;		/* Allow anonymous login */
extern int tunable_local_enable;			/* Allow local login */

extern int tunable_pasv_enable;				/* PASV mode */
extern int tunable_port_enable;				/* PORT mode */
extern int tunable_anonymous_upload_enable;	/* Allow anonymoust upload */

extern int tunable_ascii_mode_enable;		/* Enable ascii mode */
extern int tunable_bin_mode_enable;			/* Enable bin mode */

extern int tunable_max_clients;				/* Max concurrent connection */
extern int tunable_max_conn_per_ip;			/* Max connection per ip */
extern int tunable_ctrl_conn_idle_time;		/* Idle time while connected */
extern int tunable_data_conn_idle_time;		/* Idle time while trans data */
extern int tunable_accept_timeout;			/* Accept timeout */
extern int tunable_connect_timeout;			/* Connection timeout */

extern int tunable_listen_port;				/* Listen port */
extern const char *tunable_listen_addr;		/* Listen addr */
extern const char *tunable_dir_root;		/* Directory root */


#endif