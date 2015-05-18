#ifndef _TRANSFER_H_
#define _TRANSFER_H_

struct session;
void download(struct session *sess);
void upload(struct session *sess, int flag);

#endif