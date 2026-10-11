#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "spu.h"

#define ADPCM_LOOP_END      0x100
#define ADPCM_LOOP          0x200
#define ADPCM_LOOP_START    0x400

#define BLOCK_SIZE          28
#define PREV_SAMPLES        3
#define BLOCK_SAMPLES       (BLOCK_SIZE + PREV_SAMPLES)

enum {
    STEP_ATTACK,
    STEP_DECAY,
    STEP_SUSTAIN,
    STEP_RELEASE,
    STEP_OFF,
};

typedef struct voice {
    short voll, volr;   /* volume left/right */
    unsigned short pitch;
    unsigned int   ssa; /* sound start address */
    int a_mode;         /* attack mode */
    int ar;             /* attack rate */
    int dr;             /* decay rate */
    int s_mode;         /* sustain mode */
    int sr;             /* sustain rate */
    int sl;             /* sustain level */
    int r_mode;         /* release mode */
    int rr;             /* release rate */

    int step;           /* adsr step */

    int env;            /* envelope volume */
    int env_counter;
    int env_rate;
    int env_increasing;
    int env_exponential;
    int env_step;
    int env_increment;

    short outx;

    int pitch_counter;

    unsigned int addr; /* sample address */
    unsigned int lsa;  /* loop start address */

    short block_samples[BLOCK_SAMPLES];
    unsigned short block_header;
    int has_block;

    short adpcm_hist[2];
} voice;

typedef struct reverb_attr {
    unsigned short apfd1;
    unsigned short apfd2;
    short iir;
    short combv1;
    short combv2;
    short combv3;
    short combv4;
    short wall;
    short apfv1;
    short apfv2;
    unsigned short samem[2];
    unsigned short combm1[2];
    unsigned short combm2[2];
    unsigned short samed[2];
    unsigned short diffm[2];
    unsigned short combm3[2];
    unsigned short combm4[2];
    unsigned short diffd[2];
    unsigned short apf1[2];
    unsigned short apf2[2];
    short in[2];
} reverb_attr;

static short mvol[2]; /* master volume left/right */
static short rvol[2]; /* reverb volume left/right */

static unsigned int vmixr; /* voice mix reverb */
static unsigned int non;   /* noise mask */
static unsigned int pmon;  /* pitch modulation mask */

static int ren;            /* reverb enable */
static unsigned int rsize; /* reverb work area size */
static reverb_attr rattr;  /* reverb attributes */
static unsigned int raddr; /* reverb index */

static short last_rev[2];

static short reverb_downsample_buffer[2][128];
static short reverb_upsample_buffer[2][64];
static int reverb_filter_index;

static voice voices[SPU_NCH];

static unsigned short waveform_data[262144];
static unsigned short reverb_work_area[49184]; /* based on max size */

static size_t output_index;

static int adpcm_filters[][2] =
{
    {0, 0},
    {60, 0},
    {115, -52},
    {98, -55},
    {122, -60},
};

