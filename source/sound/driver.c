#include <assert.h>
#include <string.h>
#include "sound.h"
#include "spu/spu.h"

/*---------------------------------------------------------------------------*/

static u_char empty_sequence[] = {0xA0};

static short pan_l[256] =
{
    0x7F80, 0x7E80, 0x7D80, 0x7C80, 0x7B80, 0x7A80, 0x7980, 0x7880,
    0x7780, 0x7680, 0x7580, 0x7480, 0x7380, 0x7280, 0x7180, 0x7080,
    0x6F80, 0x6E80, 0x6D80, 0x6C80, 0x6B80, 0x6A80, 0x6980, 0x6880,
    0x6780, 0x6680, 0x6580, 0x6480, 0x6380, 0x6280, 0x6180, 0x6080,
    0x5F80, 0x5E80, 0x5D80, 0x5C80, 0x5B80, 0x5A80, 0x5980, 0x5880,
    0x5780, 0x5680, 0x5580, 0x5480, 0x5380, 0x5280, 0x5180, 0x5080,
    0x4F80, 0x4E80, 0x4D80, 0x4C80, 0x4B80, 0x4A80, 0x4980, 0x4880,
    0x4780, 0x4680, 0x4580, 0x4480, 0x4380, 0x4280, 0x4180, 0x4080,
    0x3F80, 0x3E80, 0x3D80, 0x3C80, 0x3B80, 0x3A80, 0x3980, 0x3880,
    0x3780, 0x3680, 0x3580, 0x3480, 0x3380, 0x3280, 0x3180, 0x3080,
    0x2F80, 0x2E80, 0x2D80, 0x2C80, 0x2B80, 0x2A80, 0x2980, 0x2880,
    0x2780, 0x2680, 0x2580, 0x2480, 0x2380, 0x2280, 0x2180, 0x2080,
    0x1F80, 0x1E80, 0x1D80, 0x1C80, 0x1B80, 0x1A80, 0x1980, 0x1880,
    0x1780, 0x1680, 0x1580, 0x1480, 0x1380, 0x1280, 0x1180, 0x1080,
    0x0F80, 0x0E80, 0x0D80, 0x0C80, 0x0B80, 0x0A80, 0x0980, 0x0880,
    0x0780, 0x0680, 0x0580, 0x0480, 0x0380, 0x0280, 0x0180, 0x0080,
    0x0000, 0xFFFF, 0xFFFC, 0xFFF7, 0xFFF0, 0xFFE7, 0xFFDC, 0xFFCF,
    0xFFC0, 0xFFAF, 0xFF9C, 0xFF87, 0xFF70, 0xFF57, 0xFF3C, 0xFF1F,
    0xFF00, 0xFEDF, 0xFEBC, 0xFE97, 0xFE70, 0xFE47, 0xFE1C, 0xFDEF,
    0xFDC0, 0xFD8F, 0xFD5C, 0xFD27, 0xFCF0, 0xFCB7, 0xFC7C, 0xFC3F,
    0xFC00, 0xFBBF, 0xFB7C, 0xFB37, 0xFAF0, 0xFAA7, 0xFA5C, 0xFA0F,
    0xF9C0, 0xF96F, 0xF91C, 0xF8C7, 0xF870, 0xF817, 0xF7BC, 0xF75F,
    0xF700, 0xF69F, 0xF63C, 0xF5D7, 0xF570, 0xF507, 0xF49C, 0xF42F,
    0xF3C0, 0xF34F, 0xF2DC, 0xF267, 0xF1F0, 0xF177, 0xF0FC, 0xF07F,
    0x1000, 0x1081, 0x1104, 0x1189, 0x1210, 0x1299, 0x1324, 0x13B1,
    0x1440, 0x14D1, 0x1564, 0x15F9, 0x1690, 0x1729, 0x17C4, 0x1861,
    0x1900, 0x19A1, 0x1A44, 0x1AE9, 0x1B90, 0x1C39, 0x1CE4, 0x1D91,
    0x1E40, 0x1EF1, 0x1FA4, 0x2059, 0x2110, 0x21C9, 0x2284, 0x2341,
    0x2400, 0x24C1, 0x2584, 0x2649, 0x2710, 0x27D9, 0x28A4, 0x2971,
    0x2A40, 0x2B11, 0x2BE4, 0x2CB9, 0x2D90, 0x2E69, 0x2F44, 0x3021,
    0x3100, 0x31E1, 0x32C4, 0x33A9, 0x3490, 0x3579, 0x3664, 0x3751,
    0x3840, 0x3931, 0x3A24, 0x3B19, 0x3C10, 0x3D09, 0x3E04, 0x3F01
};

static short pan_r[256] =
{
    0x0080, 0x0180, 0x0280, 0x0380, 0x0480, 0x0580, 0x0680, 0x0780,
    0x0880, 0x0980, 0x0A80, 0x0B80, 0x0C80, 0x0D80, 0x0E80, 0x0F80,
    0x1080, 0x1180, 0x1280, 0x1380, 0x1480, 0x1580, 0x1680, 0x1780,
    0x1880, 0x1980, 0x1A80, 0x1B80, 0x1C80, 0x1D80, 0x1E80, 0x1F80,
    0x2080, 0x2180, 0x2280, 0x2380, 0x2480, 0x2580, 0x2680, 0x2780,
    0x2880, 0x2980, 0x2A80, 0x2B80, 0x2C80, 0x2D80, 0x2E80, 0x2F80,
    0x3080, 0x3180, 0x3280, 0x3380, 0x3480, 0x3580, 0x3680, 0x3780,
    0x3880, 0x3980, 0x3A80, 0x3B80, 0x3C80, 0x3D80, 0x3E80, 0x3F80,
    0x4080, 0x4180, 0x4280, 0x4380, 0x4480, 0x4580, 0x4680, 0x4780,
    0x4880, 0x4980, 0x4A80, 0x4B80, 0x4C80, 0x4D80, 0x4E80, 0x4F80,
    0x5080, 0x5180, 0x5280, 0x5380, 0x5480, 0x5580, 0x5680, 0x5780,
    0x5880, 0x5980, 0x5A80, 0x5B80, 0x5C80, 0x5D80, 0x5E80, 0x5F80,
    0x6080, 0x6180, 0x6280, 0x6380, 0x6480, 0x6580, 0x6680, 0x6780,
    0x6880, 0x6980, 0x6A80, 0x6B80, 0x6C80, 0x6D80, 0x6E80, 0x6F80,
    0x7080, 0x7180, 0x7280, 0x7380, 0x7480, 0x7580, 0x7680, 0x7780,
    0x7880, 0x7980, 0x7A80, 0x7B80, 0x7C80, 0x7D80, 0x7E80, 0x7F80,
    0x3F01, 0x3E04, 0x3D09, 0x3C10, 0x3B19, 0x3A24, 0x3931, 0x3840,
    0x3751, 0x3664, 0x3579, 0x3490, 0x33A9, 0x32C4, 0x31E1, 0x3100,
    0x3021, 0x2F44, 0x2E69, 0x2D90, 0x2CB9, 0x2BE4, 0x2B11, 0x2A40,
    0x2971, 0x28A4, 0x27D9, 0x2710, 0x2649, 0x2584, 0x24C1, 0x2400,
    0x2341, 0x2284, 0x21C9, 0x2110, 0x2059, 0x1FA4, 0x1EF1, 0x1E40,
    0x1D91, 0x1CE4, 0x1C39, 0x1B90, 0x1AE9, 0x1A44, 0x19A1, 0x1900,
    0x1861, 0x17C4, 0x1729, 0x1690, 0x15F9, 0x1564, 0x14D1, 0x1440,
    0x13DC, 0x1324, 0x1299, 0x1210, 0x1189, 0x1104, 0x1081, 0x1000,
    0xF07F, 0xF0FC, 0xF177, 0xF1F0, 0xF267, 0xF2DC, 0xF34F, 0xF3C0,
    0xF42F, 0xF49C, 0xF507, 0xF570, 0xF5D7, 0xF63C, 0xF69F, 0xF700,
    0xF75F, 0xF7BC, 0xF817, 0xF870, 0xF8C7, 0xF91C, 0xF843, 0xF9C0,
    0xFA0F, 0xFA5C, 0xFAA7, 0xFAF0, 0xFB37, 0xFB7C, 0xFBBF, 0xFC00,
    0xFC3F, 0xFC7C, 0xFCB7, 0xFCF0, 0xFD27, 0xFD5C, 0xFD8F, 0xFDC0,
    0xFDEF, 0xFE1C, 0xFE47, 0xFE70, 0xFE97, 0xFEBC, 0xFEDF, 0xFF00,
    0xFF1F, 0xFF3C, 0xFF57, 0xFF70, 0xFF87, 0xFF9C, 0xFFAF, 0xFFC0,
    0xFFCF, 0xFFDC, 0xFFE7, 0xFFF0, 0xFFF7, 0xFFFC, 0xFFFF, 0x0000
};

static u_char music_buffer[S_MUSIC_BUFFER_SIZE];
static short sample_buffer[S_BUF_SIZE * 2];

static int player_effect_time;
static float player_accum;

S_PLAYER sound_player;
S_TRACK sound_tracks[S_NUM_VOICES];
S_INSTRUMENT sound_instruments[S_NUM_INSTR];

int sound_debug;
u_int sound_control_flags;
u_int sound_mute_mask;

int sound_reverb_volume;
u_short sound_reverb_pan;

int sound_volume;
int sound_volume_step;
u_short sound_volume_rate;

int sound_tempo;
int sound_tempo_step;
u_short sound_tempo_rate;

int sound_pitch;
int sound_pitch_step;
u_short sound_pitch_rate;

/*---------------------------------------------------------------------------*/

static void reset_effect(S_TRACK *track)
{
    int track_vol;

    track_vol = ((track->vol >> 16) * track->master_vol) >> 7;

    if (track->update & S_UPDATE_VIBRATO)
    {
        if (track->vibrato_delay_tick == 0 && --track->vibrato_rate_tick == 0)
        {
            int vol;

            track->vibrato_rate_tick = track->vibrato_rate;
            if (track->vibrato_wave[0] == 0 && track->vibrato_wave[1] == 0)
            {
                track->vibrato_wave += track->vibrato_wave[2];
            }

            vol = (track->vibrato_base * *track->vibrato_wave++) >> 16;
            if (vol != track->vibrato_pitch)
            {
                track->vibrato_pitch = vol;
                if (track->vibrato_pitch >= 0) track->vibrato_pitch *= 2;
                track->attr.mask |= S_UPDATE_PITCH;
            }
        }
    }

    if (track->update & S_UPDATE_TREMOLO)
    {
        if (track->tremolo_delay_tick == 0 && --track->tremolo_rate_tick == 0)
        {
            int vol;

            track->tremolo_rate_tick = track->tremolo_rate;
            if (track->tremolo_wave[0] == 0 && track->tremolo_wave[1] == 0)
            {
                track->tremolo_wave += track->tremolo_wave[2];
            }

            vol = (track_vol * (track->tremolo_depth >> 8)) << 9;
            vol = ((vol >> 16) * *track->tremolo_wave++) >> 15;
            if (vol != track->tremolo_vol)
            {
                track->tremolo_vol = vol;
                track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
            }
        }
    }

    if (track->update & S_UPDATE_PAN_LFO)
    {
        if (--track->pan_lfo_tick == 0)
        {
            int vol;

            track->pan_lfo_tick = track->pan_lfo_rate;
            if (track->pan_lfo_wave[0] == 0 && track->pan_lfo_wave[1] == 0)
            {
                track->pan_lfo_wave += track->pan_lfo_wave[2];
            }

            vol = ((track->pan_lfo_depth >> 8) * *track->pan_lfo_wave++) >> 15;
            if (vol != track->pan_lfo_vol)
            {
                track->pan_lfo_vol = vol;
                track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
            }
        }
    }

    if (track->update & S_UPDATE_SCHAIN_VOL)
    {
        track_vol = (track[-1].attr.pitch * track->master_vol) >> 6;
        track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
    }

    if (track->attr.mask & (S_UPDATE_VOLL | S_UPDATE_VOLR))
    {
        int pan;

        track_vol += track->tremolo_vol;
        track_vol = (track_vol * ((sound_volume >> 16) & S_VOL_MAX)) >> 7;

        pan = ((track->pan >> 8) + track->pan_lfo_vol) & 0xFF;
        switch (sound_player.mode)
        {
        case S_MODE_MONO:
            track->attr.vol_l = (track_vol * pan_l[S_PAN_CENTER]) >> 15;
            track->attr.vol_r = (track_vol * pan_l[S_PAN_CENTER]) >> 15;
            break;
        case S_MODE_STEREO:
            track->attr.vol_l = (track_vol * pan_l[pan]) >> 15;
            track->attr.vol_r = (track_vol * pan_r[pan]) >> 15;
            break;
        case S_MODE_PAIR:
            track->attr.vol_l = (track_vol * pan_l[pan]) >> 15;
            track->attr.vol_r = (track_vol * pan_r[pan]) >> 15;

            if ((1 << track->attr.voice_id) & 0xAAAAAA)
            {
                track->attr.vol_r = ~track->attr.vol_r;
            }
            else
            {
                track->attr.vol_l = ~track->attr.vol_l;
            }
            break;
        default:
            assert(0);
        }
    }

    if (track->update & S_UPDATE_SCHAIN_PITCH)
    {
        int pitch;
        u_char mul;

        pitch = track[-1].attr.pitch + track->vibrato_pitch + (track->pitch_bend >> 16);
        mul = sound_pitch >> 16;

        if (mul != 0)
        {
            if (mul < 0x80)
            {
                pitch += (pitch * mul) >> 7;
            }
            else
            {
                pitch = (pitch * mul) >> 8;
            }
        }

        track->attr.pitch = pitch & 0x3FFF;
        track->attr.mask |= S_UPDATE_PITCH;
    }
    else if (track->attr.mask & S_UPDATE_PITCH)
    {
        int pitch;
        u_char mul;

        pitch = track->pitch + track->vibrato_pitch + (track->pitch_bend >> 16);
        mul = sound_pitch >> 16;

        if (mul != 0)
        {
            if (mul < 0x80)
            {
                pitch += (pitch * mul) >> 7;
            }
            else
            {
                pitch = (pitch * mul) >> 8;
            }
        }

        track->attr.pitch = pitch & 0x3FFF;
    }
}

static void update_overlay(S_TRACK *track, int voice)
{
    S_TRACK *overlay;

    track->attr.vol_l = (track->attr.vol_l * (S_PAN_MAX - (track->overlay_pan >> 8))) >> 8;
    track->attr.vol_r = (track->attr.vol_r * (S_PAN_MAX - (track->overlay_pan >> 8))) >> 8;

    overlay = &sound_tracks[track->overlay_num];
    overlay->attr.vol_l = (track->attr.vol_l * track->overlay_pan) >> 16;
    overlay->attr.vol_r = (track->attr.vol_r * track->overlay_pan) >> 16;
    overlay->attr.pitch = track->attr.pitch;
    overlay->attr.mask |= track->attr.mask;

    sound_flush(track->attr.voice_id, &track->attr);
    sound_flush(voice, &overlay->attr);
}

static void flush_key_on(void)
{
    u_int key_on;

    if (sound_player.update & S_UPDATE_REVERB)
    {
        int depth;
        short l, r;

        depth = sound_player.reverb_depth >> 16;
        if (sound_reverb_volume < 0x80)
        {
            depth += (depth * sound_reverb_volume) >> 7;
        }
        else
        {
            depth = (depth * sound_reverb_volume) >> 8;
        }

        if (sound_reverb_pan < 0x40)
        {
            l = depth;
            r = depth - ((depth * (~sound_reverb_pan & 0x3F)) >> 6);
        }
        else
        {
            l = depth - ((depth * (sound_reverb_pan & 0x3F)) >> 6);
            r = depth;
        }

        spu_set_reverb_depth(l, r);
        sound_player.update &= ~S_UPDATE_REVERB;
    }

    if (sound_player.update & S_UPDATE_NOISE_CLOCK)
    {
        spu_set_noise_clock(sound_player.noise_clock);
        sound_player.update &= ~S_UPDATE_NOISE_CLOCK;
    }

    key_on = 0;

    if (sound_player.active_mask)
    {
        key_on |= sound_player.on_mask;

        for (int i = 0; i < S_NUM_VOICES; i++)
        {
            if (sound_player.active_mask & sound_player.keyed_mask & (1 << i))
            {
                reset_effect(&sound_tracks[i]);

                if (sound_tracks[i].attr.mask != 0)
                {
                    if (sound_mute_mask & (1 << i))
                    {
                        sound_tracks[i].attr.vol_r = 0;
                        sound_tracks[i].attr.vol_l = 0;
                    }

                    if (sound_tracks[i].update & S_UPDATE_OVERLAY)
                    {
                        update_overlay(&sound_tracks[i], sound_tracks[i].overlay_num);
                    }
                    else if (sound_tracks[i].update & S_UPDATE_ALTERNATIVE)
                    {
                        if (sound_player.on_mask & (1 << i))
                        {
                            sound_tracks[i].update ^= S_UPDATE_FLUSH_ALT;
                            sound_tracks[i].attr.mask |= S_UPDATE_ALL;
                        }

                        if (sound_tracks[i].update & S_UPDATE_FLUSH_ALT)
                        {
                            sound_flush(sound_tracks[i].alt_num, &sound_tracks[i].attr);

                            if (key_on & (1 << i))
                            {
                                key_on |= 1 << sound_tracks[i].alt_num;
                                key_on &= ~(1 << i);
                            }
                        }
                        else
                        {
                            sound_flush(sound_tracks[i].attr.voice_id, &sound_tracks[i].attr);
                        }
                    }
                    else
                    {
                        sound_flush(sound_tracks[i].attr.voice_id, &sound_tracks[i].attr);
                    }
                }
            }
        }

        sound_player.on_mask = 0;
    }

    if (key_on)
    {
        spu_set_key_on(key_on);
    }
}

static void get_active_voices(u_int *mask, u_int active)
{
    *mask |= active;

    for (int i = 0; i < S_NUM_VOICES; i++)
    {
        if (active & (1 << i))
        {
            if (sound_tracks[i].update & S_UPDATE_OVERLAY)
            {
                *mask |= 1 << sound_tracks[i].overlay_num;
            }
            else if (sound_tracks[i].update & S_UPDATE_ALTERNATIVE)
            {
                *mask |= 1 << sound_tracks[i].alt_num;
            }
        }
    }
}

static void flush_key_off(void)
{
    u_int mask;

    mask = 0;

    if (sound_player.active_mask)
    {
        if (sound_player.off_mask)
        {
            get_active_voices(&mask, sound_player.off_mask);
        }

        sound_player.off_mask = 0;
    }

    if (mask)
    {
        spu_set_key_off(mask);
    }
}

static void update_effect(S_TRACK *track)
{
    if (track->vol_time != 0)
    {
        int prev;

        track->vol_time--;
        prev = track->vol;
        track->vol += track->vol_step;

        if ((track->vol >> 20) != (prev >> 20))
        {
            track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
        }
    }

    if (track->overlay_pan_time != 0)
    {
        int prev;

        track->overlay_pan_time--;
        prev = track->overlay_pan;
        track->overlay_pan += track->overlay_pan_step;

        if ((track->overlay_pan >> 8) != (prev >> 8))
        {
            if (track->update & S_UPDATE_OVERLAY)
            {
                track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
            }
        }
    }

    if (track->pan_time != 0)
    {
        int prev;

        track->pan_time--;
        prev = track->pan;
        track->pan += track->pan_step;

        if ((track->pan >> 8) != (prev >> 8))
        {
            track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
        }
    }

    if (track->vibrato_delay_tick != 0)
    {
        track->vibrato_delay_tick--;
    }

    if (track->tremolo_delay_tick != 0)
    {
        track->tremolo_delay_tick--;
    }

    if (track->noise_time != 0 && --track->noise_time == 0)
    {
        sound_player.noise_mask ^= 1 << track->attr.voice_id;
        sound_player.update |= S_UPDATE_NOISE_CLOCK;
        sound_flush_noise();
    }

    if (track->pitch_lfo_time != 0 && --track->pitch_lfo_time == 0)
    {
        sound_player.pitch_lfo_mask ^= 1 << track->attr.voice_id;
        sound_flush_pitch_lfo();
    }

    if (track->vibrato_depth_time != 0)
    {
        u_int depth, pitch;

        track->vibrato_depth_time--;
        track->vibrato_depth += track->vibrato_depth_step;

        depth = (track->vibrato_depth & 0x7f00) >> 8;
        pitch = track->pitch;
        if (!(track->vibrato_depth & 0x8000)) pitch = (pitch * 15) >> 8;

        track->vibrato_base = (depth * pitch) >> 7;

        if (track->vibrato_delay_tick == 0 && track->vibrato_rate_tick != 1)
        {
            short *wave;
            int vol;

            wave = track->vibrato_wave;
            if (wave[0] == 0 && wave[1] == 0)
            {
                wave += wave[2];
            }

            vol = (track->vibrato_base * wave[0]) >> 16;
            if (vol != track->vibrato_pitch)
            {
                track->vibrato_pitch = vol;
                if (track->vibrato_pitch >= 0) track->vibrato_pitch *= 2;
                track->attr.mask |= S_UPDATE_PITCH;
            }
        }
    }

    if (track->tremolo_depth_time != 0)
    {
        track->tremolo_depth_time--;
        track->tremolo_depth += track->tremolo_depth_step;

        if (track->tremolo_delay_tick == 0 && track->tremolo_rate_tick != 1)
        {
            short *wave;
            int vol;

            wave = track->tremolo_wave;
            if (wave[0] == 0 && wave[1] == 0)
            {
                wave += wave[2];
            }

            vol = ((track->vol >> 16) * track->master_vol) >> 7;
            vol = (vol * (track->tremolo_depth >> 8)) << 9;
            vol = ((vol >> 16) * wave[0]) >> 15;
            if (vol != track->tremolo_vol)
            {
                track->tremolo_vol = vol;
                track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
            }
        }
    }

    if (track->pan_lfo_time != 0)
    {
        track->pan_lfo_time--;
        track->pan_lfo_depth += track->pan_lfo_step;

        if (track->pan_lfo_tick != 1)
        {
            short *wave;
            int vol;

            wave = track->pan_lfo_wave;
            if (wave[0] == 0 && wave[1] == 0)
            {
                wave += track->pan_lfo_wave[2];
            }

            vol = ((track->pan_lfo_depth >> 8) * wave[0]) >> 15;
            if (vol != track->pan_lfo_vol)
            {
                track->pan_lfo_vol = vol;
                track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
            }
        }
    }

    if (track->pitch_bend_tick != 0)
    {
        u_int prev;

        track->pitch_bend_tick--;
        prev = track->pitch_bend;
        track->pitch_bend += track->pitch_bend_step;

        if ((track->pitch_bend >> 16) != (prev >> 16))
        {
            track->attr.mask |= S_UPDATE_PITCH;
        }
    }
}

static void update_player_effect(void)
{
    if ((++player_effect_time % 4) != 0) return;
    if (sound_control_flags & S_CONTROL_PAUSE_UPDATE) return;

    if (sound_volume_rate != 0)
    {
        int prev;

        sound_volume_rate--;
        prev = sound_volume;
        sound_volume += sound_volume_step;

        if (sound_volume_rate == 0 && (sound_volume >> 16) == 0 && sound_volume_step < 0)
        {
            sound_stop_player();
        }
        else if ((sound_volume >> 16) != (prev >> 16))
        {
            sound_update_volume();
        }
    }

    if (sound_tempo_rate != 0)
    {
        sound_tempo_rate--;
        sound_tempo += sound_tempo_step;
    }

    if (sound_pitch_rate != 0)
    {
        int prev;

        sound_pitch_rate--;
        prev = sound_pitch;
        sound_pitch += sound_pitch_step;

        if ((sound_pitch >> 16) != (prev >> 16))
        {
            for (int i = 0; i < S_NUM_VOICES; i++)
            {
                sound_tracks[i].attr.mask |= S_UPDATE_PITCH;
            }
        }
    }
}

static void tick(void)
{
    flush_key_on();

    if (sound_player.active_mask)
    {
        int tempo;
        u_char mul;

        tempo = sound_player.tempo >> 16;
        mul = sound_tempo >> 16;

        if (mul)
        {
            if (mul < 0x80)
            {
                tempo += (tempo * mul) >> 7;
            }
            else
            {
                tempo = (tempo * mul) >> 8;
            }
        }

        sound_player.tempo_delta += tempo;

        if ((sound_player.tempo_delta >> 16) != 0 || (sound_control_flags & S_CONTROL_DOUBLE_SPEED))
        {
            sound_player.tempo_delta &= 0xffff;

            for (int i = 0; i < S_NUM_VOICES; i++)
            {
                if (sound_player.active_mask & (1 << i))
                {
                    sound_tracks[i].length -= 0x101;

                    if ((sound_tracks[i].length & 0xFF) == 0)
                    {
                        sound_exec_sequence(&sound_tracks[i]);
                    }
                    else if ((sound_tracks[i].length >> 8) == 0)
                    {
                        sound_tracks[i].length += 256;
                        sound_player.off_mask |= (1 << i);
                        sound_player.keyed_mask &= ~(1 << i);
                    }

                    update_effect(&sound_tracks[i]);
                }
            }

            if (sound_player.tempo_time != 0)
            {
                sound_player.tempo_time--;
                sound_player.tempo += sound_player.tempo_step;
            }

            if (sound_player.reverb_depth_time != 0)
            {
                sound_player.reverb_depth_time--;
                sound_player.reverb_depth += sound_player.reverb_depth_step;
                sound_player.update |= S_UPDATE_REVERB;
            }

            if (sound_player.num_ticks != 0 && ++sound_player.tick == sound_player.num_ticks)
            {
                sound_player.tick = 0;
                if (++sound_player.beat == sound_player.num_beats)
                {
                    sound_player.beat = 0;
                    sound_player.measure++;
                }
            }
        }
    }

    if (sound_player.mute != 0)
    {
        sound_pause();
        sound_player.mute = 0;
    }

    update_player_effect();
    flush_key_off();
}

/*---------------------------------------------------------------------------*/

int sound_main(void **buffer, size_t *size)
{
    int whole;

    player_accum += S_ACCUM;
    whole = player_accum;
    player_accum -= whole;

    tick();
    spu_step(whole, sample_buffer);
    *buffer = sample_buffer;
    *size = whole;

    /* return whether the song is still playing */
    return sound_player.music_id != 0;
}

void sound_start_player(void)
{
    u_short *buffer;

    player_accum = 0.0f;

    buffer = (u_short *)music_buffer;
    sound_player.active_mask = *(int *)buffer & S_VOICE_MASK;
    sound_player.off_mask |= S_VOICE_MASK;
    buffer += 2;

    for (int i = 0; i < S_NUM_VOICES; i++)
    {
        if (sound_player.active_mask & (1 << i))
        {
            u_short offset;

            sound_init_instrument(&sound_tracks[i], 20);

            offset = *buffer++;
            sound_tracks[i].addr = (u_char *)buffer + offset;
            sound_tracks[i].length = 0x103;
            sound_tracks[i].master_vol = S_VOL_MAX;
            sound_tracks[i].vol = S_MASTER_VOL << 16;
            sound_tracks[i].overlay_pan = S_PAN_CENTER << 8;
            sound_tracks[i].pan = S_PAN_CENTER << 8;
            sound_tracks[i].drum_addr = (S_DRUM *)music_buffer;
            sound_tracks[i].fine_tune = 0;
            sound_tracks[i].transpose = 0;
            sound_tracks[i].pitch_bend = 0;
            sound_tracks[i].key_step = 0;
            sound_tracks[i].pitch_bend_tick = 0;
            sound_tracks[i].portamento_time = 0;
            sound_tracks[i].fix_length = 0;
            sound_tracks[i].base_length = 0;
            sound_tracks[i].overlay_pan_time = 0;
            sound_tracks[i].vol_time = 0;
            sound_tracks[i].pan_time = 0;
            sound_tracks[i].update = 0;
            sound_tracks[i].loop_num = 0;
            sound_tracks[i].effect = 0;
            sound_tracks[i].pan_lfo_vol = 0;
            sound_tracks[i].pan_lfo_depth = 0;
            sound_tracks[i].tremolo_depth = 0;
            sound_tracks[i].vibrato_depth = 0;
            sound_tracks[i].pan_lfo_time = 0;
            sound_tracks[i].tremolo_depth_time = 0;
            sound_tracks[i].vibrato_depth_time = 0;
            sound_tracks[i].pitch_lfo_time = 0;
            sound_tracks[i].noise_time = 0;
        }
    }

    sound_player.tempo = ~0xffff;
    sound_player.tempo_delta = 1;
    sound_player.tempo_time = 0;
    sound_player.reverb_depth = 0;
    sound_player.reverb_depth_time = 0;
    sound_player.reverb_depth_step = 0;
    sound_player.update = 0;
    sound_player.tick = 0;
    sound_player.num_ticks = 0;
    sound_player.beat = 0;
    sound_player.measure = 0;
    sound_player.noise_mask = 0;
    sound_player.reverb_mask = 0;
    sound_player.pitch_lfo_mask = 0;
    sound_player.cond = 0;
    sound_player.prev_cond = 0;
    sound_player.mute = 0;
    sound_player.keyed_mask = 0;
    sound_player.on_mask = 0;
    sound_player.alt_mask = 0;
    sound_player.overlay_mask = 0;

    if (sound_control_flags & S_CONTROL_PAUSE_UPDATE)
    {
        sound_player.active_mask = 0;
    }

    sound_flush_noise();
    sound_flush_reverb();
    sound_flush_pitch_lfo();
}

void sound_stop_player(void)
{
    if (sound_player.active_mask)
    {
        sound_player.active_mask |= sound_player.overlay_mask | sound_player.alt_mask;
        sound_player.off_mask |= sound_player.active_mask;
        sound_player.alt_mask = 0;
        sound_player.overlay_mask = 0;
        sound_player.keyed_mask = 0;
        sound_player.on_mask = 0;

        for (int i = 0; i < S_NUM_VOICES; i++)
        {
            if (sound_player.active_mask & (1 << i))
            {
                sound_tracks[i].addr = empty_sequence;
                sound_tracks[i].length = 0x204;
            }
        }
    }
}

void sound_init_instrument(S_TRACK *track, int instrument)
{
    track->instrument = instrument;
    track->attr.addr = sound_instruments[instrument].addr;
    track->attr.loop_addr = sound_instruments[instrument].loop_addr;
    track->attr.a_mode = sound_instruments[instrument].a_mode;
    track->attr.s_mode = sound_instruments[instrument].s_mode;
    track->attr.r_mode = sound_instruments[instrument].r_mode;
    track->attr.ar = sound_instruments[instrument].ar;
    track->attr.dr = sound_instruments[instrument].dr;
    track->attr.sl = sound_instruments[instrument].sl;
    track->attr.sr = sound_instruments[instrument].sr;
    track->attr.rr = sound_instruments[instrument].rr;
    track->attr.mask |= S_UPDATE_INSTR;
}

void sound_update_volume(void)
{
    for (int i = 0; i < S_NUM_VOICES; i++)
    {
        if (sound_player.active_mask & (1 << i))
        {
            sound_tracks[i].attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
        }
    }
}

void sound_flush(int voice, S_ATTR *attr)
{
    if (attr->mask & (S_UPDATE_VOLL | S_UPDATE_VOLR))
    {
        spu_set_voice_volume(voice, attr->vol_l, attr->vol_r);
        attr->mask &= ~(S_UPDATE_VOLL | S_UPDATE_VOLR);
    }

    if (attr->mask & S_UPDATE_PITCH)
    {
        spu_set_voice_pitch(voice, attr->pitch);
        attr->mask &= ~S_UPDATE_PITCH;
    }

    if (attr->mask & S_UPDATE_WDSA)
    {
        spu_set_voice_address(voice, attr->addr);
        attr->mask &= ~S_UPDATE_WDSA;
    }

    if (attr->mask & S_UPDATE_LSAX)
    {
        spu_set_voice_loop_address(voice, attr->loop_addr);
        attr->mask &= ~S_UPDATE_LSAX;
    }

    if (attr->mask & (S_UPDATE_AMODE | S_UPDATE_AR))
    {
        spu_set_voice_attack(voice, attr->a_mode, attr->ar);
        attr->mask &= ~(S_UPDATE_AMODE | S_UPDATE_AR);
    }

    if (attr->mask & (S_UPDATE_DR))
    {
        spu_set_voice_decay(voice, attr->dr);
        attr->mask &= ~(S_UPDATE_DR);
    }

    if (attr->mask & (S_UPDATE_SMODE | S_UPDATE_SR | S_UPDATE_SL))
    {
        spu_set_voice_sustain(voice, attr->s_mode, attr->sr, attr->sl);
        attr->mask &= ~(S_UPDATE_SMODE | S_UPDATE_SR | S_UPDATE_SL);
    }

    if (attr->mask & (S_UPDATE_RMODE | S_UPDATE_RR))
    {
        spu_set_voice_release(voice, attr->r_mode, attr->rr);
        attr->mask &= ~(S_UPDATE_RMODE | S_UPDATE_RR);
    }
}

void sound_flush_noise(void)
{
    u_int mask;

    mask = 0;

    if (sound_player.noise_mask)
    {
        get_active_voices(&mask, sound_player.noise_mask);
    }

    spu_set_noise_on(mask);
    spu_set_noise_off(mask ^ S_VOICE_MASK);
}

void sound_flush_reverb(void)
{
    u_int mask;

    mask = 0;

    if (sound_control_flags & S_CONTROL_REVERB_ENABLE)
    {
        mask = S_VOICE_MASK;
    }
    else if (sound_player.reverb_mask)
    {
        get_active_voices(&mask, sound_player.reverb_mask);
    }

    spu_set_reverb_on(mask);
    spu_set_reverb_off(mask ^ S_VOICE_MASK);
}

void sound_flush_pitch_lfo(void)
{
    u_int mask;

    mask = 0;

    if (sound_player.pitch_lfo_mask)
    {
        get_active_voices(&mask, sound_player.pitch_lfo_mask);
    }

    spu_set_pitch_lfo_on(mask);
    spu_set_pitch_lfo_off(mask ^ S_VOICE_MASK);
}

void sound_set_reverb_mode(int mode)
{
    sound_player.reverb_mode = mode;
    spu_set_reverb_enable(0);
    spu_set_reverb_mode(mode);
    spu_reverb_clear();
    spu_set_reverb_enable(1);
}

void sound_copy_music(void *data, u_int size)
{
    assert(size <= S_MUSIC_BUFFER_SIZE);
    memcpy(music_buffer, data, size);
}
