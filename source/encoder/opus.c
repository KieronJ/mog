#include <stdint.h>
#include <stdio.h>

#include <opusenc.h>

#include "common.h"
#include "opus.h"

static int opus_close(ENCODER *enc)
{
    OggOpusEnc *encoder = enc->userdata;

    if (ope_encoder_drain(encoder))
    {
        fprintf(stderr, "error: unable to finish encoding\n");
        return 1;
    }

    ope_encoder_destroy(encoder);
    return 0;
}

static int opus_write(ENCODER *enc, int16_t *data, uint32_t size)
{
    OggOpusEnc *encoder = enc->userdata;

    if (ope_encoder_write(encoder, data, size))
    {
        fprintf(stderr, "error: unable to write data\n");
        return 1;
    }

    return 0;
}

int opus_open(ENCODER *enc, const char *filename)
{
    OggOpusComments *comments;
    OggOpusEnc *encoder;
    int err;
    const char *reason;

    comments = ope_comments_create();
    if (comments == NULL)
    {
        fprintf(stderr, "error: unable to create encoder metadata\n");
        return 1;
    }

    encoder = ope_encoder_create_file(filename, comments, enc->sample_rate, 2, 0, &err);
    if (encoder == NULL)
    {
        reason = ope_strerror(err);
        fprintf(stderr, "error: unable to create encoder - %s\n", reason);
        ope_comments_destroy(comments);
        return 1;
    }

    enc->close = opus_close;
    enc->write = opus_write;
    enc->userdata = encoder;

    ope_comments_destroy(comments);
    return 0;
}
