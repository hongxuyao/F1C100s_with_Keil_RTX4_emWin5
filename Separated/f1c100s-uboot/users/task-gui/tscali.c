#include "usrinc.h"
#include "tslib.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
static tslib_matrix_t mx_data = {
  .A  = 1, .B  = 0, .C = 0,
  .D  = 0, .E  = 1, .F = 0,
  .zR = 0,
};

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TSLIB_Init
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t TSLIB_Init(void)
{
  mx_data.A  = mx_data.E  = 1;
  mx_data.B  = mx_data.C  = 0;
  mx_data.D  = mx_data.F  = 0;
  mx_data.zR = 0;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TSLIB_CnvToLogPoint
//| 功能描述 |: 把触摸屏的坐标采样值转化为LCD点阵坐标。
//|          |:
//| 参数列表 |: pl 逻辑坐标数组；
//|          |: (phyX,phyY) 物理坐标数组；
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TSLIB_CnvToLogPoint(tslib_point_t* pl, u32_t phyX, u32_t phyY)
{
  pl->x = (int)(mx_data.A * phyX + mx_data.B * phyY + mx_data.C);
  pl->y = (int)(mx_data.D * phyX + mx_data.E * phyY + mx_data.F);
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TSLIB_CalcZ
//| 功能描述 |: 根据给定的X采样值计算对应的Z值。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
int TSLIB_CalcZ(int xphy, int yphy)
{
  return mx_data.zR;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TSLIB_SetCalMtrx
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TSLIB_SetCalMtrx(const tslib_matrix_t* mtrx)
{
  mx_data = *mtrx;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TSLIB_GenCalMtrx
//| 功能描述 |:
//|          |:
//| 参数列表 |: pl 逻辑坐标数组；
//|          |: pt 物理坐标数组；
//| 返    回 |:
//|          |: 成功则返回TRUE，否则返回FALSE。
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t TSLIB_GenCalMtrx(const tslib_point_t* pl, const tslib_point_t* pt, tslib_matrix_t* output)
{
  float K, fV;
  /*
  output->A = output->E = 1;
  output->B = output->C = 0;
  output->D = output->F = 0;
  */
  K = ((pt[0].x - pt[2].x) * (pt[1].y - pt[2].y)) -
      ((pt[1].x - pt[2].x) * (pt[0].y - pt[2].y));

  //if((K != 0) && (K == K)){ 这是C语言检查浮点数是否有效的另一个更通用的方法！
  if ((K != 0) && (K != NAN)) {
    fV = ((pl[0].x - pl[2].x) * (pt[1].y - pt[2].y)) -
         ((pl[1].x - pl[2].x) * (pt[0].y - pt[2].y));
    output->A = fV / K;

    fV = ((pt[0].x - pt[2].x) * (pl[1].x - pl[2].x)) -
         ((pl[0].x - pl[2].x) * (pt[1].x - pt[2].x));
    output->B = fV / K;

    fV = (pt[2].x * pl[1].x - pt[1].x * pl[2].x) * pt[0].y +
         (pt[0].x * pl[2].x - pt[2].x * pl[0].x) * pt[1].y +
         (pt[1].x * pl[0].x - pt[0].x * pl[1].x) * pt[2].y;
    output->C = fV / K;

    fV = ((pl[0].y - pl[2].y) * (pt[1].y - pt[2].y)) -
         ((pl[1].y - pl[2].y) * (pt[0].y - pt[2].y));
    output->D = fV / K;

    fV = ((pt[0].x - pt[2].x) * (pl[1].y - pl[2].y)) -
         ((pl[0].y - pl[2].y) * (pt[1].x - pt[2].x));
    output->E = fV / K;

    fV = (pt[2].x * pl[1].y - pt[1].x * pl[2].y) * pt[0].y +
         (pt[0].x * pl[2].y - pt[2].x * pl[0].y) * pt[1].y +
         (pt[1].x * pl[0].y - pt[0].x * pl[1].y) * pt[2].y;
    output->F = fV / K;

    return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: perform_calibration
//| 功能描述 |: 用采集的5个坐标，通过算法计算后得到校准矩阵。
//|          |: 校准矩阵保存在cal->a[]里面。
//| 参数列表 |:
//|          |:
//| 返    回 |: 成功返回1，失败返回0.
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
int perform_calibration(tslib_calib_t* cal)
{
  int j;
  float n, x, y, x2, y2, xy, z, zx, zy;
  float det, a, b, c, e, f, i;
  float scaling = 65536.0;

  // Get sums for matrix
  n = x = y = x2 = y2 = xy = 0;
  for (j = 0; j < TSC_SAMP_NUM; j++) {
    n  += 1.0;
    x  += (float)cal->x[j];
    y  += (float)cal->y[j];
    x2 += (float)(cal->x[j] * cal->x[j]);
    y2 += (float)(cal->y[j] * cal->y[j]);
    xy += (float)(cal->x[j] * cal->y[j]);
  }

  // Get determinant of matrix -- check if determinant is too small
  det = n * (x2 * y2 - xy * xy) + x * (xy * y - x * y2) + y * (x * xy - y * x2);
  if (det < 0.1 && det > -0.1) {
    return 0;
  }

  // Get elements of inverse matrix
  a = (x2 * y2 - xy * xy) / det;
  b = (xy * y - x * y2) / det;
  c = (x * xy - y * x2) / det;
  e = (n * y2 - y * y) / det;
  f = (x * y - n * xy) / det;
  i = (n * x2 - x * x) / det;

  // Get sums for x calibration
  z = zx = zy = 0;
  for (j = 0; j < TSC_SAMP_NUM; j++) {
    z  += (float)cal->xfb[j];
    zx += (float)(cal->xfb[j] * cal->x[j]);
    zy += (float)(cal->xfb[j] * cal->y[j]);
  }

  // Now multiply out to get the calibration for framebuffer x coord
  cal->a[0] = (int)((a * z + b * zx + c * zy) * (scaling));
  cal->a[1] = (int)((b * z + e * zx + f * zy) * (scaling));
  cal->a[2] = (int)((c * z + f * zx + i * zy) * (scaling));

  // Get sums for y calibration
  z = zx = zy = 0;
  for (j = 0; j < TSC_SAMP_NUM; j++) {
    z += (float)cal->yfb[j];
    zx += (float)(cal->yfb[j] * cal->x[j]);
    zy += (float)(cal->yfb[j] * cal->y[j]);
  }

  // Now multiply out to get the calibration for framebuffer y coord
  cal->a[3] = (int)((a * z + b * zx + c * zy) * (scaling));
  cal->a[4] = (int)((b * z + e * zx + f * zy) * (scaling));
  cal->a[5] = (int)((c * z + f * zx + i * zy) * (scaling));

  // If we got here, we're OK, so assign scaling to a[6] and return
  cal->a[6] = (int)scaling;

  cal->a[7] = (cal->z[0] + cal->z[1] + cal->z[2] + cal->z[3] + cal->z[4]) / 5;

  return 1;
#if 0
  // This code was here originally to just insert default values
  for (j = 0; j < 7; j++) {
    cal->a[j] = 0;
  }
  cal->a[1] = 1;
  cal->a[5] = 1;
  cal->a[6] = 1;
  return 1;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: tslib_update_matrix
//| 功能描述 |: 分别是：C,A,B,F,D,E,G,za,zb
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void tslib_update_matrix(const int caldata[])
{
  float fG = (float)caldata[6];

  if (caldata[6] == 0) {
    fG = 65536.0;
  }
  mx_data.A  = (caldata[1] / fG);
  mx_data.B  = (caldata[2] / fG);
  mx_data.C  = (caldata[0] / fG);
  mx_data.D  = (caldata[4] / fG);
  mx_data.E  = (caldata[5] / fG);
  mx_data.F  = (caldata[3] / fG);

  mx_data.zR = caldata[7];
}

