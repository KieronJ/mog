#ifndef _SOUND_H_
#define _SOUND_H_

#include "common.h"

/*---------------------------------------------------------------------------*/

#define S_NUM_VOICES    24
#define S_VOICE_MASK    ((1 << S_NUM_VOICES) - 1)

#define S_NUM_INSTR     128
#define S_INSTR2_OFFSET 53

#define S_PAN_CENTER    0x40
#define S_PAN_MAX       0x7f
#define S_VOL_MAX       0x7f

#define S_MASTER_VOL    0x3fff

#define S_MODE_STEREO   (1 << 0)
#define S_MODE_MONO     (1 << 1)
#define S_MODE_PAIR     (1 << 2)

#define S_EFFECT_LEGATO         (1 << 0)
#define S_EFFECT_LEGATO_PREV    (1 << 1)
#define S_EFFECT_FULL_LENGTH    (1 << 2)

#define S_CONTROL_PAUSE_UPDATE  (1 << 0)
#define S_CONTROL_DOUBLE_SPEED  (1 << 2)
#define S_CONTROL_REVERB_ENABLE (1 << 4)

#define S_UPDATE_VOLL   (1 << 0)
#define S_UPDATE_VOLR   (1 << 1)
#define S_UPDATE_PITCH  (1 << 4)
#define S_UPDATE_WDSA   (1 << 7)
#define S_UPDATE_AMODE  (1 << 8)
#define S_UPDATE_SMODE  (1 << 9)
#define S_UPDATE_RMODE  (1 << 10)
#define S_UPDATE_AR     (1 << 11)
#define S_UPDATE_DR     (1 << 12)
#define S_UPDATE_SR     (1 << 13)
#define S_UPDATE_RR     (1 << 14)
#define S_UPDATE_SL     (1 << 15)
#define S_UPDATE_LSAX   (1 << 16)

#define S_UPDATE_VOICE  (S_UPDATE_VOLL | S_UPDATE_VOLR)
#define S_UPDATE_ADSR   (S_UPDATE_AMODE | S_UPDATE_SMODE | S_UPDATE_RMODE \
                        | S_UPDATE_AR | S_UPDATE_DR | S_UPDATE_SR \
                        | S_UPDATE_RR | S_UPDATE_SL)
#define S_UPDATE_INSTR  (S_UPDATE_WDSA | S_UPDATE_ADSR | S_UPDATE_LSAX)
#define S_UPDATE_ALL    (S_UPDATE_VOICE | S_UPDATE_PITCH | S_UPDATE_INSTR )

/* S_TRACK.update */
#define S_UPDATE_VIBRATO        (1 << 0)
#define S_UPDATE_TREMOLO        (1 << 1)
#define S_UPDATE_PAN_LFO        (1 << 2)
#define S_UPDATE_DRUM_MODE      (1 << 3)
#define S_UPDATE_SCHAIN_PITCH   (1 << 4)
#define S_UPDATE_SCHAIN_VOL     (1 << 5)
#define S_UPDATE_OVERLAY        (1 << 7)
#define S_UPDATE_ALTERNATIVE    (1 << 8)
#define S_UPDATE_FLUSH_ALT      (1 << 10)

#define S_UPDATE_LFO (S_UPDATE_VIBRATO | S_UPDATE_TREMOLO | S_UPDATE_PAN_LFO \
                     | S_UPDATE_SCHAIN_PITCH | S_UPDATE_SCHAIN_VOL)

/* S_PLAYER.update */
#define S_UPDATE_NOISE_CLOCK    (1 << 4)
#define S_UPDATE_REVERB         (1 << 6)

#define S_MUSIC_BUFFER_SIZE (12 * 1024)

#define S_DEFAULT_SOUND_ADDR 0x76fe0

#define S_NUM_MAGIC 4

#define S_OP_TIE            0x84
#define S_OP_REST           0x8f
#define S_OP_BLOCK_END      0xa0
#define S_OP_LOOP_END       0xc9
#define S_OP_LOOP_REPEAT    0xca
#define S_OP_EFFECT_OFF     0xcb
#define S_OP_LEGATO_OFF     0xcd
#define S_OP_FULL_LEN_OFF   0xd1
#define S_OP_PORTAMENTO_OFF 0xdb
#define S_OP_JUMP           0xee
#define S_OP_JUMP_COND      0xef
#define S_OP_LOOP_JUMP      0xf0
#define S_OP_LOOP_LEAVE     0xf1

#define S_SAMPLE_RATE   44100
#define S_SEQ_HZ        240
#define S_ACCUM         ((float)S_SAMPLE_RATE/S_SEQ_HZ)
#define S_BUF_SIZE      ((S_SAMPLE_RATE/S_SEQ_HZ) + 1)

#define S_SEQ_ID_LB2    82 /* One-Winged Angel */

/*---------------------------------------------------------------------------*/

typedef struct {
    u_int   mode;
    u_int   active_mask;
    u_int   on_mask;
    u_int   keyed_mask;
    u_int   off_mask;
    u_int   tempo;
    int     tempo_step;
    u_int   tempo_delta;
    u_int   overlay_mask;
    u_int   alt_mask;
    u_int   noise_mask;
    u_int   reverb_mask;
    u_int   pitch_lfo_mask;
    u_int   update;
    int     reverb_mode;
    u_int   reverb_depth;
    int     reverb_depth_step;
    u_short tempo_time;
    u_short music_id;
    u_short prev_cond;
    u_short cond;
    u_short reverb_depth_time;
    u_short noise_clock;
    u_short mute;
    u_short num_beats;
    u_short beat;
    u_short num_ticks;
    u_short tick;
    u_short measure;
} S_PLAYER;

