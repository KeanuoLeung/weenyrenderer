#ifndef _TINY_MATH__
#define _TINY_MATH__

#ifdef _KEANUO_DEBUGING__
#define calc_slope(x0, y0, x1, y1) \
  (printf("[Tinymath] the slope is %.2f \n", (float)(y1 - y0) / (x1 - x0)), (float)(y1 - y0) / (x1 - x0))
#else
#define calc_slope(x0, y0, x1, y1) \
  (float)(y1 - y0) / (x1 - x0)
#endif

#endif // _TINY_MATH__