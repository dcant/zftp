#ifndef _SESSION_H_
#define _SESSION_H_

#ifndef MAXCMD
#define MAXCMD 1024
#endif

struct sockaddr_in;

typedef struct session {
	int tmode;	// 0 -> bin  1 -> ascii
	int userid;

	int ctrl_fd;
	int pasv_listen_fd;
	int data_fd;

	// command channel
	int parent_fd;	//
	int child_fd;	// used to 

	int is_login;
	int is_transmitting;
	int is_abor_recved;

	long long re_pos;	// restart position

	int clients_num;
	int ip_client_num;

	struct sockaddr_in *cliaddr;

	char ftp_cmd_line[MAXCMD];
	char ftp_cmd_op[MAXCMD];
	char ftp_cmd_arg[MAXCMD];
} session_t;

//extern session_t *ptrsess;

#endif