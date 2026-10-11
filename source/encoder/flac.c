#include <stdint.h>
#include <stdio.h>

#include <FLAC/metadata.h>
#include <FLAC/stream_encoder.h>

#include "common.h"
#include "flac.h"

static int flac_close(ENCODER *enc)
{
    FLAC__StreamEncoder *encoder = enc->userdata;

    if (!FLAC__stream_encoder_finish(encoder))
    {
        fprintf(stderr, "error: unable to finish encoding\n");
        return 1;
    }

    FLAC__stream_encoder_delete(encoder);
    return 0;
}

static int flac_write(ENCODER *enc, int16_t *data, uint32_t size)
{
    FLAC__StreamEncoder *encoder = enc->userdata;
    int32_t *temp;

    temp = malloc(size * sizeof(int32_t) * 2);
    if (temp == NULL)
    {
        fprintf(stderr, "error: unable to allocate temp data\n");
        return 1;
    }

    for (uint32_t i = 0; i < size * 2; i++)
    {
        temp[i] = data[i];
    }

    if (!FLAC__stream_encoder_process_interleaved(encoder, temp, size))
    {
        FLAC__stream_encoder_get_state(encoder);
        fprintf(stderr, "error: unable to write data\n");
        free(temp);
        return 1;
    }

    free(temp);
    return 0;
}

int flac_open(ENCODER *enc, const char *filename)
{
    FLAC__StreamEncoder *encoder;
    FLAC__StreamEncoderInitStatus status;
    const char *reason;

    encoder = FLAC__stream_encoder_new();
    if (encoder == NULL)
    {
        fprintf(stderr, "error: unable to create encoder\n");
        return 1;
    }

    FLAC__stream_encoder_set_verify(encoder, true);
    FLAC__stream_encoder_set_compression_level(encoder, 5);
    FLAC__stream_encoder_set_channels(encoder, 2);
    FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
    FLAC__stream_encoder_set_sample_rate(encoder, enc->sample_rate);

    status = FLAC__stream_encoder_init_file(encoder, filename, NULL, NULL);
    if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
    {
        reason = FLAC__StreamEncoderInitStatusString[status];
        fprintf(stderr, "error: unable to init encoder - %s\n", reason);
        FLAC__stream_encoder_delete(encoder);
        return 1;
    }

    enc->close = flac_close;
    enc->write = flac_write;
    enc->userdata = encoder;
    return 0;
}
