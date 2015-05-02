#ifndef _PRELOGIN_H_
#define _PRELOGIN_H_

struct session;

void init_session(struct session *sess);
void start_session(struct session *sess);

#endif