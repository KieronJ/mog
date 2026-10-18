#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sound.h"
#include "spu/spu.h"

/*---------------------------------------------------------------------------*/

static u_char blank_sound_data[32] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/*---------------------------------------------------------------------------*/

static void init_data(void)
{
    memset(&sound_player, 0, sizeof(sound_player));
    sound_player.mode = S_MODE_STEREO;

    sound_control_flags = 0;
    sound_mute_mask = 0;
    sound_reverb_pan = S_PAN_CENTER;
    sound_volume = S_VOL_MAX << 16;
    sound_volume_rate = 0;
    sound_tempo = 0;
    sound_tempo_rate = 0;
    sound_pitch = 0;
    sound_pitch_rate = 0;

    spu_set_master_volume(S_MASTER_VOL, S_MASTER_VOL);

    for (int i = 0; i < S_NUM_VOICES; i++)
    {
        sound_tracks[i].attr.voice_id = i;
        sound_tracks[i].effect = 0;
        spu_set_voice_volume(i, 0, 0);
    }
}

/*---------------------------------------------------------------------------*/

int sound_start(int debug)
{
    sound_debug = debug;

    spu_init();
    spu_write(S_DEFAULT_SOUND_ADDR, blank_sound_data, sizeof(blank_sound_data));

    if (sound_load_instr()) return 1;

    init_data();
    return 0;
}

void sound_stop(void)
{
    spu_set_key_off(S_VOICE_MASK);
    spu_quit();
}

void *sound_load_file_data(const char *filename)
{
    FILE *fp;
    long size;
    void *data;

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("error: could not open %s!\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data = malloc(size);
    if (data == NULL)
    {
        printf("error: could not allocate file buffer!\n");
        fclose(fp);
        return NULL;
    }

    if (fread(data, size, 1, fp) == 0)
    {
        printf("error: could not read file!\n");
        free(data);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return data;
}

int sound_load_instr(void)
{
    S_WAVE *wave;
    void *instr;
    size_t load_size;

    wave = sound_load_file_data("data/INSTR.ALL");
    if (wave == NULL)
    {
        printf("unable to load INSTR.ALL\n");
        goto error;
    }

    instr = sound_load_file_data("data/INSTR.DAT");
    if (instr == NULL)
    {
        printf("unable to load INSTR.DAT\n");
        goto error;
    }

    spu_write(wave->addr, wave->data, wave->size);

    load_size = sizeof(sound_instruments);
    memcpy(sound_instruments, instr, load_size);

    free(wave);
    free(instr);
    return 0;

error:
    free(wave);
    free(instr);
    return 1;
}

int sound_load_instr2(void)
{
    S_WAVE *wave;
    void *instr;
    size_t load_size;

    wave = sound_load_file_data("data/INSTR2.ALL");
    if (wave == NULL)
    {
        printf("unable to load INSTR2.ALL\n");
        goto error;
    }

    instr = sound_load_file_data("data/INSTR2.DAT");
    if (instr == NULL)
    {
        printf("unable to load INSTR2.DAT\n");
        goto error;
    }

    spu_write(wave->addr, wave->data, wave->size);

    load_size = sizeof(sound_instruments);
    load_size -= sizeof(sound_instruments[0]) * S_INSTR2_OFFSET;
    memcpy(sound_instruments + S_INSTR2_OFFSET, instr, load_size);

    free(wave);
    free(instr);
    return 0;

error:
    free(wave);
    free(instr);
    return 1;
}
