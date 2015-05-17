#ifndef _PRIVSOCK_H_
#define _PRIVSOCK_H_

#define PRIV_SOCK_CHECK		1	// check whether pasv mode is on
#define PRIV_SOCK_LISTEN	2	// set tcp server in pasv mode
#define PRIV_SOCK_ACCEPT	3	// accept connection in pasv mode
#define PRIV_SOCK_GET		4	// get data sock
#define PRIV_SOCK_CLOSE		5	// close pre mode

struct session;
void priv_sock_init(struct session *sess);

void priv_sock_send_cmd(int fd, char cmd);
char priv_sock_recv_cmd(int fd);

void priv_sock_send_res(int fd, int res);
int priv_sock_recv_res(int fd);

void priv_sock_send_int(int fd, int num);
int priv_sock_recv_int(int fd);

void priv_sock_send_str(int fd, const char *str, int len);
void priv_sock_recv_str(int fd, char *buf, int len);

#endif