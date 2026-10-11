#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder/flac.h"
#include "encoder/opus.h"
#include "sound/sound.h"

int main(int argc, char **argv)
{
    ENCODER enc;

    const char *sequence = NULL;
    const char *output = NULL;
    const char *encoder = NULL;

    int debug = 0;
    int time = 120;

    if (argc < 2)
    {
        printf("usage: mog [-d] [-t time] -o output -e encoder seq_file\n");
        return 0;
    }

    enc.sample_rate = S_SAMPLE_RATE;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug = 1;
            continue;
        }

        if (strcmp(argv[i], "-t") == 0)
        {
            time = atoi(argv[i + 1]);
            i++; // skip value
            continue;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            output = argv[i + 1];
            i++; // skip value
            continue;
        }

        if (strcmp(argv[i], "-e") == 0)
        {
            encoder = argv[i + 1];
            i++; // skip value
            continue;
        }

        if (argv[i][0] == '-')
        {
            printf("unrecognised option: %s\n", argv[i]);
            continue;
        }

        if (!sequence)
        {
            sequence = argv[i];
            continue;
        }

        printf("unknown argument: %s\n", argv[i]);
    }

    if (!output)
    {
        printf("missing required argument output\n");
        return 1;
    }

    if (!encoder)
    {
        printf("missing required argument encoder\n");
        return 1;
    }

    if (!sequence)
    {
        printf("missing required argument sequence\n");
        return 1;
    }

    if (sound_start(debug))
    {
        printf("unable to start sequencer\n");
        return 1;
    }

    if (sound_play_music(sequence))
    {
        printf("unable to load sequence file\n");
        return 1;
    }

    if (strcmp(encoder, "flac") == 0)
    {
        flac_open(&enc, output);
    }
    else if (strcmp(encoder, "opus") == 0)
    {
        opus_open(&enc, output);
    }
    else
    {
        printf("unknown encoder %s\n", encoder);
        return 1;
    }

    for (int i = 0; i < time * S_SEQ_HZ; i++)
    {
        void *buffer;
        size_t size;

        if (!sound_main(&buffer, &size)) break;
        enc.write(&enc, buffer, size);
    }

    enc.close(&enc);
    printf("output written to %s\n", output);

    sound_stop();
    return 0;
}

