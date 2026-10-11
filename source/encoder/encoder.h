#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>

typedef struct _ENCODER {
    int (*close)(struct _ENCODER *);
    int (*write)(struct _ENCODER *, int16_t *, uint32_t);
    void *userdata;
    int sample_rate;
} ENCODER;

#endif /* _ENCODER_H_ */
