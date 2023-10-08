#ifndef __TSLIB_H__
#define __TSLIB_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

#define TSC_SAMP_NUM    (5)

#define TSC_SAVE_NUM    (8)

typedef struct {
  float A, B, C;
  float D, E, F;
  int zR;
} tslib_matrix_t;

typedef struct {
  int x, y;
} tslib_point_t;

bool_t TSLIB_Init(void);
int    TSLIB_CalcZ(int xphy, int yphy);
void   TSLIB_CnvToLogPoint(tslib_point_t* pl, u32_t phyX, u32_t phyY);
void   TSLIB_SetCalMtrx(const tslib_matrix_t* mtrx);
bool_t TSLIB_GenCalMtrx(const tslib_point_t* pl, const tslib_point_t* pt, tslib_matrix_t* output);

typedef struct {
  /* 逻辑坐标X数组 */
  int xfb[TSC_SAMP_NUM];
  /* 逻辑坐标Y数组 */
  int yfb[TSC_SAMP_NUM];
  /* 物理采样X数组 */
  int x[TSC_SAMP_NUM];
  /* 物理采样Y数组 */
  int y[TSC_SAMP_NUM];
  /* 物理采样Z数组 */
  int z[TSC_SAMP_NUM];
  /* 5点校准算法的计算结果 */
  /*
  A  = a[1];
  B  = a[2];
  C  = a[0];
  D  = a[4];
  E  = a[5];
  F  = a[3];
  G  = a[6];
  R  = a[7]
  */
  int a[TSC_SAVE_NUM];
} tslib_calib_t;

int  perform_calibration(tslib_calib_t* cal);
void tslib_update_matrix(const int caldata[]);

////////////////////////////////////////////////////////////////////////////////
/* variance是第1级滤波 */
void tslib_variance_clr(void);
void tslib_variance_add(int x, int y);

/* average是第2级滤波 */
void tslib_average_clr(void);
void tslib_average_add(int x, int y);

/* dejitter是第3级滤波 */
void tslib_dejitter_clr(void);
void tslib_dejitter_add(int x, int y);

/* callback是最后一级，负责把采样值传递给GUI */
void tslib_clr_callback(void);
void tslib_add_callback(int x, int y);


////////////////////////////////////////////////////////////////////////////////
#endif /* __TSLIB_H__ */

