#include "usrinc.h"
#include "tslib.h"

////////////////////////////////////////////////////////////////////////////////
#define SQUARE(i)   ((i)*(i))


/**
* This filter works as follows: we keep track of latest N samples,
* and average them with certain weights. The oldest samples have the
* least weight and the most recent samples have the most weight.
* This helps remove the jitter and at the same time doesn't influence
* responsivity because for each input sample we generate one output
* sample; pen movement becomes just somehow more smooth.
*/
#define NR_SAMPHISTLEN      (4)

/* To keep things simple (avoiding division) we ensure that
* SUM(weight) = power-of-two. Also we must know how to approximate
* measurements when we have less than NR_SAMPHISTLEN samples.
*/
static u8_t const weights[NR_SAMPHISTLEN - 1][NR_SAMPHISTLEN + 1] = {
  /* The last element is pow2(SUM(0..3)) */
  { 5, 3, 0, 0, 3 },  /* When we have 2 samples ... */
  { 8, 5, 3, 0, 4 },  /* When we have 3 samples ... */
  { 6, 4, 3, 3, 4 },  /* When we have 4 samples ... */
};

static struct {
  int xsamps[NR_SAMPHISTLEN];
  int ysamps[NR_SAMPHISTLEN];
  u8_t head, count, stat;
} dejitter;

static void dejitter_average(int* px, int* py)
{
  int count = dejitter.count;
  int sn = dejitter.head;
  int i, xsum = 0, ysum = 0;
  u8_t const* weightsPtr;

  // count在 [2, 4] 区间内。
  weightsPtr = weights[count - 2];
  for (i = 0; i < count; i++) {
    xsum += (dejitter.xsamps[sn] * weightsPtr[i]);
    ysum += (dejitter.ysamps[sn] * weightsPtr[i]);
    sn = ((sn - 1) & (NR_SAMPHISTLEN - 1));
  }
  *px = (xsum >> weightsPtr[NR_SAMPHISTLEN]);
  *py = (ysum >> weightsPtr[NR_SAMPHISTLEN]);
}

void tslib_dejitter_clr(void)
{
  dejitter.head = 0;
  dejitter.count = 0;
  if (dejitter.stat) {
    tslib_clr_callback();
    dejitter.stat = 0;
  }
}

void tslib_dejitter_add(int x, int y)
{
  dejitter.stat = 1;
  if (dejitter.count) {
    int sn, dx, dy;
    sn = ((dejitter.head - 1) & (NR_SAMPHISTLEN - 1));
    dx = (x - dejitter.xsamps[sn]);
    dy = (y - dejitter.ysamps[sn]);
    if (dx * dx + dy * dy > SQUARE(10)) {
      dejitter.count = 0;
    }
  }
  dejitter.xsamps[dejitter.head] = x;
  dejitter.ysamps[dejitter.head] = y;
  if (dejitter.count < NR_SAMPHISTLEN) {
    dejitter.count++;
  }
  if (dejitter.count > 1) {
    dejitter_average(&x, &y);
  }
  dejitter.head = ((dejitter.head + 1) & (NR_SAMPHISTLEN - 1));
  tslib_add_callback(x, y);
}

////////////////////////////////////////////////////////////////////////////////
static struct {
  int xavg, yavg;
  u8_t stat;
} average;

void tslib_average_clr(void)
{
  average.stat = 0;
  tslib_dejitter_clr();
}

void tslib_average_add(int x, int y)
{
  if (average.stat) {
    average.xavg = ((average.xavg * 5 + x * 3) >> 3);
    average.yavg = ((average.yavg * 5 + y * 3) >> 3);
    tslib_dejitter_add(average.xavg, average.yavg);
  } else {
    average.xavg = x;
    average.yavg = y;
    average.stat = 1;
  }
}

////////////////////////////////////////////////////////////////////////////////
static struct {
  int xnoise, ynoise;
  int xlast, ylast;
  u8_t stat;
} variance;

void tslib_variance_clr(void)
{
  variance.stat = 0;
  tslib_average_clr();
}

void tslib_variance_add(int x, int y)
{
  int dx, dy;
  int submit;

  switch (variance.stat) {
    case 0:     /* 触摸笔按下的第1个点 */
      variance.xlast = x;
      variance.ylast = y;
      variance.stat = 1;
      break;

    case 1:     /* 尚未有可疑点存在 */
      dx = (x - variance.xlast);
      dy = (y - variance.ylast);
      if (dx * dx + dy * dy < SQUARE(10)) {
        // 触摸点在许可范围内。
        variance.xlast = x;
        variance.ylast = y;
        tslib_average_add(x, y);
      } else {
        // 触摸点作为可疑点。
        variance.xnoise = x;
        variance.ynoise = y;
        variance.stat = 2;
      }
      break;

    case 2:     /* 有可疑点存在，仔细检测新点 */
      submit = 0;
      dx = (x - variance.xnoise);
      dy = (y - variance.ynoise);
      if (dx * dx + dy * dy < SQUARE(10)) {
        // 新的触摸点接近可疑点。
        submit = 1;
      } else {
        dx = (x - variance.xlast);
        dy = (y - variance.ylast);
        if (dx * dx + dy * dy < SQUARE(10)) {
          // 新的触摸点接近原合法点。
          submit = 1;
        }
      }
      variance.xlast = x;
      variance.ylast = y;
      variance.stat = 1;
      if (submit) {
        // 新的触摸点通过检测。
        tslib_average_add(x, y);
      }
      break;

    case 3:     /* 有可疑点存在，无条件接受新点 */
      variance.xlast = x;
      variance.ylast = y;
      variance.stat = 1;
      tslib_average_add(x, y);
      break;

    default:
      tslib_variance_clr();
      break;
  }
}
