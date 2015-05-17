#ifndef _ZSIGNAL_H_
#define _ZSIGNAL_H_

#include <signal.h>

struct session;
extern struct session *ps;
typedef void (*sighandler_t)(int);

void set_signal_child();
sighandler_t zsignal(int signum, sighandler_t handler);
void set_signal_alarm_ctrl();
void set_signal_urg();

void enable_signal_urg(int fd);
void enable_msg_oobinline(int fd);

void start_signal_alarm_ctrl();

#endif