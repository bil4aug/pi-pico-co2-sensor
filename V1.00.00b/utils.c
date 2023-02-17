#include "utils.h"
#ifdef __cplusplus
extern "C"
{
#endif
  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
  void encode(int base, uint8_t* out,int from , int to, float number) {
    int rounded = number * 100, r, written = from;
    while (rounded && written < to) {
        r = rounded % base;
        rounded -= r;
        rounded /= base;
        out[written] = r+32;
        written++;
    }
    while (written < to)
    {
        out[written] = 31;
        written++;
    }
}
#ifdef __cplusplus
}
#endif