/* windowed sinc with a blackman window */
static short interp_table[] =
{
    -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001,
    -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0002, 0x0002, 0x0002, 0x0003, 0x0003,
    0x0003, 0x0004, 0x0004, 0x0005, 0x0005, 0x0006, 0x0007, 0x0007,
    0x0008, 0x0009, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e,
    0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0015, 0x0016, 0x0018,
    0x0019, 0x001b, 0x001c, 0x001e, 0x0020, 0x0021, 0x0023, 0x0025,
    0x0027, 0x0029, 0x002c, 0x002e, 0x0030, 0x0033, 0x0035, 0x0038,
    0x003a, 0x003d, 0x0040, 0x0043, 0x0046, 0x0049, 0x004d, 0x0050,
    0x0054, 0x0057, 0x005b, 0x005f, 0x0063, 0x0067, 0x006b, 0x006f,
    0x0074, 0x0078, 0x007d, 0x0082, 0x0087, 0x008c, 0x0091, 0x0096,
    0x009c, 0x00a1, 0x00a7, 0x00ad, 0x00b3, 0x00ba, 0x00c0, 0x00c7,
    0x00cd, 0x00d4, 0x00db, 0x00e3, 0x00ea, 0x00f2, 0x00fa, 0x0101,
    0x010a, 0x0112, 0x011b, 0x0123, 0x012c, 0x0135, 0x013f, 0x0148,
    0x0152, 0x015c, 0x0166, 0x0171, 0x017b, 0x0186, 0x0191, 0x019c,
    0x01a8, 0x01b4, 0x01c0, 0x01cc, 0x01d9, 0x01e5, 0x01f2, 0x0200,
    0x020d, 0x021b, 0x0229, 0x0237, 0x0246, 0x0255, 0x0264, 0x0273,
    0x0283, 0x0293, 0x02a3, 0x02b4, 0x02c4, 0x02d6, 0x02e7, 0x02f9,
    0x030b, 0x031d, 0x0330, 0x0343, 0x0356, 0x036a, 0x037e, 0x0392,
    0x03a7, 0x03bc, 0x03d1, 0x03e7, 0x03fc, 0x0413, 0x042a, 0x0441,
    0x0458, 0x0470, 0x0488, 0x04a0, 0x04b9, 0x04d2, 0x04ec, 0x0506,
    0x0520, 0x053b, 0x0556, 0x0572, 0x058e, 0x05aa, 0x05c7, 0x05e4,
    0x0601, 0x061f, 0x063e, 0x065c, 0x067c, 0x069b, 0x06bb, 0x06dc,
    0x06fd, 0x071e, 0x0740, 0x0762, 0x0784, 0x07a7, 0x07cb, 0x07ef,
    0x0813, 0x0838, 0x085d, 0x0883, 0x08a9, 0x08d0, 0x08f7, 0x091e,
    0x0946, 0x096f, 0x0998, 0x09c1, 0x09eb, 0x0a16, 0x0a40, 0x0a6c,
    0x0a98, 0x0ac4, 0x0af1, 0x0b1e, 0x0b4c, 0x0b7a, 0x0ba9, 0x0bd8,
    0x0c07, 0x0c38, 0x0c68, 0x0c99, 0x0ccb, 0x0cfd, 0x0d30, 0x0d63,
    0x0d97, 0x0dcb, 0x0e00, 0x0e35, 0x0e6b, 0x0ea1, 0x0ed7, 0x0f0f,
    0x0f46, 0x0f7f, 0x0fb7, 0x0ff1, 0x102a, 0x1065, 0x109f, 0x10db,
    0x1116, 0x1153, 0x118f, 0x11cd, 0x120b, 0x1249, 0x1288, 0x12c7,
    0x1307, 0x1347, 0x1388, 0x13c9, 0x140b, 0x144d, 0x1490, 0x14d4,
    0x1517, 0x155c, 0x15a0, 0x15e6, 0x162c, 0x1672, 0x16b9, 0x1700,
    0x1747, 0x1790, 0x17d8, 0x1821, 0x186b, 0x18b5, 0x1900, 0x194b,
    0x1996, 0x19e2, 0x1a2e, 0x1a7b, 0x1ac8, 0x1b16, 0x1b64, 0x1bb3,
    0x1c02, 0x1c51, 0x1ca1, 0x1cf1, 0x1d42, 0x1d93, 0x1de5, 0x1e37,
    0x1e89, 0x1edc, 0x1f2f, 0x1f82, 0x1fd6, 0x202a, 0x207f, 0x20d4,
    0x2129, 0x217f, 0x21d5, 0x222c, 0x2282, 0x22da, 0x2331, 0x2389,
    0x23e1, 0x2439, 0x2492, 0x24eb, 0x2545, 0x259e, 0x25f8, 0x2653,
    0x26ad, 0x2708, 0x2763, 0x27be, 0x281a, 0x2876, 0x28d2, 0x292e,
    0x298b, 0x29e7, 0x2a44, 0x2aa1, 0x2aff, 0x2b5c, 0x2bba, 0x2c18,
    0x2c76, 0x2cd4, 0x2d33, 0x2d91, 0x2df0, 0x2e4f, 0x2eae, 0x2f0d,
    0x2f6c, 0x2fcc, 0x302b, 0x308b, 0x30ea, 0x314a, 0x31aa, 0x3209,
    0x3269, 0x32c9, 0x3329, 0x3389, 0x33e9, 0x3449, 0x34a9, 0x3509,
    0x3569, 0x35c9, 0x3629, 0x3689, 0x36e8, 0x3748, 0x37a8, 0x3807,
    0x3867, 0x38c6, 0x3926, 0x3985, 0x39e4, 0x3a43, 0x3aa2, 0x3b00,
    0x3b5f, 0x3bbd, 0x3c1b, 0x3c79, 0x3cd7, 0x3d35, 0x3d92, 0x3def,
    0x3e4c, 0x3ea9, 0x3f05, 0x3f62, 0x3fbd, 0x4019, 0x4074, 0x40d0,
    0x412a, 0x4185, 0x41df, 0x4239, 0x4292, 0x42eb, 0x4344, 0x439c,
    0x43f4, 0x444c, 0x44a3, 0x44fa, 0x4550, 0x45a6, 0x45fc, 0x4651,
    0x46a6, 0x46fa, 0x474e, 0x47a1, 0x47f4, 0x4846, 0x4898, 0x48e9,
    0x493a, 0x498a, 0x49d9, 0x4a29, 0x4a77, 0x4ac5, 0x4b13, 0x4b5f,
    0x4bac, 0x4bf7, 0x4c42, 0x4c8d, 0x4cd7, 0x4d20, 0x4d68, 0x4db0,
    0x4df7, 0x4e3e, 0x4e84, 0x4ec9, 0x4f0e, 0x4f52, 0x4f95, 0x4fd7,
    0x5019, 0x505a, 0x509a, 0x50da, 0x5118, 0x5156, 0x5194, 0x51d0,
    0x520c, 0x5247, 0x5281, 0x52ba, 0x52f3, 0x532a, 0x5361, 0x5397,
    0x53cc, 0x5401, 0x5434, 0x5467, 0x5499, 0x54ca, 0x54fa, 0x5529,
    0x5558, 0x5585, 0x55b2, 0x55de, 0x5609, 0x5632, 0x565b, 0x5684,
    0x56ab, 0x56d1, 0x56f6, 0x571b, 0x573e, 0x5761, 0x5782, 0x57a3,
    0x57c3, 0x57e2, 0x57ff, 0x581c, 0x5838, 0x5853, 0x586d, 0x5886,
    0x589e, 0x58b5, 0x58cb, 0x58e0, 0x58f4, 0x5907, 0x5919, 0x592a,
    0x593a, 0x5949, 0x5958, 0x5965, 0x5971, 0x597c, 0x5986, 0x598f,
    0x5997, 0x599e, 0x59a4, 0x59a9, 0x59ad, 0x59b0, 0x59b2, 0x59b3,
};

