#include "pico/stdlib.h"
#ifdef __cplusplus
extern "C" {
#endif
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void encode(int base, uint8_t* out,int from , int to, float number);
#ifdef __cplusplus
}
#endif