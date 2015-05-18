#ifndef _ZSIGNAL_H_
#define _ZSIGNAL_H_

#include <signal.h>

struct session;
extern struct session *ps;
typedef void (*sighandler_t)(int);

sighandler_t zsignal(int signum, sighandler_t handler);
void set_signal_child();
void set_signal_alarm_ctrl();
void set_signal_urg();
void set_signal_alarm_data();

void enable_signal_urg(int fd);
void enable_msg_oobinline(int fd);

void start_signal_alarm_ctrl();
void start_signal_alarm_data();

#endif