static unsigned short reverb_mode_attr[SPU_REV_MODE_MAX][32] =
{
    /* off */
    {
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    },
    /* room */
    {
        0x007D, 0x005B, 0x6D80, 0x54B8, 0xBED0, 0x0000, 0x0000, 0xBA80,
        0x5800, 0x5300, 0x04D6, 0x0333, 0x03F0, 0x0227, 0x0374, 0x01EF,
        0x0334, 0x01B5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x01B4, 0x0136, 0x00B8, 0x005C, 0x8000, 0x8000
    },
    /* studio a */
    {
        0x0033, 0x0025, 0x70F0, 0x4FA8, 0xBCE0, 0x4410, 0xC0F0, 0x9C00,
        0x5280, 0x4EC0, 0x03E4, 0x031B, 0x03A4, 0x02AF, 0x0372, 0x0266,
        0x031C, 0x025D, 0x025C, 0x018E, 0x022F, 0x0135, 0x01D2, 0x00B7,
        0x018F, 0x00B5, 0x00B4, 0x0080, 0x004C, 0x0026, 0x8000, 0x8000
    },
    /* studio b */
    {
        0x00B1, 0x007F, 0x70F0, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0xB4C0,
        0x5280, 0x4EC0, 0x0904, 0x076B, 0x0824, 0x065F, 0x07A2, 0x0616,
        0x076C, 0x05ED, 0x05EC, 0x042E, 0x050F, 0x0305, 0x0462, 0x02B7,
        0x042F, 0x0265, 0x0264, 0x01B2, 0x0100, 0x0080, 0x8000, 0x8000
    },
    /* studio c */
    {
        0x00E3, 0x00A9, 0x6F60, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0xA680,
        0x5680, 0x52C0, 0x0DFB, 0x0B58, 0x0D09, 0x0A3C, 0x0BD9, 0x0973,
        0x0B59, 0x08DA, 0x08D9, 0x05E9, 0x07EC, 0x04B0, 0x06EF, 0x03D2,
        0x05EA, 0x031D, 0x031C, 0x0238, 0x0154, 0x00AA, 0x8000, 0x8000
    },
    /* hall */
    {
        0x01A5, 0x0139, 0x6000, 0x5000, 0x4C00, 0xB800, 0xBC00, 0xC000,
        0x6000, 0x5C00, 0x15BA, 0x11BB, 0x14C2, 0x10BD, 0x11BC, 0x0DC1,
        0x11C0, 0x0DC3, 0x0DC0, 0x09C1, 0x0BC4, 0x07C1, 0x0A00, 0x06CD,
        0x09C2, 0x05C1, 0x05C0, 0x041A, 0x0274, 0x013A, 0x8000, 0x8000
    },
    /* space */
    {
        0x033D, 0x0231, 0x7E00, 0x5000, 0xB400, 0xB000, 0x4C00, 0xB000,
        0x6000, 0x5400, 0x1ED6, 0x1A31, 0x1D14, 0x183B, 0x1BC2, 0x16B2,
        0x1A32, 0x15EF, 0x15EE, 0x1055, 0x1334, 0x0F2D, 0x11F6, 0x0C5D,
        0x1056, 0x0AE1, 0x0AE0, 0x07A2, 0x0464, 0x0232, 0x8000, 0x8000
    },
    /* echo */
    {
        0x0001, 0x0001, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x8100,
        0x0000, 0x0000, 0x1FFF, 0x0FFF, 0x1005, 0x0005, 0x0000, 0x0000,
        0x1005, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x1004, 0x1002, 0x0004, 0x0002, 0x8000, 0x8000
    },
    /* delay */
    {
        0x0001, 0x0001, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x1FFF, 0x0FFF, 0x1005, 0x0005, 0x0000, 0x0000,
        0x1005, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x1004, 0x1002, 0x0004, 0x0002, 0x8000, 0x8000
    },
    /* pipe */
    {
        0x0017, 0x0013, 0x70F0, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0x8500,
        0x5F80, 0x54C0, 0x0371, 0x02AF, 0x02E5, 0x01DF, 0x02B0, 0x01D7,
        0x0358, 0x026A, 0x01D6, 0x011E, 0x012D, 0x00B1, 0x011F, 0x0059,
        0x01A0, 0x00E3, 0x0058, 0x0040, 0x0028, 0x0014, 0x8000, 0x8000
    },
};

