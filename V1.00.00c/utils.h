#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/rtc.h"
#include "GUI_Paint.h"
#include "EPD_2in9d.h"
#include "scd30.h"
#include "const.h"
#ifdef __cplusplus
extern "C" {
#endif
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void encode(int base, uint8_t* out,int from , int to, float number);
void PrintDecimal(uint16_t Xstart, uint16_t Ystart, float Decimal, sFONT *Font, sFONT *DecFont, uint16_t f_Color, uint16_t b_Color, short digits, short decimals, char *unit, short unitLength);
void initializeDisplay();
void initializaeBT();
void initializeADC();
void configureBluetooth();
void initializeSensor();
void pairSensor();
void startSensor();
void showLoading(uint8_t *image);
void printGUI(uint8_t *image);
void clearValueSections(uint8_t *image);
#ifdef __cplusplus
}
#endif