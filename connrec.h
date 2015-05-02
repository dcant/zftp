#ifndef _CONNREC_H_
#define _CONNREC_H_

struct session;

void init_recorder();

void signal_child_handler(int signum);

void main_loop(struct session *sess, int fd, int timeout);

#endif