static unsigned int reverb_work_area_size[SPU_REV_MODE_MAX] =
{
    64,    /* off */
    4960,  /* room */
    4000,  /* studio a */
    9248,  /* studio b */
    14320, /* studio c */
    22256, /* hall */
    31584, /* space */
    49184, /* echo */
    49184, /* delay */
    7680   /* pipe */
};

static short reverb_fir_coeffs[] =
{
    -1, 2, -10, 35, -103, 266, -616, 1332, -2960, 10246,
    10246, -2960, 1332, -616, 266, -103, 35, -10, 2, -1,
};

static void set_adsr(voice *v)
{
    int rate;
    int clamp;

    int increasing;
    int exponential;
    int step;
    int increment;

    switch (v->step)
    {
    case STEP_ATTACK:
        rate = v->ar;
        clamp = v->ar != 0x7f;
        increasing = 1;
        exponential = v->a_mode == SPU_ADSR_EXP_INC;
        break;

    case STEP_DECAY:
        rate = v->dr << 2;
        clamp = 1; /* decay always clamps */
        increasing = 0;
        exponential = 1;
        break;

    case STEP_SUSTAIN:
        rate = v->sr;
        clamp = v->sr != 0x7f;
        increasing = v->s_mode == SPU_ADSR_LIN_INC || v->s_mode == SPU_ADSR_EXP_INC;
        exponential = v->s_mode == SPU_ADSR_EXP_INC || v->s_mode == SPU_ADSR_EXP_DEC;
        break;

    case STEP_RELEASE:
    case STEP_OFF:
        rate = v->rr << 2;
        clamp = v->rr != 0x1f;
        increasing = 0;
        exponential = v->r_mode == SPU_ADSR_EXP_DEC;
        break;

    default:
        assert(0); /* unreachable */
        break;
    }

    step = 7 - (rate & 0x3);
    step = increasing ? step : ~step;

    increment = 0x8000;

    if (rate < 44)
    {
        step = (unsigned int)step << (11 - (rate >> 2));
    }
    else if (rate >= 48)
    {
        increment >>= (rate >> 2) - 11;

        if (clamp)
        {
            increment = MAX(increment, 1);
        }
    }

    v->env_counter = 0;
    v->env_rate = rate;
    v->env_increasing = increasing;
    v->env_exponential = exponential;
    v->env_step = step;
    v->env_increment = increment;
}

