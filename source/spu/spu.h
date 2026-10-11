#ifndef _SPU_H_
#define _SPU_H_

#include <stddef.h>

#define SPU_NCH                 24
#define SPU_VOICE_MASK          ((1 << SPU_NCH) - 1)

#define SPU_ADSR_LIN_INC        1
#define SPU_ADSR_LIN_DEC        3
#define SPU_ADSR_EXP_INC        5
#define SPU_ADSR_EXP_DEC        7

#define SPU_REV_MODE_OFF        0
#define SPU_REV_MODE_ROOM       1
#define SPU_REV_MODE_STUDIO_A   2
#define SPU_REV_MODE_STUDIO_B   3
#define SPU_REV_MODE_STUDIO_C   4
#define SPU_REV_MODE_HALL       5
#define SPU_REV_MODE_SPACE      6
#define SPU_REV_MODE_ECHO       7
#define SPU_REV_MODE_DELAY      8
#define SPU_REV_MODE_PIPE       9
#define SPU_REV_MODE_MAX        10

void spu_init(void);
void spu_quit(void);

void spu_step(int step_size, short *output);

void spu_set_reverb_enable(int enable);
void spu_set_reverb_mode(int mode);
void spu_set_reverb_depth(short l, short r);

void spu_set_reverb_on(unsigned int mask);
void spu_set_reverb_off(unsigned int mask);

void spu_reverb_clear(void);

void spu_set_noise_on(unsigned int mask);
void spu_set_noise_off(unsigned int mask);

void spu_set_pitch_lfo_on(unsigned int mask);
void spu_set_pitch_lfo_off(unsigned int mask);

void spu_write(unsigned int addr, void *ptr, unsigned int size);

void spu_set_voice_volume(int voice, unsigned short l, unsigned short r);
void spu_set_voice_pitch(int voice, unsigned short pitch);
void spu_set_voice_address(int voice, unsigned int addr);
void spu_set_voice_loop_address(int voice, unsigned int addr);

void spu_set_voice_attack(int voice, int mode, unsigned short rate);
void spu_set_voice_decay(int voice, unsigned short rate);
void spu_set_voice_sustain(int voice, int mode, unsigned short rate, unsigned short level);
void spu_set_voice_release(int voice, int mode, unsigned short rate);

void spu_set_key_on(unsigned int keys);
void spu_set_key_off(unsigned int keys);

void spu_set_master_volume(unsigned short l, unsigned short r);

void spu_set_noise_clock(int clock);

#endif /* _SPU_H_ */