typedef struct {
    u_char  instrument;
    u_char  key;
    u_char  vol1;
    u_char  vol2;
    u_char  pan;
} S_DRUM;

typedef struct {
    u_int   voice_id;
    u_int   mask;
    u_int   addr;
    u_int   loop_addr;
    int     a_mode;
    int     s_mode;
    int     r_mode;
    u_short pitch;
    u_short ar;
    u_short dr;
    u_short sl;
    short   sr;
    u_short rr;
    short   vol_l;
    short   vol_r;
} S_ATTR;

typedef struct {
    u_char  *addr;
    u_char  *loop_addr[4];
    S_DRUM  *drum_addr;
    short   *vibrato_wave;
    short   *tremolo_wave;
    short   *pan_lfo_wave;
    u_int   overlay_num;
    u_int   alt_num;
    u_int   master_vol;
    u_int   pitch;
    u_int   pitch_bend;
    u_int   update;
    int     vol;
    int     vol_step;
    int     pitch_bend_step;
    u_short length;
    u_short instrument;
    u_short vol_time;
    u_short overlay_pan_time;
    u_short pan;
    u_short pan_time;
    u_short pitch_bend_tick;
    u_short octave;
    u_short pitch_bend_time;
    u_short prev_key;
    u_short portamento_time;
    u_short effect;
    u_short vibrato_delay;
    u_short vibrato_delay_tick;
    u_short vibrato_rate;
    u_short vibrato_rate_tick;
    u_short vibrato_type;
    u_short vibrato_base;
    u_short vibrato_depth;
    u_short vibrato_depth_time;
    short   vibrato_depth_step;
    u_short tremolo_delay;
    u_short tremolo_delay_tick;
    u_short tremolo_rate;
    u_short tremolo_rate_tick;
    u_short tremolo_type;
    u_short tremolo_depth;
    u_short tremolo_depth_time;
    short   tremolo_depth_step;
    u_short pan_lfo_rate;
    u_short pan_lfo_tick;
    u_short pan_lfo_type;
    u_short pan_lfo_depth;
    u_short pan_lfo_time;
    short   pan_lfo_step;
    u_short noise_time;
    u_short pitch_lfo_time;
    u_short loop_num;
    u_short loop_time[4];
    short   base_length;
    short   fix_length;
    short   overlay_pan;
    short   overlay_pan_step;
    short   pan_step;
    u_short transpose;
    short   fine_tune;
    u_short key;
    short   key_step;
    u_short prev_transpose;
    short   vibrato_pitch;
    short   tremolo_vol;
    short   pan_lfo_vol;
    S_ATTR  attr;
} S_TRACK;

typedef struct {
    u_int   addr;
    u_int   loop_addr;
    u_char  ar;
    u_char  dr;
    u_char  sl;
    u_char  sr;
    u_char  rr;
    u_char  a_mode;
    u_char  s_mode;
    u_char  r_mode;
    int     pitch[12];
} S_INSTRUMENT;

typedef struct {
    u_int   addr;
    u_int   size;
    u_char  unused[8];
    u_char  data[0];
} S_WAVE;

typedef struct {
    char    magic[S_NUM_MAGIC];
    u_short id;
    u_short size;
    u_short reverb_mode;
    u_char  date[3];
    u_char  time[3];
    u_char  data[0];
} S_SEQUENCE;

/*---------------------------------------------------------------------------*/

extern S_PLAYER sound_player;
extern S_TRACK sound_tracks[S_NUM_VOICES];
extern S_INSTRUMENT sound_instruments[S_NUM_INSTR];

extern int sound_debug;
extern u_int sound_control_flags;
extern u_int sound_mute_mask;

extern int sound_reverb_volume;
extern u_short sound_reverb_pan;

extern int sound_volume;
extern int sound_volume_step;
extern u_short sound_volume_rate;

extern int sound_tempo;
extern int sound_tempo_step;
extern u_short sound_tempo_rate;

extern int sound_pitch;
extern int sound_pitch_step;
extern u_short sound_pitch_rate;

/*---------------------------------------------------------------------------*/

/* init.c */
int sound_start(int debug);
void sound_stop(void);
void *sound_load_file_data(const char *filename);
int sound_load_instr(void);
int sound_load_instr2(void);

/* driver.c */
int sound_main(void **buffer, size_t *size);
void sound_start_player(void);
void sound_stop_player(void);
void sound_init_instrument(S_TRACK *track, int instrument);
void sound_update_volume(void);
void sound_flush(int voice, S_ATTR *attr);
void sound_flush_noise(void);
void sound_flush_reverb(void);
void sound_flush_pitch_lfo(void);
void sound_set_reverb_mode(int mode);
void sound_copy_music(void *data, u_int size);

/* command.c */
int sound_play_music(const char *filename);
void sound_set_volume(int volume);
void sound_move_volume(int time, int target);
void sound_move_volume_range(int time, int volume, int target);
void sound_set_tempo(int tempo);
void sound_move_tempo(int time, int target);
void sound_move_tempo_range(int time, int tempo, int target);
void sound_set_pitch(int pitch);
void sound_move_pitch(int time, int pitch);
void sound_move_pitch_range(int time, int pitch, int target);
void sound_stop_music(void);
void sound_set_mono(void);
void sound_set_stereo(void);
void sound_set_pair(void);
void sound_set_mute(int mute);
void sound_set_condition(u_short cond);
void sound_pause(void);
void sound_set_reverb_pan(int pan);
void sound_set_reverb_volume(int volume);

/* sequence.c */
void sound_exec_sequence(S_TRACK *track);

#endif /* _SOUND_H_ */