static void tick_adsr(voice *v)
{
    int step = v->env_step;
    int increment = v->env_increment;

    if (v->env_exponential)
    {
        if (v->env_increasing)
        {
            if (v->env >= 0x6000)
            {
                if (v->env_rate < 40)
                {
                    step >>= 2;
                }
                else if (v->env_rate >= 44)
                {
                    increment >>= 2;
                }
                else
                {
                    step >>= 1;
                    increment >>= 1;
                }
            }
        }
        else
        {
            step = (step * v->env) >> 15;
        }
    }

    v->env_counter += increment;
    if (!(v->env_counter & 0x8000))
    {
        return;
    }

    v->env_counter = 0;
    v->env += step;

    if (v->env_increasing)
    {
        v->env = CLAMP(v->env, -0x8000, 0x7fff);
    }
    else
    {
        v->env = CLAMP(v->env, 0, 0x7fff);
    }

    if (v->step == STEP_ATTACK && v->env == 0x7fff)
    {
        v->step = STEP_DECAY;
        set_adsr(v);
    }
    else if (v->step == STEP_DECAY && v->env <= ((v->sl + 1) * 0x800))
    {
        v->step = STEP_SUSTAIN;
        set_adsr(v);
    }
    else if (v->step == STEP_RELEASE && v->env == 0)
    {
        v->step = STEP_OFF;
        set_adsr(v);
    }
}

static short saturate_sample(int value)
{
    return CLAMP(value, -0x8000, 0x7fff);
}

static void load_block(voice *v)
{
    int addr;
    unsigned short header;
    int shift;
    int filter;
    int filt0, filt1;
    unsigned short word;
    int sample;

    addr = v->addr * 4;
    header = waveform_data[addr++];
    shift = header & 0xf;
    filter = (header >> 4) & 0xf;

    assert(shift <= 12);
    assert(filter <= 4);

    v->block_header = header;

    if (header & ADPCM_LOOP_START)
    {
        v->lsa = v->addr;
    }

    filt0 = adpcm_filters[filter][0];
    filt1 = adpcm_filters[filter][1];

    // Save the last 3 samples from the previous block for interpolation
    v->block_samples[0] = v->block_samples[BLOCK_SAMPLES - 3];
    v->block_samples[1] = v->block_samples[BLOCK_SAMPLES - 2];
    v->block_samples[2] = v->block_samples[BLOCK_SAMPLES - 1];

    for (int w = 0; w < 7; w++)
    {
        word = waveform_data[addr++];

        for (int i = 0; i < 4; i++)
        {
            sample = (short)(word << 12);
            sample >>= shift;

            word >>= 4;

            sample += (v->adpcm_hist[0] * filt0) >> 6;
            sample += (v->adpcm_hist[1] * filt1) >> 6;
            sample = saturate_sample(sample);

            v->adpcm_hist[1] = v->adpcm_hist[0];
            v->adpcm_hist[0] = sample;

            v->block_samples[PREV_SAMPLES + w * 4 + i] = sample;
        }
    }
}

static short sample_voice(voice *v)
{
    int sample;
    int interp;
    int output;

    if (!v->has_block)
    {
        load_block(v);
        v->has_block = 1;
    }

    sample = (v->pitch_counter >> 12) + PREV_SAMPLES;
    interp = (v->pitch_counter >> 4) & 0xff;

    output  = interp_table[255 - interp] * v->block_samples[sample - 3];
    output += interp_table[511 - interp] * v->block_samples[sample - 2];
    output += interp_table[256 + interp] * v->block_samples[sample - 1];
    output += interp_table[interp] * v->block_samples[sample];

    return output >> 15;
}

