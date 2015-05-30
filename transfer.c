#include "transfer.h"
#include "session.h"
#include "ftpcmdio.h"
#include "ftpcode.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int _get_transfer_fd(session_t *sess);

void download(session_t *sess)
{
	sess->is_transmitting = 1;
//	long long offset = sess->re_pos;
	sess->re_pos = 0;

	if (!_get_transfer_fd(sess)) {
		ftp_cmdio_write(sess->ctrl_fd, FTP_LOCALE, "Can't open data fd.");
		sess->is_transmitting = 0;
		return;
	}

	int fd = open(sess->ftp_cmd_arg, O_RDONLY);
	if (fd == -1) {
		ftp_cmdio_write(sess->ctrl_fd, FTP_FILEE, "Can't open file.");
		sess->is_transmitting = 0;
		return;
	}
}

void upload(session_t *sess, int flag)
{}

static int _get_transfer_fd(session_t *sess)
{
	return 1;
}