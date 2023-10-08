#ifndef __F1C100S_REG_DEFE_H__
#define __F1C100S_REG_DEFE_H__

#include <stdint.h>

#define F1C100S_DEFE_BASE           (0x01e00000)

struct f1c100s_defe_reg_t {
  uint32_t enable;                  /* 0x000 */
  uint32_t frame_ctrl;              /* 0x004 */
  uint32_t bypass;                  /* 0x008 */
  uint32_t algorithm_sel;           /* 0x00c */
  uint32_t line_int_ctrl;           /* 0x010 */
  uint8_t  res0[0x0c];              /* 0x014 */
  uint32_t ch0_addr;                /* 0x020 */
  uint32_t ch1_addr;                /* 0x024 */
  uint32_t ch2_addr;                /* 0x028 */
  uint32_t field_sequence;          /* 0x02c */
  uint32_t ch0_offset;              /* 0x030 */
  uint32_t ch1_offset;              /* 0x034 */
  uint32_t ch2_offset;              /* 0x038 */
  uint8_t  res1[0x04];              /* 0x03c */
  uint32_t ch0_stride;              /* 0x040 */
  uint32_t ch1_stride;              /* 0x044 */
  uint32_t ch2_stride;              /* 0x048 */
  uint32_t input_fmt;               /* 0x04c */
  uint32_t ch3_addr;                /* 0x050 */
  uint32_t ch4_addr;                /* 0x054 */
  uint32_t ch5_addr;                /* 0x058 */
  uint32_t output_fmt;              /* 0x05c */
  uint32_t int_enable;              /* 0x060 */
  uint32_t int_status;              /* 0x064 */
  uint32_t status;                  /* 0x068 */
  uint8_t  res2[0x04];              /* 0x06c */
  uint32_t csc_coef00;              /* 0x070 */
  uint32_t csc_coef01;              /* 0x074 */
  uint32_t csc_coef02;              /* 0x078 */
  uint32_t csc_coef03;              /* 0x07c */
  uint32_t csc_coef10;              /* 0x080 */
  uint32_t csc_coef11;              /* 0x084 */
  uint32_t csc_coef12;              /* 0x088 */
  uint32_t csc_coef13;              /* 0x08c */
  uint32_t csc_coef20;              /* 0x090 */
  uint32_t csc_coef21;              /* 0x094 */
  uint32_t csc_coef22;              /* 0x098 */
  uint32_t csc_coef23;              /* 0x09c */
  uint32_t deinterlace_ctrl;        /* 0x0a0 */
  uint32_t deinterlace_diag;        /* 0x0a4 */
  uint32_t deinterlace_tempdiff;    /* 0x0a8 */
  uint32_t deinterlace_sawtooth;    /* 0x0ac */
  uint32_t deinterlace_spatcomp;    /* 0x0b0 */
  uint32_t deinterlace_burstlen;    /* 0x0b4 */
  uint32_t deinterlace_preluma;     /* 0x0b8 */
  uint32_t deinterlace_tile_addr;   /* 0x0bc */
  uint32_t deinterlace_tile_stride; /* 0x0c0 */
  uint8_t  res3[0x0c];              /* 0x0c4 */
  uint32_t wb_stride_enable;        /* 0x0d0 */
  uint32_t ch3_stride;              /* 0x0d4 */
  uint32_t ch4_stride;              /* 0x0d8 */
  uint32_t ch5_stride;              /* 0x0dc */
  uint32_t fe_3d_ctrl;              /* 0x0e0 */
  uint32_t fe_3d_ch0_addr;          /* 0x0e4 */
  uint32_t fe_3d_ch1_addr;          /* 0x0e8 */
  uint32_t fe_3d_ch2_addr;          /* 0x0ec */
  uint32_t fe_3d_ch0_offset;        /* 0x0f0 */
  uint32_t fe_3d_ch1_offset;        /* 0x0f4 */
  uint32_t fe_3d_ch2_offset;        /* 0x0f8 */
  uint8_t  res4[0x04];              /* 0x0fc */
  uint32_t ch0_insize;              /* 0x100 */
  uint32_t ch0_outsize;             /* 0x104 */
  uint32_t ch0_horzfact;            /* 0x108 */
  uint32_t ch0_vertfact;            /* 0x10c */
  uint32_t ch0_horzphase;           /* 0x110 */
  uint32_t ch0_vertphase0;          /* 0x114 */
  uint32_t ch0_vertphase1;          /* 0x118 */
  uint8_t  res5[0x04];              /* 0x11c */
  uint32_t ch0_horztapoffset0;      /* 0x120 */
  uint32_t ch0_horztapoffset1;      /* 0x124 */
  uint32_t ch0_verttapoffset;       /* 0x128 */
  uint8_t  res6[0xd4];              /* 0x12c */
  uint32_t ch1_insize;              /* 0x200 */
  uint32_t ch1_outsize;             /* 0x204 */
  uint32_t ch1_horzfact;            /* 0x208 */
  uint32_t ch1_vertfact;            /* 0x20c */
  uint32_t ch1_horzphase;           /* 0x210 */
  uint32_t ch1_vertphase0;          /* 0x214 */
  uint32_t ch1_vertphase1;          /* 0x218 */
  uint8_t  res7[0x04];              /* 0x21c */
  uint32_t ch1_horztapoffset0;      /* 0x220 */
  uint32_t ch1_horztapoffset1;      /* 0x224 */
  uint32_t ch1_verttapoffset;       /* 0x228 */
  uint8_t  res8[0x1d4];             /* 0x22c */
  uint32_t ch0_horzcoef0[32];       /* 0x400 */
  uint32_t ch0_horzcoef1[32];       /* 0x480 */
  uint32_t ch0_vertcoef[32];        /* 0x500 */
  uint8_t  res9[0x80];              /* 0x580 */
  uint32_t ch1_horzcoef0[32];       /* 0x600 */
  uint32_t ch1_horzcoef1[32];       /* 0x680 */
  uint32_t ch1_vertcoef[32];        /* 0x700 */
  uint8_t  res10[0x280];            /* 0x780 */
  uint32_t vpp_enable;              /* 0xa00 */
  uint32_t vpp_dcti;                /* 0xa04 */
  uint32_t vpp_lp1;                 /* 0xa08 */
  uint32_t vpp_lp2;                 /* 0xa0c */
  uint32_t vpp_wle;                 /* 0xa10 */
  uint32_t vpp_ble;                 /* 0xa14 */
};

#endif /* __F1C100S_REG_DEFE_H__ */