static void update_pitch(voice *v, int i)
{
    int pitch;
    int factor;

    pitch = v->pitch;
    if (i > 0 && (pmon & (1u << i)))
    {
        factor = voices[i - 1].outx + 0x8000;
        pitch = ((short)pitch * factor) >> 15;
        pitch &= 0xffff;
    }

    v->pitch_counter += MIN(pitch, 0x4000);
    if (v->pitch_counter >= 0x1c000)
    {
        v->pitch_counter -= 0x1c000;

        v->addr += 2;
        v->has_block = 0;

        if (v->block_header & ADPCM_LOOP_END)
        {
            v->addr = v->lsa;

            if (!(v->block_header & ADPCM_LOOP))
            {
                v->step = STEP_OFF;
                v->env = 0;
                set_adsr(v);
            }
        }
    }
}

static int apply_volume(short sample, short volume)
{
    return (sample * volume) >> 15;
}

static void tick_voice(voice *v, int i, int *l, int *r)
{
    int sample;

    tick_adsr(v);

    sample = sample_voice(v);
    sample = apply_volume(sample, v->env);
    v->outx = saturate_sample(sample);

    *l = apply_volume(sample, v->voll << 1);
    *r = apply_volume(sample, v->volr << 1);

   update_pitch(v, i);
}

static short reverb_read(unsigned int addr)
{
    return reverb_work_area[(raddr + addr) % rsize];
}

static void reverb_write(unsigned int addr, short value)
{
    reverb_work_area[(raddr + addr) % rsize] = value;
}

static short reverb_downsample(const short *samples)
{
    int sum = 0;

    for (size_t i = 0; i < 20; i++)
    {
        sum += reverb_fir_coeffs[i] * samples[i * 2];
    }

    sum += 0x4000 * samples[19];

    return saturate_sample(sum >> 15);
}

static short reverb_upsample(const short *samples)
{
    int sum = 0;

    for (size_t i = 0; i < 20; i++)
    {
        sum += reverb_fir_coeffs[i] * samples[i];
    }

    return saturate_sample(sum >> 14);
}

static void process_reverb(int l, int r)
{
    short sample;
    short iir_input_a;
    short iir_input_b;
    short iir_a;
    short iir_b;
    int acc;
    short fb_a;
    short fb_b;
    short mda;
    short mdb;
    short ivb;

    l = saturate_sample(l);
    reverb_downsample_buffer[0][reverb_filter_index] = l;
    reverb_downsample_buffer[0][reverb_filter_index + 64] = l;

    r = saturate_sample(r);
    reverb_downsample_buffer[1][reverb_filter_index] = r;
    reverb_downsample_buffer[1][reverb_filter_index + 64] = r;

    if (reverb_filter_index & 0x1)
    {
        for (int i = 0; i < 2; i++)
        {
            /* downsample the input to 22.05 kHz */
            sample = reverb_downsample(&reverb_downsample_buffer[i][(reverb_filter_index - 38) & 0x3f]);

            iir_input_a = saturate_sample((((reverb_read(rattr.samed[i ^ 0] * 4) * rattr.wall) >> 14) + ((sample * rattr.in[i]) >> 14)) >> 1);
            iir_input_b = saturate_sample((((reverb_read(rattr.diffd[i ^ 1] * 4) * rattr.wall) >> 14) + ((sample * rattr.in[i]) >> 14)) >> 1);
            iir_a = saturate_sample((((iir_input_a * rattr.iir) >> 14) + (((32768 - rattr.iir) * reverb_read(rattr.samem[i] * 4 - 1)) >> 14)) >> 1);
            iir_b = saturate_sample((((iir_input_b * rattr.iir) >> 14) + (((32768 - rattr.iir) * reverb_read(rattr.diffm[i] * 4 - 1)) >> 14)) >> 1);

            reverb_write(rattr.samem[i] * 4, iir_a);
            reverb_write(rattr.diffm[i] * 4, iir_b);

            acc = ((reverb_read(rattr.combm1[i] * 4) * rattr.combv1) >> 14) +
                  ((reverb_read(rattr.combm2[i] * 4) * rattr.combv2) >> 14) +
                  ((reverb_read(rattr.combm3[i] * 4) * rattr.combv3) >> 14) +
                  ((reverb_read(rattr.combm4[i] * 4) * rattr.combv4) >> 14);

            fb_a = reverb_read((rattr.apf1[i] - rattr.apfd1) * 4);
            fb_b = reverb_read((rattr.apf2[i] - rattr.apfd2) * 4);
            mda = saturate_sample((acc + ((fb_a * -rattr.apfv1) >> 14)) >> 1);
            mdb = saturate_sample(fb_a + ((((mda * rattr.apfv1) >> 14) + ((fb_b * -rattr.apfv2) >> 14)) >> 1));
            ivb = saturate_sample(fb_b + ((mdb * rattr.apfv2) >> 15));

            reverb_write(rattr.apf1[i] * 4, mda);
            reverb_write(rattr.apf2[i] * 4, mdb);

            reverb_upsample_buffer[i][reverb_filter_index >> 1] = ivb;
            reverb_upsample_buffer[i][(reverb_filter_index >> 1) + 32] = ivb;

            /* upsample the output to 44.1 kHz */
            last_rev[i] = reverb_upsample(&reverb_upsample_buffer[i][((reverb_filter_index >> 1) - 19) & 0x1f]);
        }

        /* increment the reverb index */
        raddr = (raddr + 1) % rsize;
    }
    else
    {
        last_rev[0] = reverb_upsample_buffer[0][((reverb_filter_index >> 1) - 10) & 0x1f];
        last_rev[1] = reverb_upsample_buffer[1][((reverb_filter_index >> 1) - 10) & 0x1f];
    }

    reverb_filter_index = (reverb_filter_index + 1) & 0x3f;
}

