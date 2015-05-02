#ifndef _FTPCMDIO_H_
#define _FTPCMDIO_H_

void ftp_cmdio_write(int fd, int status, const char *msg);
void ftp_cmdio_write_m(int fd, int status, const char *msg);
void ftp_cmdio_write_exit(int fd, int status, const char *msg);
int ftp_cmdio_get_cmd_args(int fd, void *buf, int len);

#endif