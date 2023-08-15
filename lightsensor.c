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

#include "lightsensor.h"
#include "inputdev.h"

#define LIGHTSENSOR_IOCTL_MAGIC 'l'
#define LIGHTSENSOR_IOCTL_GET_ENABLED _IOR(LIGHTSENSOR_IOCTL_MAGIC, 1, int *)
#define LIGHTSENSOR_IOCTL_ENABLE _IOW(LIGHTSENSOR_IOCTL_MAGIC, 2, int *)

struct lightsensor_handle {
    int char_fd;
    input_t *in;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
};

static int _lightsensor_error(lightsensor_t* ls, int code, int c_errno, const char* fmt, ...)
{
    va_list ap;
    ls->error.c_errno = c_errno;
    va_start(ap, fmt);
    vsnprintf(ls->error.errmsg, sizeof(ls->error.errmsg), fmt, ap);
    va_end(ap);

    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(ls->error.errmsg + strlen(ls->error.errmsg), 
            sizeof(ls->error.errmsg) - strlen(ls->error.errmsg),
            ": %s [errno %d]", buf, c_errno);
    }
    return code;
}

int lightsensor_errno(lightsensor_t* ls)
{
    return ls->error.c_errno;
}

const char* lightsensor_errmsg(lightsensor_t* ls)
{
    return ls->error.errmsg;
}

lightsensor_t* lightsensor_new(void)
{
    lightsensor_t* ls = calloc(1, sizeof(lightsensor_t));
    if (ls == NULL)
        return NULL;

    ls->in = input_new();
    if (!ls->in)
        goto fail;

    return ls;
fail:
    lightsensor_free(ls);
    return NULL;
}

void lightsensor_free(lightsensor_t* ls)
{
    if (ls->in)
        free(ls->in);
    if (ls)
        free(ls);
}

int lightsensor_open(lightsensor_t* ls, const char* path, const char* input_path)
{
    if ((ls->char_fd = open(path, O_RDWR)) < 0)
        return _lightsensor_error(ls, LIGHTSENSOR_ERROR_OPEN, errno, "Opening lightsensor chardev\"%s\"", path);

    if (input_open(ls->in, input_path) !=0) {
        return _lightsensor_error(ls, LIGHTSENSOR_ERROR_OPEN, 0, "Openning lightsensor inputdev %s", input_path);
    }

    return 0;
}

int lightsensor_close(lightsensor_t* ls)
{
    if (ls->char_fd > 0) {
        if (close(ls->char_fd) < 0)
            return _lightsensor_error(ls, LIGHTSENSOR_ERROR_CLOSE, errno, "Closing lightsensor/misc");
        ls->char_fd = -1;
    }
    if (ls->in) {
        input_close(ls->in);
    }
    return 0;
}

int lightsensor_get_enable(lightsensor_t *ls,  unsigned int* value)
{
    int flags = 0;
    if (ioctl(ls->char_fd, LIGHTSENSOR_IOCTL_GET_ENABLED, &flags) < 0) {
        return _lightsensor_error(ls, LIGHTSENSOR_ERROR_QUERY, errno, "Getting lightsensor attributes");
    }

    *value = flags;
    return 0;
}

int lightsensor_set_enable(lightsensor_t* ls, unsigned int value)
{
    if (ioctl(ls->char_fd, LIGHTSENSOR_IOCTL_ENABLE, &value) < 0) {
        return _lightsensor_error(ls, LIGHTSENSOR_ERROR_CONFIGURE, errno, "Setting lightsensor attributes");
    }

    return 0;
}

int lightsensor_read(lightsensor_t* ls,  struct input_event ev[], size_t ev_len, int timeout_ms)
{
    return input_read(ls->in, ev, ev_len, timeout_ms);
}