static void tick(short *output)
{
    int dryl = 0;
    int dryr = 0;
    int wetl = 0;
    int wetr = 0;
    int outl;
    int outr;

    for (int i = 0; i < SPU_NCH; i++)
    {
        voice *v = &voices[i];
        int vl, vr;

        if (v->step == STEP_OFF)
        {
            continue;
        }

        tick_voice(v, i, &vl, &vr);

        dryl += vl;
        dryr += vr;

        if (ren && (vmixr & (1u << i)))
        {
            wetl += vl;
            wetr += vr;
        }
    }

    if (ren)
    {
        process_reverb(wetl, wetr);
    }

    outl = saturate_sample(dryl + ((last_rev[0] * rvol[0]) >> 15));
    outr = saturate_sample(dryr + ((last_rev[1] * rvol[1]) >> 15));

    output[output_index++] = apply_volume(outl, mvol[0] << 1);
    output[output_index++] = apply_volume(outr, mvol[1] << 1);
}

void spu_init(void)
{
    mvol[0] = 0;
    mvol[1] = 0;
    rvol[0] = 0;
    rvol[1] = 0;

    memset(waveform_data, 0xff, sizeof(waveform_data));
    memset(reverb_work_area, 0, sizeof(reverb_work_area));

    memset(reverb_downsample_buffer, 0, sizeof(reverb_downsample_buffer));
    memset(reverb_upsample_buffer, 0, sizeof(reverb_upsample_buffer));

    spu_set_key_on(SPU_VOICE_MASK);
    spu_set_key_off(SPU_VOICE_MASK);

    vmixr = 0;
    non = 0;
    pmon = 0;

    for (int i = 0; i < SPU_NCH; i++)
    {
        voices[i].voll = 0;
        voices[i].volr = 0;
        voices[i].pitch = 0x3fff;
        voices[i].ssa = 0x200;
        voices[i].a_mode = SPU_ADSR_LIN_INC;
        voices[i].ar = 0;
        voices[i].dr = 0;
        voices[i].s_mode = SPU_ADSR_LIN_INC;
        voices[i].sr = 0;
        voices[i].sl = 0;
        voices[i].r_mode = SPU_ADSR_LIN_DEC;
        voices[i].rr = 0;
        voices[i].step = STEP_OFF;
        voices[i].has_block = 0;

        set_adsr(&voices[i]);
    }

    ren = 0;
    rsize = 64;
    raddr = 0;
    reverb_filter_index = 0;
}

void spu_quit(void)
{
    /* do nothing */
}

void spu_step(int step_size, short *output)
{
    output_index = 0;

    for (int i = 0; i < step_size; i++)
    {
        tick(output);
    }
}

void spu_set_reverb_enable(int enable)
{
    ren = enable;
}

void spu_set_reverb_mode(int mode)
{
    assert(ren == 0);

    rvol[0] = 0;
    rvol[1] = 0;

    memcpy(&rattr, &reverb_mode_attr[mode], sizeof(rattr));
    rsize = reverb_work_area_size[mode];
}

void spu_set_reverb_depth(short l, short r)
{
    rvol[0] = l;
    rvol[1] = r;
}

void spu_set_reverb_on(unsigned int mask)
{
    vmixr |= mask & SPU_VOICE_MASK;
}

void spu_set_reverb_off(unsigned int mask)
{
    vmixr &= ~(mask & SPU_VOICE_MASK);
}

void spu_reverb_clear(void)
{
    memset(reverb_work_area, 0, sizeof(reverb_work_area));
}

void spu_set_noise_on(unsigned int mask)
{
    non |= mask & SPU_VOICE_MASK;

    if (non != 0)
    {
        printf("warning: unimplemented noise mode enabled\n");
    }
}

void spu_set_noise_off(unsigned int mask)
{
    non &= ~(mask & SPU_VOICE_MASK);
}

void spu_set_pitch_lfo_on(unsigned int mask)
{
    pmon |= mask & SPU_VOICE_MASK;
}

void spu_set_pitch_lfo_off(unsigned int mask)
{
    pmon &= ~(mask & SPU_VOICE_MASK);
}

void spu_write(unsigned int addr, void *ptr, unsigned int size)
{
    assert((addr % 0x8) == 0);
    assert((addr + size) <= sizeof(waveform_data));
    memcpy((char *)waveform_data + addr, ptr, size);
}

void spu_set_voice_volume(int num, unsigned short l, unsigned short r)
{
    if (l & 0x8000 || r & 0x8000)
    {
        printf("warning: unsupported volume envelope on voice %d\n", num);
    }

    voices[num].voll = l & 0x7fff;
    voices[num].volr = r & 0x7fff;
}

void spu_set_voice_pitch(int num, unsigned short pitch)
{
    voices[num].pitch = pitch;
}

void spu_set_voice_address(int num, unsigned int addr)
{
    assert((addr % 8) == 0);
    assert(addr < sizeof(waveform_data));
    voices[num].ssa = (addr + 0x7) >> 3;
}

void spu_set_voice_loop_address(int num, unsigned int addr)
{
    assert((addr % 8) == 0);
    assert(addr < sizeof(waveform_data));
    voices[num].lsa = (addr + 0x7) >> 3;
}

void spu_set_voice_attack(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_EXP_INC);

    voices[num].a_mode = mode;
    voices[num].ar = (rate > 0x7f) ? 0x7f : rate;

    set_adsr(&voices[num]);
}

void spu_set_voice_decay(int num, unsigned short rate)
{
    voices[num].dr = (rate > 0xf) ? 0xf : rate;

    set_adsr(&voices[num]);
}

void spu_set_voice_sustain(int num, int mode, unsigned short rate, unsigned short level)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_INC || mode == SPU_ADSR_EXP_DEC);

    voices[num].s_mode = mode;
    voices[num].sr = (rate > 0x7f) ? 0x7f : rate;
    voices[num].sl = (level > 0xf) ? 0xf : level;

    set_adsr(&voices[num]);
}

void spu_set_voice_release(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_DEC);

    voices[num].r_mode = mode;
    voices[num].rr = (rate > 0x1f) ? 0x1f : rate;

    set_adsr(&voices[num]);
}

void spu_set_key_on(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1u << i))
        {
            voices[i].step = STEP_ATTACK;
            voices[i].addr = voices[i].ssa;
            voices[i].env = 0;
            voices[i].env_counter = 0;
            voices[i].pitch_counter = 0;
            voices[i].has_block = 0;

            set_adsr(&voices[i]);

            voices[i].adpcm_hist[0] = 0;
            voices[i].adpcm_hist[1] = 0;

            memset(voices[i].block_samples, 0, sizeof(voices[i].block_samples));
        }
    }
}

void spu_set_key_off(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1u << i))
        {
            if (voices[i].env == 0)
            {
                voices[i].step = STEP_OFF;
            }
            else
            {
                voices[i].step = STEP_RELEASE;
            }

            set_adsr(&voices[i]);
        }
    }
}

void spu_set_master_volume(unsigned short l, unsigned short r)
{
    if (l & 0x8000 || r & 0x8000)
    {
        printf("warning: unsupported master volume envelope\n");
    }

    mvol[0] = l & 0x7fff;
    mvol[1] = r & 0x7fff;
}

void spu_set_noise_clock(int clock)
{
    (void)clock;

    printf("warning: unimplemented noise clock set\n");
}