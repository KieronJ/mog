#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sound.h"

/*---------------------------------------------------------------------------*/

int sound_play_music(const char *filename)
{
    S_SEQUENCE *seq;

    seq = sound_load_file_data(filename);
    if (seq == NULL)
    {
        printf("unable to load sequence data\n");
        goto error;
    }

    if (strncmp(seq->magic, "AKAO", S_NUM_MAGIC) != 0)
    {
        printf("invalid sequence data magic!\n");
        goto error;
    }

    if (sound_debug)
    {
        printf("sequence id:   %u\n", seq->id);
        printf("sequence time: 19%02x/%02x/%02x\n", seq->date[0], seq->date[1], seq->date[2]);
        printf("sequence date: %02x:%02x:%02x\n", seq->time[0], seq->time[1], seq->time[2]);
    }

    if (sound_player.music_id != seq->id)
    {
        /* load bank2 data for One-Winged Angel */
        if (seq->id == S_SEQ_ID_LB2 && sound_load_instr2())
        {
            goto error;
        }

        sound_set_reverb_mode(seq->reverb_mode);
        sound_copy_music(seq->data, seq->size);
        sound_stop_player();
        sound_start_player();
        sound_player.music_id = seq->id;
    }

    free(seq);
    return 0;

error:
    free(seq);
    return 1;
}

void sound_set_volume(int volume)
{
    sound_volume = (volume & S_VOL_MAX) << 16;
    sound_volume_rate = 0;
    sound_update_volume();
}

void sound_move_volume(int time, int target)
{
    if (time == 0) time = 1;
    target = (target & S_VOL_MAX) << 16;
    sound_volume_rate = time;
    sound_volume_step = (target - sound_volume) / time;
    sound_update_volume();
}

void sound_move_volume_range(int time, int volume, int target)
{
    if (time == 0) time = 1;
    volume = (volume & S_VOL_MAX) << 16;
    target = (target & S_VOL_MAX) << 16;
    sound_volume = volume;
    sound_volume_rate = time;
    sound_volume_step = (target - volume) / time;
    sound_update_volume();
}

void sound_set_tempo(int tempo)
{
    sound_tempo = tempo << 16;
    sound_tempo_rate = 0;
}

void sound_move_tempo(int time, int target)
{
    if (time == 0) time = 1;
    target <<= 16;
    sound_tempo_rate = time;
    sound_tempo_step = (target - sound_tempo) / time;
}

void sound_move_tempo_range(int time, int tempo, int target)
{
    if (time == 0) time = 1;
    tempo <<= 16;
    target <<= 16;
    sound_tempo = tempo;
    sound_tempo_rate = time;
    sound_tempo_step = (target - tempo) / time;
}

void sound_set_pitch(int pitch)
{
    sound_pitch = pitch << 16;
    sound_pitch_rate = 0;
}

void sound_move_pitch(int time, int target)
{
    if (time == 0) time = 1;
    target <<= 16;
    sound_pitch_rate = time;
    sound_pitch_step = (target - sound_pitch) / time;
}

void sound_move_pitch_range(int time, int pitch, int target)
{
    if (time == 0) time = 1;
    pitch <<= 16;
    target <<= 16;
    sound_pitch = pitch;
    sound_pitch_rate = time;
    sound_pitch_step = (target - pitch) / time;
}

void sound_stop_music(void)
{
    sound_stop_player();
}

void sound_set_mono(void)
{
    sound_player.mode = S_MODE_MONO;
    sound_update_volume();
}

void sound_set_stereo(void)
{
    sound_player.mode = S_MODE_STEREO;
    sound_update_volume();
}

void sound_set_pair(void)
{
    sound_player.mode = S_MODE_PAIR;
    sound_update_volume();
}

void sound_set_mute(int mute)
{
    sound_mute_mask = !!mute;
    for (int i = 0; i < S_NUM_VOICES; i++)
    {
        sound_tracks[i].attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
    }
}

void sound_set_condition(u_short cond)
{
    sound_player.cond = cond;
}

void sound_pause(void)
{
    S_ATTR attr;

    if (sound_player.active_mask)
    {
        attr.vol_l = 0;
        attr.vol_r = 0;
        attr.sr = S_VOL_MAX;

        for (int i = 0; i < S_NUM_VOICES; i++)
        {
            if ((sound_player.active_mask | sound_player.overlay_mask | sound_player.alt_mask) & (1 << i))
            {
                attr.mask = (S_UPDATE_VOLL | S_UPDATE_VOLR | S_UPDATE_SMODE | S_UPDATE_SR);
                sound_flush(i, &attr);
            }
        }
    }

    sound_player.active_mask = 0;
    sound_control_flags |= S_CONTROL_PAUSE_UPDATE;
}

void sound_set_reverb_pan(int pan)
{
    sound_reverb_pan = pan & S_PAN_MAX;
    sound_player.update |= S_UPDATE_REVERB;
}

void sound_set_reverb_volume(int volume)
{
    sound_reverb_volume = volume;

    if (sound_reverb_volume)
    {
        sound_control_flags |= S_CONTROL_REVERB_ENABLE;
    }
    else
    {
        sound_control_flags &= ~S_CONTROL_REVERB_ENABLE;
    }

    sound_flush_reverb();
    sound_player.update |= S_UPDATE_REVERB;
}
