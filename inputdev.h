#ifndef _INPUTDEV_H
#define _INPUTDEV_H

#ifdef __cplusplus
extern "C" {
#endif

enum input_error_code {
    INPUT_ERROR_ARG = -1,
    INPUT_ERROR_OPEN = -2,
    INPUT_ERROR_QUERY = -3,
    INPUT_ERROR_CONFIGURE = -4,
    INPUT_ERROR_IO = -5,
    INPUT_ERROR_CLOSE = -6,
};

typedef struct input_handle input_t;

input_t* input_new(void);
void input_free(input_t* ls);
int input_open(input_t* ls, const char* path);
int input_close(input_t* ls);
const char* input_errmsg(input_t* ls);
int input_read(input_t *in,  struct input_event ev[], size_t ev_len, int timeout_ms);
#ifdef __cplusplus
}
#endif

#endif