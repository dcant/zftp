#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include <netinet/in.h>

void check_privilege();
void set_daemon();
void set_dir_root();

int tcp_server(const char *addr, int port);
int tcp_client(const char *addr, int port);
int connect_timeout(int sockfd, struct sockaddr_in *addr, int secs);
int accept_timeout(int sockfd, struct sockaddr_in *addr, int secs);

int readn(int fd, char *buf, int n);
int writen(int fd, const char *buf, int n);

int rlock_file(int fd);	//read lock
int wlock_file(int fd);	//write lock
int unlock_file(int fd);

int send_fd(int fd, int sfd);
int recv_fd(int fd, int *rfd);

int recv_peek(int fd, void *buf, int n);

#endif