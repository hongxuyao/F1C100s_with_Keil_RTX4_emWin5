#ifndef __F1C100S_REG_DEBE_H__
#define __F1C100S_REG_DEBE_H__

#include <stdint.h>

#define F1C100S_DEBE_BASE           (0x01e60000)

struct f1c100s_debe_reg_t {
  uint8_t  res0[0x800];             /* 0x000 */
  uint32_t mode;                    /* 0x800 */
  uint32_t backcolor;               /* 0x804 */
  uint32_t disp_size;               /* 0x808 */
  uint8_t  res1[0x4];               /* 0x80c */
  uint32_t layer0_size;             /* 0x810 */
  uint32_t layer1_size;             /* 0x814 */
  uint32_t layer2_size;             /* 0x818 */
  uint32_t layer3_size;             /* 0x81c */
  uint32_t layer0_pos;              /* 0x820 */
  uint32_t layer1_pos;              /* 0x824 */
  uint32_t layer2_pos;              /* 0x828 */
  uint32_t layer3_pos;              /* 0x82c */
  uint8_t  res2[0x10];              /* 0x830 */
  uint32_t layer0_stride;           /* 0x840 */
  uint32_t layer1_stride;           /* 0x844 */
  uint32_t layer2_stride;           /* 0x848 */
  uint32_t layer3_stride;           /* 0x84c */
  uint32_t layer0_addr_low32b;      /* 0x850 */
  uint32_t layer1_addr_low32b;      /* 0x854 */
  uint32_t layer2_addr_low32b;      /* 0x858 */
  uint32_t layer3_addr_low32b;      /* 0x85c */
  uint32_t layer0_addr_high4b;      /* 0x860 */
  uint32_t layer1_addr_high4b;      /* 0x864 */
  uint32_t layer2_addr_high4b;      /* 0x868 */
  uint32_t layer3_addr_high4b;      /* 0x86c */
  uint32_t reg_ctrl;                /* 0x870 */
  uint8_t  res3[0xc];               /* 0x874 */
  uint32_t color_key_max;           /* 0x880 */
  uint32_t color_key_min;           /* 0x884 */
  uint32_t color_key_config;        /* 0x888 */
  uint8_t  res4[0x4];               /* 0x88c */
  uint32_t layer0_attr0_ctrl;       /* 0x890 */
  uint32_t layer1_attr0_ctrl;       /* 0x894 */
  uint32_t layer2_attr0_ctrl;       /* 0x898 */
  uint32_t layer3_attr0_ctrl;       /* 0x89c */
  uint32_t layer0_attr1_ctrl;       /* 0x8a0 */
  uint32_t layer1_attr1_ctrl;       /* 0x8a4 */
  uint32_t layer2_attr1_ctrl;       /* 0x8a8 */
  uint32_t layer3_attr1_ctrl;       /* 0x8ac */
  uint8_t  res5[0x110];             /* 0x8b0 */
  uint32_t output_color_ctrl;       /* 0x9c0 */
  uint8_t  res6[0xc];               /* 0x9c4 */
  uint32_t output_color_coef[12];   /* 0x9d0 */
};

#endif /* __F1C100S_REG_DEBE_H__ */
