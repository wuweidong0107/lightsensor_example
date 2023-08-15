
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/input.h>

#include "inputdev.h"

struct input_handle {
    int fd;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
};

static int _input_error(input_t* in, int code, int c_errno, const char* fmt, ...)
{
    va_list ap;
    in->error.c_errno = c_errno;
    va_start(ap, fmt);
    vsnprintf(in->error.errmsg, sizeof(in->error.errmsg), fmt, ap);
    va_end(ap);

    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(in->error.errmsg + strlen(in->error.errmsg), 
            sizeof(in->error.errmsg) - strlen(in->error.errmsg),
            ": %s [errno %d]", buf, c_errno);
    }
    return code;
}

int input_errno(input_t* in)
{
    return in->error.c_errno;
}

const char* input_errmsg(input_t* in)
{
    return in->error.errmsg;
}

input_t* input_new(void)
{
    input_t* in = calloc(1, sizeof(input_t));
    if (in == NULL)
        return NULL;
    
    return in;
}

void input_free(input_t* in)
{
    free(in);
}

int input_open(input_t* in, const char* path)
{
    if ((in->fd = open(path, O_RDWR)) < 0)
        return _input_error(in, INPUT_ERROR_OPEN, errno, "Opening input device \"%s\"", path);

    return 0;
}

int input_close(input_t* in)
{
    if (in->fd < 0)
        return 0;
    if (close(in->fd) < 0)
        return _input_error(in, INPUT_ERROR_CLOSE, errno, "Closing input device");
    in->fd = -1;

    return 0;
}

int input_read(input_t *in,  struct input_event ev[], size_t ev_len, int timeout_ms)
{
    ssize_t ret;

    struct timeval tv_timeout;
    tv_timeout.tv_sec = timeout_ms / 1000;
    tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;

    size_t ev_read = 0;
    size_t len = sizeof(struct input_event);

    while (ev_read < ev_len) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(in->fd, &rfds);

        if ((ret = select(in->fd+1, &rfds, NULL, NULL, (timeout_ms < 0) ? NULL : &tv_timeout)) < 0)
            return _input_error(in, INPUT_ERROR_IO, errno, "select() on input device");

        /* Timeout */
        if (ret == 0)
            break;

        if ((ret = read(in->fd, &ev[ev_read], len) < 0))
            return _input_error(in, INPUT_ERROR_IO, errno, "Reading input device");

        /* Empty read */
        if (ret == 0 && len != 0)
            return _input_error(in, INPUT_ERROR_IO, 0, "Reading input device: unexpected empty read");

        ev_read += ret;
    }
    return ev_read;
}