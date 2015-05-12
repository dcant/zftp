#include "utils.h"
#include "err.h"
#include "tunables.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/capability.h>
#include <sys/prctl.h>

void check_privilege()
{
	if (geteuid()) {
		printf("Please run as root!\n");
		exit(-1);
	}
}

void drop_privilege()
{
	struct passwd *pw;
	if ((pw = getpwnam("nobody")) == NULL)
		ERROR_EXIT("getpwnam");

	if (setgid(pw->pw_gid) == -1)
		ERROR_EXIT("setgid");
	if (seteuid(pw->pw_uid) == -1)
		ERROR_EXIT("seteuid");

	struct __user_cap_data_struct data;
    struct __user_cap_header_struct hdr;
    hdr.version = _LINUX_CAPABILITY_VERSION_1;
    hdr.pid = getpid();
    data.effective = (1 << CAP_NET_BIND_SERVICE);
    data.permitted = (1 << CAP_NET_BIND_SERVICE);
    data.inheritable = 0;

    if(capset(&hdr, &data) == -1)
        ERROR_EXIT("capset");
}

void set_daemon()
{
	pid_t pid;
	int i;
	pid = fork();
	if (pid < 0)
		ERROR_EXIT("fork");
	else if (pid > 0)
		exit(0);

	setsid();
	pid = fork();
	if (pid < 0)
		ERROR_EXIT("fork");
	else if (pid > 0)
		exit(0);
	chdir("/tmp");
	umask(0);
	for (i = 0; i <	NOFILE; i++)
		close(i);
}

void set_dir_root()
{
	chroot(tunable_dir_root);
	chdir("/");
}

int tcp_server(const char *addr, int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ERROR_EXIT("socket");
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	if (addr != NULL)
		saddr.sin_addr.s_addr = inet_addr(addr);
	else
		saddr.sin_addr.s_addr = INADDR_ANY;

	int on = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
		ERROR_EXIT("setsocket");
	if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
		ERROR_EXIT("bind");
	if (listen(sockfd, SOMAXCONN) == -1)
		ERROR_EXIT("listen");

	return sockfd;
}

int tcp_client(const char *addr, int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ERROR_EXIT("socket");

	struct sockaddr_in caddr;
	memset(&caddr, 0, sizeof(caddr));
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	caddr.sin_addr.s_addr = inet_addr(addr);
	if (bind(sockfd, (struct sockaddr *)&caddr, sizeof(caddr)) == -1)
		ERROR_EXIT("bind");

	return sockfd;
}

int connect_timeout(int sockfd, struct sockaddr_in *addr, int secs)
{
	int ret = 0;
	socklen_t len = sizeof(struct sockaddr_in);
	struct timeval timeout = {secs, 0};
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))
		== -1)
		ERROR_EXIT("setsockopt");
	if (connect(sockfd, (struct sockaddr *)addr, len) == -1) {
		if (errno == EINPROGRESS)
			errno = ETIMEDOUT;
		ret = -1;
		return ret;
	}
	return ret;
}

int accept_timeout(int sockfd, struct sockaddr_in *addr, int secs)
{
	int ret = 0;
	socklen_t len = sizeof(struct sockaddr_in);
	struct timeval timeout = {secs, 0};
	fd_set fs;
	if (secs > 0) {
		do {
			FD_ZERO(&fs);
			FD_SET(sockfd, &fs);
			ret = select(sockfd + 1, &fs, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0) {
			errno = ETIMEDOUT;
			return -1;
		} else if (ret == -1)
			return ret;
	}

	if (addr == NULL)
		ret = accept(sockfd, NULL, NULL);
	else
		ret = accept(sockfd, (struct sockaddr *)addr, &len);

	return ret;
}

int readn(int fd, char *buf, int n)
{
	int left = n, rd = 0;
	char *p = buf;
	while (left > 0) {
		if ((rd = read(fd, p, left)) < 0) {
			if (errno == EINTR)
				rd = 0;
			else
				ERROR_EXIT("read");
		} else if (rd == 0)
			break;
		left -= rd;
		p += rd;
	}

	return n - left;
}

int writen(int fd, const char *buf, int n)
{
	int left = n, wr = 0;
	const char *p = buf;
	while (left > 0) {
		if ((wr = write(fd, p, left)) < 0) {
			if (errno == EINTR)
				wr = 0;
			else
				ERROR_EXIT("write");
		} else if (wr == 0)
			break;
		left -= wr;
		p += wr;
	}

	return n - left;
}

static int lock_file(int fd, short type)
{
	int ret;
	struct flock lockinfo;
	lockinfo.l_type = type;
	lockinfo.l_whence = SEEK_SET;
	lockinfo.l_start = 0;
	lockinfo.l_len = 0;
	lockinfo.l_pid = getpid();

	if (type == F_UNLCK)
		return fcntl(fd, F_SETLK, &lockinfo);
	else {
		do {
			ret = fcntl(fd, F_SETLKW, &lockinfo);
		} while (ret < 0 && errno == EINTR);
	}

	return ret;
}

int rlock_file(int fd)
{
	return lock_file(fd, F_RDLCK);
}

int wlock_file(int fd)
{
	return lock_file(fd, F_WRLCK);
}

int unlock_file(int fd)
{
	return lock_file(fd, F_UNLCK);
}

int send_fd(int fd, int sfd)
{
	struct msghdr msg;
	struct cmsghdr *cmptr;
	struct iovec iov[1];
	char c = 0;
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(sfd))];
	} control_union;	// use union to make sure cmsghdr and msg_control align

	iov[0].iov_base = &c;
	iov[0].iov_len = sizeof(c);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = control_union.control;
	msg.msg_controllen = sizeof(control_union.control);
	msg.msg_flags = 0;

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(sfd));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *)CMSG_DATA(cmptr)) = sfd;

	return sendmsg(fd, &msg, 0);
}

int recv_fd(int fd, int *rsd)
{
	int ret;
	struct msghdr msg;
	struct cmsghdr *cmptr;
	struct iovec iov[1];
	char c;
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(*rsd))];
	} control_union;

	iov[0].iov_base = &c;
	iov[0].iov_len = sizeof(c);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = control_union.control;
	msg.msg_controllen = sizeof(control_union.control);
	msg.msg_flags = 0;
	*((int *)CMSG_DATA(CMSG_FIRSTHDR(&msg))) = -1;

	ret = recvmsg(fd, &msg, 0);
	if (ret != 1)
		return ret;

	cmptr = CMSG_FIRSTHDR(&msg);
	if (cmptr == NULL)
		return -1;
	*rsd = *((int *)CMSG_DATA(cmptr));

	return ret;
}

int recv_peek(int fd, void *buf, int n)
{
	int rd = 0;
	while (1) {
		if((rd = recv(fd, buf, n, MSG_PEEK)) < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		break;
	}
	return rd;
}