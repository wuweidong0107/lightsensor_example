#ifndef _LIGHTSENSOR_H
#define _LIGHTSENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/input.h>
enum lightsensor_error_code {
    LIGHTSENSOR_ERROR_ARG = -1,
    LIGHTSENSOR_ERROR_OPEN = -2,
    LIGHTSENSOR_ERROR_QUERY = -3,
    LIGHTSENSOR_ERROR_CONFIGURE = -4,
    LIGHTSENSOR_ERROR_IO = -5,
    LIGHTSENSOR_ERROR_CLOSE = -6,
};

typedef struct lightsensor_handle lightsensor_t;

lightsensor_t* lightsensor_new(void);
void lightsensor_free(lightsensor_t* ls);
int lightsensor_open(lightsensor_t* ls, const char* path, const char* input_path);
int lightsensor_close(lightsensor_t* ls);
const char* lightsensor_errmsg(lightsensor_t* ls);
int lightsensor_get_enable(lightsensor_t *ls,  unsigned int* value);
int lightsensor_set_enable(lightsensor_t* ls, unsigned int value);
int lightsensor_read(lightsensor_t* ls,  struct input_event ev[], size_t ev_len, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif