#include <stdio.h>
#include "sound.h"

/*---------------------------------------------------------------------------*/

static u_short note_length[11] =
{
    0xC0C0, 0x6060, 0x3030, 0x1818, 0x0C0C, 0x0606, 0x0303, 0x2020,
    0x1010, 0x0808, 0x0404
};

static short wave_data[708] =
{
    0x1FFF, 0xE001, 0x3FFF, 0xC001, 0x5FFF, 0xA001, 0x7FFF, 0x8001,
    0x0000, 0x0000, 0xFFFE, 0x0000, 0x7FFF, 0x8001, 0x0000, 0x0000,
    0xFFFE, 0x0000, 0x1FFF, 0x0000, 0x3FFF, 0x0000, 0x5FFF, 0x0000,
    0x7FFF, 0x0000, 0x0000, 0xFFFE, 0x7FFF, 0x0000, 0x7FFF, 0x0000,
    0x0000, 0xFFFE, 0xE001, 0x0000, 0xC001, 0x0000, 0xA001, 0x0000,
    0x8001, 0x0000, 0x0000, 0xFFFE, 0x8001, 0x0000, 0x8001, 0x0000,
    0x0000, 0xFFFE, 0x0000, 0x0500, 0x09E2, 0x12CD, 0x169E, 0x19E1,
    0x1C81, 0x1E6D, 0x1F9A, 0x1FFF, 0x1F9C, 0x1E71, 0x1C86, 0x19E8,
    0x16A7, 0x12D7, 0x0E90, 0x09EE, 0x050D, 0x0000, 0xFB0C, 0xF62A,
    0xF186, 0xED3D, 0xE96A, 0xE626, 0xE384, 0xE196, 0xE067, 0xE000,
    0xE061, 0xE18A, 0xE373, 0xE60F, 0xE94F, 0xED1D, 0xF163, 0xF604,
    0xFAE5, 0x0000, 0x0A01, 0x13C4, 0x259A, 0x2D3C, 0x33C2, 0x3902,
    0x3CDA, 0x3F34, 0x3FFF, 0x3F38, 0x3CE2, 0x390D, 0x33D1, 0x2D4F,
    0x25AF, 0x1D21, 0x13DD, 0x0A1B, 0x0000, 0xF618, 0xEC54, 0xE30C,
    0xDA7B, 0xD2D5, 0xCC4D, 0xC709, 0xC32D, 0xC0CF, 0xC000, 0xC0C3,
    0xC315, 0xC6E6, 0xCC1F, 0xD29E, 0xDA3B, 0xE2C6, 0xEC09, 0xF5CA,
    0x0000, 0x0F02, 0x1DA6, 0x3867, 0x43DA, 0x4DA3, 0x5583, 0x5B47,
    0x5ECE, 0x5FFF, 0x5ED4, 0x5B53, 0x5594, 0x4DBA, 0x43F6, 0x3886,
    0x2BB2, 0x1DCB, 0x0F29, 0x0000, 0xF124, 0xE27E, 0xD492, 0xC7B8,
    0xBC40, 0xB273, 0xAA8E, 0xA4C3, 0xA137, 0xA000, 0xA124, 0xA49F,
    0xAA59, 0xB22E, 0xBBED, 0xC759, 0xD429, 0xE20E, 0xF0AF, 0x0000,
    0x1403, 0x2788, 0x4B34, 0x5A79, 0x6784, 0x7204, 0x79B5, 0x7E68,
    0x7FFF, 0x7E71, 0x79C5, 0x721B, 0x67A3, 0x5A9E, 0x4B5E, 0x3A43,
    0x27BA, 0x1437, 0x0000, 0xEC30, 0xD8A8, 0xC619, 0xB4F6, 0xA5AB,
    0x989A, 0x8E13, 0x865A, 0x819F, 0x8000, 0x8186, 0x862A, 0x8DCC,
    0x983E, 0xA53D, 0xB477, 0xC58D, 0xD813, 0xEB95, 0x0000, 0x0000,
    0xFFD9, 0x0000, 0x0000, 0x1403, 0x2788, 0x4B34, 0x5A79, 0x6784,
    0x7204, 0x79B5, 0x7E68, 0x7FFF, 0x7E71, 0x79C5, 0x721B, 0x67A3,
    0x5A9E, 0x4B5E, 0x3A43, 0x27BA, 0x1437, 0x0000, 0xEC30, 0xD8A8,
    0xC619, 0xB4F6, 0xA5AB, 0x989A, 0x8E13, 0x865A, 0x819F, 0x8000,
    0x8186, 0x862A, 0x8DCC, 0x983E, 0xA53D, 0xB477, 0xC58D, 0xD813,
    0xEB95, 0x0000, 0x0000, 0xFFD9, 0x0000, 0x09E2, 0x12CD, 0x19E1,
    0x1E6D, 0x1FFF, 0x1E71, 0x19E8, 0x12D7, 0x09EE, 0x000D, 0xF62A,
    0xED3D, 0xE626, 0xE196, 0xE000, 0xE18A, 0xE60F, 0xED1D, 0xF604,
    0x0000, 0x13C4, 0x259A, 0x33C2, 0x3CDA, 0x3FFF, 0x3CE2, 0x33D1,
    0x25AF, 0x13DD, 0x001A, 0xEC54, 0xDA7B, 0xCC4D, 0xC32D, 0xC000,
    0xC315, 0xCC1F, 0xDA3B, 0xEC09, 0x0000, 0x1DA6, 0x3867, 0x4DA3,
    0x5B47, 0x5FFF, 0x5B53, 0x4DBA, 0x3886, 0x1DCB, 0x0027, 0xE27E,
    0xC7B8, 0xB273, 0xA4C3, 0xA000, 0xA49F, 0xB22E, 0xC759, 0xE20E,
    0x0000, 0x2788, 0x4B34, 0x6784, 0x79B5, 0x7FFF, 0x79C5, 0x67A3,
    0x4B5E, 0x27BA, 0x0034, 0xD8A8, 0xB4F6, 0x989A, 0x865A, 0x8000,
    0x862A, 0x983E, 0xB477, 0xD813, 0x0000, 0x0000, 0xFFEC, 0x0000,
    0x0000, 0x2788, 0x4B34, 0x6784, 0x79B5, 0x7FFF, 0x79C5, 0x67A3,
    0x4B5E, 0x27BA, 0x0034, 0xD8A8, 0xB4F6, 0x989A, 0x865A, 0x8000,
    0x862A, 0x983E, 0xB477, 0xD813, 0x0000, 0x0000, 0xFFEC, 0x0000,
    0x0000, 0x07FF, 0x0FFF, 0x17FF, 0x1FFF, 0x17FF, 0x0FFF, 0x07FF,
    0x0000, 0xF801, 0xF001, 0xE801, 0xE001, 0xE801, 0xF001, 0xF801,
    0x0000, 0x0FFF, 0x1FFF, 0x2FFF, 0x3FFF, 0x2FFF, 0x1FFF, 0x0FFF,
    0x0000, 0xF001, 0xE001, 0xD001, 0xC001, 0xD001, 0xE001, 0xF001,
    0x0000, 0x17FF, 0x2FFF, 0x47FF, 0x5FFF, 0x47FF, 0x2FFF, 0x17FF,
    0x0000, 0xE801, 0xD001, 0xB801, 0xA001, 0xB801, 0xD001, 0xE801,
    0x0000, 0x1FFF, 0x3FFF, 0x5FFF, 0x7FFF, 0x5FFF, 0x3FFF, 0x1FFF,
    0x0000, 0xE001, 0xC001, 0xA001, 0x8001, 0xA001, 0xC001, 0xE001,
    0x0000, 0x0000, 0xFFF0, 0x0000, 0x0000, 0x1FFF, 0x3FFF, 0x5FFF,
    0x7FFF, 0x5FFF, 0x3FFF, 0x1FFF, 0x0000, 0xE001, 0xC001, 0xA001,
    0x8001, 0xA001, 0xC001, 0xE001, 0x0000, 0x0000, 0xFFF0, 0x0000,
    0x0000, 0x31FD, 0x7D05, 0xF5FD, 0xECEE, 0x793C, 0x75D0, 0x07FC,
    0xF411, 0xC2F5, 0x660F, 0x330E, 0x3FEA, 0xBEE1, 0x8C12, 0x1821,
    0xAFFB, 0x4A00, 0xD3E7, 0xF4F5, 0xE137, 0xE7D3, 0x520D, 0x1103,
    0xAEDF, 0x4AF8, 0x0746, 0x70F4, 0x8EC1, 0x9719, 0x3512, 0xF600,
    0x1B00, 0xB4E9, 0x3B1E, 0xAE15, 0x9CC8, 0x65F9, 0xC323, 0x71F8,
    0x2CF1, 0xA604, 0x92FC, 0xC90E, 0xB009, 0x10E5, 0x0804, 0x8001,
    0x12FA, 0x1118, 0xE709, 0x47F8, 0x95D9, 0xDD1C, 0x1B09, 0x0BF4,
    0x7BFC, 0x20FF, 0x9404, 0x7DFC, 0x8AF5, 0xB717, 0x74F9, 0x2AFC,
    0xDD06, 0xD7F3, 0xF4FD, 0x0608, 0xF218, 0x69DF, 0xBC07, 0x4AF7,
    0x89EB, 0xBB27, 0x1109, 0x7FEF, 0x3615, 0xECCD, 0x7621, 0x9815,
    0x56CF, 0xD406, 0x3322, 0xDFE6, 0xAF0A, 0xB9F5, 0xCE08, 0x5D05,
    0xBFEE, 0xF01A, 0x20E8, 0x9F16, 0x8CF2, 0xC2E7, 0xD22E, 0xC601,
    0xB9D0, 0x810C, 0x4323, 0x28DD, 0x96F7, 0x5C3C, 0x9FDE, 0xAEE8,
    0xB61B, 0x3EF3, 0x1408, 0xDAE9, 0x2C0B, 0xB133, 0xA8E1, 0x05DA,
    0x0914, 0x4E22, 0xFFE8, 0x87F1, 0x21F0, 0x8D25, 0x7DFC, 0x1CF0,
    0x5E11, 0x3DE8, 0xC70A, 0x8F17, 0xFFF0, 0x6A0D, 0xFFE3, 0x12F3,
    0x041F, 0x780C, 0xC4EF, 0x5B02, 0x3D02, 0x02F2, 0x1519, 0xDAD5,
    0xBD09, 0x9219, 0xF503, 0x62DD, 0x3806, 0x0F26, 0xA8FB, 0xE0FE,
    0xF6E3, 0x0E02, 0x0210, 0x4406, 0x94D4, 0xD10A, 0xA846, 0x10F1,
    0x1ACC, 0xBDFD, 0x1A30, 0xA0EB, 0x5FEA, 0xBF10, 0xBBEF, 0xDF1B,
    0xC20C, 0x2AE0, 0x3EF2, 0xAE27, 0x5001, 0xF9DD, 0x3611, 0xAD1A,
    0x07DF, 0x6D05, 0x1A0D, 0xB6DB, 0x6D2B, 0x3607, 0xFBCC, 0x4208,
    0x0F21, 0x36F6, 0xC806, 0x0603, 0xFDD7, 0x7817, 0x0605, 0xAF0B,
    0x95FC, 0x10EC, 0xA70E, 0xADF3, 0x660F, 0xBDF2, 0xDEF8, 0xE421,
    0xB1E7, 0xFCED, 0x5B1C, 0xA109, 0xCCF2, 0xF5F3, 0x9AFB, 0xB417,
    0x92E8, 0x7F08, 0x4BFA, 0x89FD, 0x3C1C, 0xFCEF, 0xF2DB, 0x4A1D,
    0x671D, 0xD6FF, 0x65DA, 0xF1FC, 0x6E0A, 0x0A0C, 0x11F8, 0x4A02,
    0x5BFC, 0xEB06, 0xA5ED, 0x30E6, 0x8011, 0xD234, 0x08F8, 0x43DC,
    0xF609, 0xA6F2, 0x9A0D, 0x7006, 0xF5EA, 0x9CFC, 0x811E, 0xADE6,
    0xE60E, 0x95F7, 0xB304, 0x9CFF, 0xEFEA, 0xF519, 0xFA00, 0x3AF6,
    0xC7D5, 0x2722, 0xBB38, 0x43D1, 0x77ED, 0x4908, 0xCE02, 0xAB0F,
    0x3DEB, 0xDB0A, 0x6401, 0x3FF9, 0x76F2, 0x9309, 0xD20F, 0x74E5,
    0x0000, 0x0000, 0xFF00, 0x0000
};

static short *wave_table[16] =
{
    wave_data,
    wave_data + 12,
    wave_data + 18,
    wave_data + 28,
    wave_data + 34,
    wave_data + 44,
    wave_data + 50,
    wave_data + 210,
    wave_data + 360,
    wave_data + 428,
    wave_data + 252,
    wave_data + 336,
    wave_data + 448,
    wave_data + 428,
    wave_data + 360,
    wave_data + 428,
};

static u_char opcode_param_length[96] =
{
    0, 2, 2, 2, 3, 2, 1, 1,
    2, 3, 2, 3, 2, 2, 2, 2,
    3, 2, 2, 1, 4, 2, 1, 2,
    4, 2, 1, 2, 3, 2, 1, 2,
    2, 2, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 2, 2,
    1, 0, 2, 2, 1, 1, 1, 1,
    2, 2, 2, 0, 2, 3, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 4, 3, 4, 3, 1, 0, 0,
    0, 0, 2, 1, 3, 1, 2, 3,
    2, 1, 0, 0, 0, 3, 3, 0,
};

static inline u_int read8(S_TRACK *track)
{
    return *track->addr++;
}

static inline u_int read16(S_TRACK *track)
{
    u_char lo, hi;
    lo = *track->addr++;
    hi = *track->addr++;
    return lo | (hi << 8);
}

static void display_bpm(void)
{
    float tempo, bpm;

    if (sound_player.num_ticks == 0) return;

    tempo = ((float)(sound_player.tempo >> 16) / 65536.0f) * 240.0f;
    bpm = (tempo * 60.0f) / sound_player.num_ticks;
    printf("sequencer set to %.02f bpm\n", bpm);
}

/*---------------------------------------------------------------------------*/

void set_tempo(S_TRACK *track)
{
    sound_player.tempo = read16(track) << 16;
    sound_player.tempo_time = 0;

    if (sound_debug)
    {
        display_bpm();
    }
}

void move_tempo(S_TRACK *track)
{
    int delta;

    sound_player.tempo_time = read8(track);
    if (sound_player.tempo_time == 0) sound_player.tempo_time = 256;
    sound_player.tempo &= ~0xffff;

    delta = (read16(track) << 16) - sound_player.tempo;
    sound_player.tempo_step = delta / sound_player.tempo_time;
}

void set_reverb_depth(S_TRACK *track)
{
    sound_player.reverb_depth = read16(track) << 16;
    sound_player.reverb_depth_time = 0;
    sound_player.update |= S_UPDATE_REVERB;
}

void move_reverb_depth(S_TRACK *track)
{
    int delta;

    sound_player.reverb_depth_time = read8(track);
    if (sound_player.reverb_depth_time == 0) sound_player.reverb_depth_time = 256;
    sound_player.reverb_depth &= ~0xffff;

    delta = (read16(track) << 16) - sound_player.reverb_depth;
    sound_player.reverb_depth_step = delta / sound_player.reverb_depth_time;
}

void set_master_vol(S_TRACK *track)
{
    track->master_vol = read8(track);
    track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
}

void set_vol(S_TRACK *track)
{
    track->vol = (signed char)read8(track) << 23;
    track->vol_time = 0;
    track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
}

void move_vol(S_TRACK *track)
{
    int delta;

    track->vol_time = read8(track);
    if (track->vol_time == 0) track->vol_time = 256;
    track->vol &= ~0xffff;

    delta = ((signed char)read8(track) << 23) - track->vol;
    track->vol_step = delta / track->vol_time;
}

void set_overlay_voice_on(S_TRACK *track)
{
    int num;

    if (track->update & S_UPDATE_OVERLAY)
    {
        num = track->overlay_num;
    }
    else
    {
        num = 0;
        while ((sound_player.active_mask | sound_player.overlay_mask | sound_player.alt_mask) & (1 << num))
        {
            num++;
        }
    }

    if (num < S_NUM_VOICES)
    {
        sound_init_instrument(track, read8(track));
        sound_init_instrument(&sound_tracks[num], read8(track));
        sound_player.overlay_mask |= 1 << num;
        track->overlay_num = num;
        track->update |= S_UPDATE_OVERLAY;
    }
    else
    {
        track->overlay_num = 0;

        if (sound_debug)
        {
            printf("no space for overlay voice!\n");
        }
    }
}

void set_overlay_voice_off(S_TRACK *track)
{
    if (track->update & S_UPDATE_OVERLAY)
    {
        track->update &= ~S_UPDATE_OVERLAY;
        sound_player.overlay_mask &= ~(1 << track->overlay_num);
    }
}

void set_overlay_pan(S_TRACK *track)
{
    track->overlay_pan = read8(track) << 8;
    track->overlay_pan_time = 0;

    if (track->update & S_UPDATE_OVERLAY)
    {
        track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
    }
}

void move_overlay_pan(S_TRACK *track)
{
    int delta;

    track->overlay_pan_time = read8(track);
    if (track->overlay_pan_time == 0) track->overlay_pan_time = 256;
    track->overlay_pan &= ~0xff;

    delta = (read8(track) << 8) - track->overlay_pan;
    track->overlay_pan_step = delta / track->overlay_pan_time;
}

void set_pan(S_TRACK *track)
{
    track->pan = read8(track) << 8;
    track->pan_time = 0;
    track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
}

void move_pan(S_TRACK *track)
{
    int delta;

    track->pan_time = read8(track);
    if (track->pan_time == 0) track->pan_time = 256;
    track->pan &= ~0xff;

    delta = (read8(track) << 8) - track->pan;
    track->pan_step = delta / track->pan_time;
}

void set_octave(S_TRACK *track)
{
    track->octave = read8(track);
}

void inc_octave(S_TRACK *track)
{
    track->octave = (track->octave + 1) % 16;
}

void dec_octave(S_TRACK *track)
{
    track->octave = (track->octave - 1) % 16;
}

void load_instrument(S_TRACK *track)
{
    int instr;

    instr = read8(track);

    if (track->update & S_UPDATE_OVERLAY)
    {
        track->update &= ~S_UPDATE_OVERLAY;
        sound_player.overlay_mask &= ~(1 << track->overlay_num);
    }

    track->attr.mask |= S_UPDATE_PITCH;
    track->pitch *= sound_instruments[instr].pitch[0];
    track->pitch /= sound_instruments[track->instrument].pitch[0];

    if (track->update & S_UPDATE_ALTERNATIVE)
    {
        track->instrument = instr;
        track->attr.addr = sound_instruments[instr].addr;
        track->attr.loop_addr = sound_instruments[instr].loop_addr;
        track->attr.ar = sound_instruments[instr].ar;
        track->attr.dr = sound_instruments[instr].dr;
        track->attr.sl = sound_instruments[instr].sl;
        track->attr.sr = sound_instruments[instr].sr;
        track->attr.a_mode = sound_instruments[instr].a_mode;
        track->attr.s_mode = sound_instruments[instr].s_mode;
        track->attr.mask |= S_UPDATE_INSTR & ~(S_UPDATE_RMODE | S_UPDATE_RR);
    }
    else
    {
        sound_init_instrument(track, instr);
    }
}

void reset_instrument(S_TRACK *track)
{
    int instr;

    instr = read8(track);

    track->attr.mask |= S_UPDATE_PITCH;
    track->pitch *= sound_instruments[instr].pitch[0];
    track->pitch /= sound_instruments[track->instrument].pitch[0];

    track->instrument = instr;
    track->attr.addr = S_DEFAULT_SOUND_ADDR;
    track->attr.loop_addr = sound_instruments[instr].loop_addr;
    track->attr.ar = sound_instruments[instr].ar;
    track->attr.dr = sound_instruments[instr].dr;
    track->attr.sl = sound_instruments[instr].sl;
    track->attr.sr = sound_instruments[instr].sr;
    track->attr.a_mode = sound_instruments[instr].a_mode;
    track->attr.s_mode = sound_instruments[instr].s_mode;

    if (track->update & S_UPDATE_ALTERNATIVE)
    {
        track->attr.mask |= S_UPDATE_INSTR & ~(S_UPDATE_RMODE | S_UPDATE_RR);
    }
    else
    {
        track->attr.rr = sound_instruments[instr].rr;
        track->attr.r_mode = sound_instruments[instr].r_mode;
        track->attr.mask |= S_UPDATE_INSTR;
    }
}

void reset_adsr(S_TRACK *track)
{
    S_INSTRUMENT* instr;

    instr = &sound_instruments[track->instrument];
    track->attr.ar = instr->ar;
    track->attr.dr = instr->dr;
    track->attr.sl = instr->sl;
    track->attr.sr = instr->sr;
    track->attr.rr = instr->rr;
    track->attr.a_mode = instr->a_mode;
    track->attr.s_mode = instr->s_mode;
    track->attr.r_mode = instr->r_mode;
    track->attr.mask |= S_UPDATE_ADSR;

    if (track->update & S_UPDATE_OVERLAY)
    {
        S_TRACK *overlay;

        overlay = &sound_tracks[track->overlay_num];
        overlay->attr.ar = track->attr.ar;
        overlay->attr.dr = track->attr.dr;
        overlay->attr.sl = track->attr.sl;
        overlay->attr.sr = track->attr.sr;
        overlay->attr.rr = track->attr.rr;
        overlay->attr.a_mode = track->attr.a_mode;
        overlay->attr.s_mode = track->attr.s_mode;
        overlay->attr.r_mode = track->attr.r_mode;
    }
}

void set_transpose(S_TRACK *track)
{
    track->transpose = (signed char)read8(track);
}

void move_transpose(S_TRACK *track)
{
    track->transpose += (signed char)read8(track);
}

void set_pitch_bend(S_TRACK *track)
{
    track->pitch_bend_time = read8(track);
    if (track->pitch_bend_time == 0) track->pitch_bend_time = 256;
    track->key_step = (signed char)read8(track);
}

void set_portamento_on(S_TRACK *track)
{
    track->portamento_time = read8(track);
    if (track->portamento_time == 0) track->portamento_time = 256;
    track->prev_transpose = 0;
    track->prev_key = 0;
    track->effect = S_EFFECT_LEGATO;
}

void set_portamento_off(S_TRACK *track)
{
    track->portamento_time = 0;
}

void set_fine_tune(S_TRACK *track)
{
    track->fine_tune = (signed char)read8(track);
}

void move_fine_tune(S_TRACK *track)
{
    track->fine_tune += (signed char)read8(track);
}

void set_vibrato_on(S_TRACK *track)
{
    int depth, pitch;

    track->update |= S_UPDATE_VIBRATO;

    track->vibrato_delay = read8(track);

    track->vibrato_rate = read8(track);
    if (track->vibrato_rate == 0) track->vibrato_rate = 256;

    track->vibrato_type = read8(track);

    depth = (track->vibrato_depth & 0x7f00) >> 8;
    pitch = track->pitch & 0xffff;
    if (!(track->vibrato_depth & 0x8000)) pitch = (pitch * 15) >> 8;

    track->vibrato_base = (depth * pitch) >> 7;
    track->vibrato_wave = wave_table[track->vibrato_type];
    track->vibrato_delay_tick = track->vibrato_delay;
    track->vibrato_rate_tick = 1;
}

void set_vibrato_depth(S_TRACK *track)
{
    int depth, pitch;

    track->vibrato_depth = read8(track) << 8;

    depth = (track->vibrato_depth & 0x7f00) >> 8;
    pitch = track->pitch;
    if (!(track->vibrato_depth & 0x8000)) pitch = (pitch * 15) >> 8;

    track->vibrato_base = (depth * pitch) >> 7;
}

void move_vibrato_depth(S_TRACK *track)
{
    int delta;

    track->vibrato_depth_time = read8(track);
    if (track->vibrato_depth_time == 0) track->vibrato_depth_time = 256;

    delta = (read8(track) << 8) - track->vibrato_depth;
    track->vibrato_depth_step = delta / track->vibrato_depth_time;
}

void set_vibrato_off(S_TRACK *track)
{
    track->vibrato_pitch = 0;
    track->update &= ~S_UPDATE_VIBRATO;
    track->attr.mask |= S_UPDATE_PITCH;
}

void set_tremolo_on(S_TRACK *track)
{
    track->update |= S_UPDATE_TREMOLO;

    track->tremolo_delay = read8(track);

    track->tremolo_rate = read8(track);
    if (track->tremolo_rate == 0) track->tremolo_rate = 256;

    track->tremolo_type = read8(track);
    track->tremolo_wave = wave_table[track->tremolo_type];
    track->tremolo_delay_tick = track->tremolo_delay;
    track->tremolo_rate_tick = 1;
}

void set_tremolo_depth(S_TRACK *track)
{
    track->tremolo_depth = read8(track) << 8;
}

void move_tremolo_depth(S_TRACK *track)
{
    int delta;

    track->tremolo_depth_time = read8(track);
    if (track->tremolo_depth_time == 0) track->tremolo_depth_time = 256;

    delta = (read8(track) << 8) - track->tremolo_depth;
    track->tremolo_depth_step = delta / track->tremolo_depth_time;
}

void set_tremolo_off(S_TRACK *track)
{
    track->tremolo_vol = 0;
    track->update &= ~S_UPDATE_TREMOLO;
    track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
}

void set_pan_lfo_on(S_TRACK *track)
{
    track->update |= S_UPDATE_PAN_LFO;

    track->pan_lfo_rate = read8(track);
    if (track->pan_lfo_rate == 0) track->pan_lfo_rate = 256;

    track->pan_lfo_type = read8(track);
    track->pan_lfo_wave = wave_table[track->pan_lfo_type];
    track->pan_lfo_tick = 1;
}

void set_pan_lfo_depth(S_TRACK *track)
{
    track->pan_lfo_depth = read8(track) << 7;
}

void move_pan_lfo_depth(S_TRACK *track)
{
    int delta;

    track->pan_lfo_time = read8(track);
    if (track->pan_lfo_time == 0) track->pan_lfo_time = 256;

    delta = (read8(track) << 7) - track->pan_lfo_depth;
    track->pan_lfo_step = delta / track->pan_lfo_time;
}

void set_pan_lfo_off(S_TRACK *track)
{
    track->pan_lfo_vol = 0;
    track->update &= ~S_UPDATE_PAN_LFO;
    track->attr.mask |= (S_UPDATE_VOLL | S_UPDATE_VOLR);
}

void set_noise_on(S_TRACK *track)
{
    (void)track;

    sound_player.noise_mask |= 1 << track->attr.voice_id;
    sound_player.update |= S_UPDATE_NOISE_CLOCK;
    sound_flush_noise();
}

void set_noise_off(S_TRACK *track)
{
    sound_player.noise_mask &= ~(1 << track->attr.voice_id);
    sound_player.update |= S_UPDATE_NOISE_CLOCK;
    sound_flush_noise();
    track->noise_time = 0;
}

void set_pitch_lfo_on(S_TRACK *track)
{
    (void)track;

    sound_player.pitch_lfo_mask |= 1 << track->attr.voice_id;
    sound_flush_pitch_lfo();
}

void set_pitch_lfo_off(S_TRACK *track)
{
    sound_player.pitch_lfo_mask &= ~(1 << track->attr.voice_id);
    sound_flush_pitch_lfo();
    track->pitch_lfo_time = 0;
}

void set_reverb_on(S_TRACK *track)
{
    (void)track;

    sound_player.reverb_mask |= 1 << track->attr.voice_id;
    sound_flush_reverb();
}

void set_reverb_off(S_TRACK *track)
{
    (void)track;

    sound_player.reverb_mask &= ~(1 << track->attr.voice_id);
    sound_flush_reverb();
}

void set_legato_on(S_TRACK *track)
{
    track->effect = S_EFFECT_LEGATO;
}

void set_legato_off(S_TRACK *track)
{
    (void)track;

    /* do nothing */
}

void set_full_length_on(S_TRACK *track)
{
    track->effect = S_EFFECT_FULL_LENGTH;
}

void set_full_length_off(S_TRACK *track)
{
    (void)track;

    /* do nothing */
}

void set_noise_clock(S_TRACK *track)
{
    int clock;

    clock = read8(track);
    if (clock > 63)
    {
        sound_player.noise_clock = (sound_player.noise_clock + (clock & 0x3F)) & 0x3F;
    }
    else
    {
        sound_player.noise_clock = clock;
    }

    sound_player.update |= S_UPDATE_NOISE_CLOCK;
}

void set_ar(S_TRACK *track)
{
    track->attr.ar = read8(track);
    track->attr.mask |= S_UPDATE_AMODE | S_UPDATE_AR;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.ar = track->attr.ar;
    }
}

void set_dr(S_TRACK *track)
{
    track->attr.dr = read8(track);
    track->attr.mask |= S_UPDATE_DR;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.dr = track->attr.dr;
    }
}

void set_sl(S_TRACK *track)
{
    track->attr.sl = read8(track);
    track->attr.mask |= S_UPDATE_SL;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.sl = track->attr.sl;
    }
}

void set_sr(S_TRACK *track)
{
    track->attr.sr = read8(track);
    track->attr.mask |= S_UPDATE_SMODE | S_UPDATE_SR;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.sr = track->attr.sr;
    }
}

void set_rr(S_TRACK *track)
{
    track->attr.rr = read8(track);
    track->attr.mask |= S_UPDATE_RMODE | S_UPDATE_RR;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.rr = track->attr.rr;
    }
}

void set_amode(S_TRACK *track)
{
    track->attr.a_mode = read8(track);
    track->attr.mask |= S_UPDATE_AMODE;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.a_mode = track->attr.a_mode;
    }
}

void set_smode(S_TRACK *track)
{
    track->attr.s_mode = read8(track);
    track->attr.mask |= S_UPDATE_SMODE;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.s_mode = track->attr.s_mode;
    }
}

void set_rmode(S_TRACK *track)
{
    track->attr.r_mode = read8(track);
    track->attr.mask |= S_UPDATE_RMODE;

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_tracks[track->overlay_num].attr.r_mode = track->attr.r_mode;
    }
}

void set_alt_voice_on(S_TRACK *track)
{
    int num;

    track->attr.rr = read8(track);
    if (track->update & S_UPDATE_ALTERNATIVE) return;

    num = 0;
    while ((sound_player.active_mask | sound_player.overlay_mask | sound_player.alt_mask) & (1 << num))
    {
        num++;
    }

    if (num < S_NUM_VOICES)
    {
        sound_player.alt_mask |= 1 << num;
        track->alt_num = num;
        track->update |= S_UPDATE_ALTERNATIVE;
    }
    else
    {
        track->alt_num = 0;

        if (sound_debug)
        {
            printf("no space for alt voice!\n");
        }
    }
}

void set_alt_voice_off(S_TRACK *track)
{
    sound_player.alt_mask &= ~(1 << track->alt_num);
    track->update &= ~S_UPDATE_ALTERNATIVE;
    track->attr.rr = sound_instruments[track->instrument].rr;
    track->attr.mask |= S_UPDATE_RMODE | S_UPDATE_RR;
}

void loop_start(S_TRACK *track)
 {
    track->loop_num = (track->loop_num + 1) % 4;
    track->loop_addr[track->loop_num] = track->addr;
    track->loop_time[track->loop_num] = 0;
}

void loop_end(S_TRACK *track)
{
    int time;

    time = read8(track);
    if (time == 0) time = 256;

    if (++track->loop_time[track->loop_num] == time)
    {
        track->loop_num = (track->loop_num - 1) % 4;
    }
    else
    {
        track->addr = track->loop_addr[track->loop_num];
    }
}

void loop_jump(S_TRACK *track)
{
    int time, offset;

    time = read8(track);
    if (time == 0) time = 256;

    if ((track->loop_time[track->loop_num] + 1) == time)
    {
        offset = (short)read16(track);
        track->addr += offset;
    }
    else
    {
        track->addr += 2;
    }
}

void loop_leave(S_TRACK *track)
{
    int time, offset;

    time = read8(track);
    if (time == 0) time = 256;

    if ((track->loop_time[track->loop_num] + 1) == time)
    {
        offset = (short)read16(track);
        track->addr += offset;
        track->loop_num = (track->loop_num - 1) & 3;
    }
    else
    {
        track->addr += 2;
    }
}

void loop_repeat(S_TRACK *track)
{
    track->loop_time[track->loop_num]++;
    track->addr = track->loop_addr[track->loop_num];
}

void set_note_length(S_TRACK *track)
{
    int len;

    len = read8(track);
    track->fix_length = 0;
    track->length = (len << 8) | len;
    track->base_length = len;
}

void set_note_fix_length(S_TRACK *track)
{
    int delta;

    delta = (short)read8(track);
    if (delta)
    {
        delta += track->base_length;
        if (delta < 1) delta = 1;
        if (delta > 255) delta = 255;
    }
    track->fix_length = delta;
}

void set_drum_on(S_TRACK *track)
{
    int offset;

    offset = (short)read16(track);
    track->drum_addr = (S_DRUM *)(track->addr + offset);
    track->update |= S_UPDATE_DRUM_MODE;
}

void set_drum_off(S_TRACK *track)
{
    track->update &= ~S_UPDATE_DRUM_MODE;
}

void set_time_signature(S_TRACK *track)
{
    sound_player.num_ticks = read8(track);
    sound_player.num_beats = read8(track);
    sound_player.tick = 0;
    sound_player.beat = 0;

    if (sound_debug)
    {
        display_bpm();
    }
}

void set_measure(S_TRACK *track)
{
    sound_player.measure = read16(track);
}

void set_mute(S_TRACK *track)
{
    (void)track;

    sound_player.mute = 1;
}

void set_dr_sl(S_TRACK *track)
{
    set_dr(track);
    set_sl(track);
}

void set_noise_switch_force(S_TRACK *track)
{
    track->noise_time = read8(track) + 1;
    if (track->noise_time == 1) track->noise_time = 257;
    set_noise_on(track);
}

void set_noise_switch(S_TRACK *track)
{
    track->noise_time = read8(track) + 1;
    if (track->noise_time == 1) track->noise_time = 257;
}

void set_pitch_lfo_switch_force(S_TRACK *track)
{
    track->pitch_lfo_time = read8(track) + 1;
    if (track->pitch_lfo_time == 1) track->pitch_lfo_time = 257;
    set_pitch_lfo_on(track);
}

void set_pitch_lfo_switch(S_TRACK *track)
{
    track->pitch_lfo_time = read8(track) + 1;
    if (track->pitch_lfo_time == 1) track->pitch_lfo_time = 257;
}

void set_effect_off(S_TRACK *track)
{
    track->update &= ~S_UPDATE_LFO;
    set_noise_off(track);
    set_pitch_lfo_off(track);
    set_reverb_off(track);
    track->effect &= ~(S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH);
}

void set_side_chain_on(S_TRACK *track)
{
    track->update |= S_UPDATE_SCHAIN_PITCH;
}

void set_side_chain_off(S_TRACK *track)
{
    track->update &= ~S_UPDATE_SCHAIN_PITCH;
}

void set_side_chain_vol_on(S_TRACK *track)
{
    track->update |= S_UPDATE_SCHAIN_VOL;
}

void set_side_chain_vol_off(S_TRACK *track)
{
    track->update &= ~S_UPDATE_SCHAIN_VOL;
}

void jump(S_TRACK *track)
{
    int offset;

    offset = (short)read16(track);
    track->addr += offset;
}

void jump_cond(S_TRACK *track)
{
    int cond, offset;

    cond = read8(track);
    if (sound_player.cond != 0 && cond <= sound_player.cond)
    {
        offset = (short)read16(track);
        track->addr += offset;
        sound_player.prev_cond = cond;
    }
    else
    {
        track->addr += 2;
    }
}

void block_end(S_TRACK *track)
{
    sound_player.active_mask &= ~(1 << track->attr.voice_id) & S_VOICE_MASK;
    sound_player.noise_mask &= ~(1 << track->attr.voice_id) & S_VOICE_MASK;
    sound_player.reverb_mask &= ~(1 << track->attr.voice_id) & S_VOICE_MASK;
    sound_player.pitch_lfo_mask &= ~(1 << track->attr.voice_id) & S_VOICE_MASK;

    if (sound_player.active_mask == 0)
    {
        sound_player.music_id = 0;
    }

    if (track->update & S_UPDATE_OVERLAY)
    {
        sound_player.overlay_mask &= ~(1 << track->overlay_num);
    }

    if (track->update & S_UPDATE_ALTERNATIVE)
    {
        sound_player.alt_mask &= ~(1 << track->alt_num);
    }

    track->update = 0;
    sound_player.update |= S_UPDATE_NOISE_CLOCK;

    sound_flush_noise();
    sound_flush_reverb();
    sound_flush_pitch_lfo();
}

void no_op(S_TRACK *track)
{
    block_end(track);
}

/*---------------------------------------------------------------------------*/

typedef void (*S_OPCODE)(S_TRACK *);

static S_OPCODE opcodes[96] =
{
    block_end,
    load_instrument,
    set_note_length,
    set_master_vol,
    set_pitch_bend,
    set_octave,
    inc_octave,
    dec_octave,
    set_vol,
    move_vol,
    set_pan,
    move_pan,
    set_noise_clock,
    set_ar,
    set_dr,
    set_sl,
    set_dr_sl,
    set_sr,
    set_rr,
    reset_adsr,
    set_vibrato_on,
    set_vibrato_depth,
    set_vibrato_off,
    set_amode,
    set_tremolo_on,
    set_tremolo_depth,
    set_tremolo_off,
    set_smode,
    set_pan_lfo_on,
    set_pan_lfo_depth,
    set_pan_lfo_off,
    set_rmode,
    set_transpose,
    move_transpose,
    set_reverb_on,
    set_reverb_off,
    set_noise_on,
    set_noise_off,
    set_pitch_lfo_on,
    set_pitch_lfo_off,
    loop_start,
    loop_end,
    loop_repeat,
    set_effect_off,
    set_legato_on,
    set_legato_off,
    set_noise_switch_force,
    set_noise_switch,
    set_full_length_on,
    set_full_length_off,
    set_pitch_lfo_switch_force,
    set_pitch_lfo_switch,
    set_side_chain_on,
    set_side_chain_off,
    set_side_chain_vol_on,
    set_side_chain_vol_off,
    set_fine_tune,
    move_fine_tune,
    set_portamento_on,
    set_portamento_off,
    set_note_fix_length,
    move_vibrato_depth,
    move_tremolo_depth,
    move_pan_lfo_depth,
    no_op,
    no_op,
    no_op,
    no_op,
    no_op,
    no_op,
    no_op,
    no_op,
    set_tempo,
    move_tempo,
    set_reverb_depth,
    move_reverb_depth,
    set_drum_on,
    set_drum_off,
    jump,
    jump_cond,
    loop_jump,
    loop_leave,
    reset_instrument,
    set_mute,
    set_overlay_voice_on,
    set_overlay_voice_off,
    set_overlay_pan,
    move_overlay_pan,
    set_alt_voice_on,
    set_alt_voice_off,
    no_op,
    no_op,
    no_op,
    set_time_signature,
    set_measure,
    no_op
};

static u_char get_next_note(S_TRACK *track)
{
    u_char *addr;
    int loop_num, opcode, length;
    u_char lo, hi;

    addr = track->addr;
    loop_num = track->loop_num;

    while (1)
    {
        opcode = *addr;

        if (opcode < 0x9A)
        {
            if (opcode >= S_OP_REST)
            {
                track->portamento_time = 0;
                track->effect &= ~(S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH);
            }

            return opcode;
        }

        if (opcode < S_OP_BLOCK_END)
        {
            return S_OP_BLOCK_END;
        }

        length = opcode_param_length[opcode - S_OP_BLOCK_END];
        if (length != 0)
        {
            addr += length;
            continue;
        }

        addr++;

        switch (opcode)
        {
        case S_OP_LOOP_END:
            if (*addr++ == (track->loop_time[loop_num] + 1))
            {
                loop_num = (loop_num - 1) % 4;
            }
            else
            {
                addr = track->loop_addr[loop_num];
            }
            break;
        case S_OP_LOOP_REPEAT:
            addr = track->loop_addr[loop_num];
            break;
        case S_OP_LOOP_JUMP:
        case S_OP_LOOP_LEAVE:
            if (*addr++ == (track->loop_time[loop_num] + 1))
            {
                lo = *addr++;
                hi = *addr++;
                addr += (short)(lo | (hi << 8));
                loop_num = (loop_num - 1) % 4;
            }
            else
            {
                addr += 2;
            }
            break;
        case S_OP_EFFECT_OFF:
        case S_OP_LEGATO_OFF:
        case S_OP_FULL_LEN_OFF:
        case S_OP_PORTAMENTO_OFF:
            track->portamento_time = 0;
            track->effect &= ~(S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH);
            break;
        case S_OP_JUMP:
            lo = *addr++;
            hi = *addr++;
            addr += (short)(lo | (hi << 8));
            break;
        case S_OP_JUMP_COND:
            if (*addr++ <= sound_player.cond)
            {
                lo = *addr++;
                hi = *addr++;
                addr += (short)(lo | (hi << 8));
            }
            else
            {
                addr += 2;
            }
            break;
        default:
            track->portamento_time = 0;
            track->effect &= ~(S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH);
            return S_OP_BLOCK_END;
        }
    }
}

/*---------------------------------------------------------------------------*/

void sound_exec_sequence(S_TRACK *track)
{
    int key, next, length, octave, delta;
    u_int pitch;
    S_DRUM *drum;

    do
    {
        key = read8(track);
        if (key < S_OP_BLOCK_END) break;
        opcodes[key - S_OP_BLOCK_END](track);
    }
    while (key != S_OP_BLOCK_END);

    if (key == S_OP_BLOCK_END) return;

    next = get_next_note(track);

    if (track->fix_length != 0)
    {
        track->length = (track->fix_length << 8) + track->fix_length;
    }

    if (track->length & 0xFF)
    {
        if (next >= S_OP_REST)
        {
            track->length -= 0x200;
        }
        else if (next < S_OP_TIE && !(track->effect & (S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH)))
        {
            track->length -= 0x200;
        }
    }
    else
    {
        length = note_length[key % 11];
        if ((next < S_OP_TIE || next >= S_OP_REST) && !(track->effect & (S_EFFECT_LEGATO | S_EFFECT_FULL_LENGTH)))
        {
            length -= 0x200;
        }
        track->length = length;
    }

    track->base_length = track->length & 0xFF;

    if (key >= S_OP_REST)
    {
        track->portamento_time = 0;
        track->vibrato_pitch = 0;
        track->tremolo_vol = 0;
        track->effect &= ~S_EFFECT_LEGATO_PREV;
        return;
    }

    if (key < S_OP_TIE)
    {
        key /= 11;

        if (track->update & S_UPDATE_DRUM_MODE)
        {
            sound_player.on_mask |= 1 << track->attr.voice_id;

            drum = &track->drum_addr[key % 12];
            if (drum->instrument != track->instrument)
            {
                track->instrument = drum->instrument;
                track->attr.addr = sound_instruments[drum->instrument].addr;
                track->attr.loop_addr = sound_instruments[drum->instrument].loop_addr;
                track->attr.ar = sound_instruments[drum->instrument].ar;
                track->attr.dr = sound_instruments[drum->instrument].dr;
                track->attr.sl = sound_instruments[drum->instrument].sl;
                track->attr.sr = sound_instruments[drum->instrument].sr;
                track->attr.a_mode = sound_instruments[drum->instrument].a_mode;
                track->attr.s_mode = sound_instruments[drum->instrument].s_mode;

                if (track->update & S_UPDATE_ALTERNATIVE)
                {
                    track->attr.mask |= S_UPDATE_INSTR & ~(S_UPDATE_RMODE | S_UPDATE_RR);
                }
                else
                {
                    track->attr.rr = sound_instruments[drum->instrument].rr;
                    track->attr.r_mode = sound_instruments[drum->instrument].r_mode;
                    track->attr.mask |= S_UPDATE_INSTR;
                }
            }

            pitch = sound_instruments[drum->instrument].pitch[drum->key % 12];
            octave = drum->key / 12;

            if (octave >= 7)
            {
                pitch <<= octave - 6;
            }
            else if (octave < 6)
            {
                pitch >>= 6 - octave;
            }

            track->vol = (drum->vol1 + (drum->vol2 << 8)) << 16;
            track->pan = drum->pan << 8;
        }
        else
        {
            key += track->octave * 12;
            if (track->portamento_time != 0 && track->prev_key != 0)
            {
                track->pitch_bend_time = track->portamento_time;
                track->key_step = (key - track->prev_key) + (track->transpose - track->prev_transpose);
                track->key = track->prev_key - (track->transpose - track->prev_transpose);
                key = track->prev_key + track->prev_transpose;
            }
            else
            {
                track->key = key;
                key += track->transpose;
            }

            octave = key / 12;
            key %= 12;

            if (!(track->effect & S_EFFECT_LEGATO_PREV))
            {
                sound_player.on_mask |= 1 << track->attr.voice_id;

                if (track->update & S_UPDATE_OVERLAY)
                {
                    sound_player.on_mask |= 1 << track->overlay_num;
                }

                track->pitch_bend_tick = 0;
            }

            pitch = sound_instruments[track->instrument].pitch[key];
            if (octave > 6)
            {
                pitch <<= octave - 6;
            }
            else if (octave < 6)
            {
                pitch >>= 6 - octave;
            }
        }

        sound_player.keyed_mask |= 1 << track->attr.voice_id;
        track->attr.mask |= S_UPDATE_VOICE | S_UPDATE_PITCH;

        if (track->fine_tune != 0)
        {
            if (track->fine_tune > 0)
            {
                pitch += (pitch * track->fine_tune) >> 7;
            }
            else
            {
                pitch += (pitch * track->fine_tune) >> 8;
            }

            pitch &= 0xffff;
        }

        track->pitch = pitch;

        if (track->update & S_UPDATE_VIBRATO)
        {
            u_int depth;

            depth = (track->vibrato_depth & 0x7f00) >> 8;
            if (!(track->vibrato_depth & 0x8000)) pitch = (track->pitch * 15) >> 8;

            track->vibrato_base = (depth * pitch) >> 7;
            track->vibrato_wave = wave_table[track->vibrato_type];
            track->vibrato_delay_tick = track->vibrato_delay;
            track->vibrato_rate_tick = 1;
        }

        if (track->update & S_UPDATE_TREMOLO)
        {
            track->tremolo_wave = wave_table[track->tremolo_type];
            track->tremolo_delay_tick = track->tremolo_delay;
            track->tremolo_rate_tick = 1;
        }

        if (track->update & S_UPDATE_PAN_LFO)
        {
            track->pan_lfo_wave = wave_table[track->pan_lfo_type];
            track->pan_lfo_tick = 1;
        }

        track->vibrato_pitch = 0;
        track->tremolo_vol = 0;
        track->pitch_bend = 0;
    }

    track->effect = (track->effect & ~S_EFFECT_LEGATO_PREV) | ((track->effect & S_EFFECT_LEGATO) << 1);

    if (track->key_step != 0)
    {
        track->key += track->key_step;
        key = track->key + track->transpose;
        pitch = sound_instruments[track->instrument].pitch[key % 12];

        if (track->fine_tune != 0)
        {
            if (track->fine_tune > 0)
            {
                pitch += (pitch * track->fine_tune) >> 7;
            }
            else
            {
                pitch += (pitch * track->fine_tune) >> 8;
            }
        }

        pitch <<= 16;
        octave = key / 12;
        if (octave > 6)
        {
            pitch <<= octave - 6;
        }
        else if (octave < 6)
        {
            pitch >>= 6 - octave;
        }

        track->pitch_bend_tick = track->pitch_bend_time;

        delta = pitch - ((track->pitch << 16) + track->pitch_bend);
        track->pitch_bend_step = delta / track->pitch_bend_tick;
        track->key_step = 0;
    }

    track->prev_key = track->key;
    track->prev_transpose = track->transpose;
}
