#ifndef _PROCESS_H_
#define _PROCESS_H_

struct session;

void set_child_context(struct session *sess);
void set_parent_context(struct session *sess);

void handle_ftpcmd(struct session *sess);
void handle_childcmd(struct session *sess);

#endif