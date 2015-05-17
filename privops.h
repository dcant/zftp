#ifndef _PRIVOPS_H_
#define _PRIVOPS_H_

struct session;

void priv_op_check(struct session *sess);
void priv_op_close(struct session *sess);
void priv_op_listen(struct session *sess);
void priv_op_accept(struct session *sess);
void priv_op_get(struct session *sess);

#endif