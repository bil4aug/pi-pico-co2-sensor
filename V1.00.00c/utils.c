#include "utils.h"
#ifdef __cplusplus
extern "C"
{
#endif
  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
  void encode(int base, uint8_t *out, int from, int to, float number)
  {
    int rounded = number * 100, r, written = from;
    while (rounded && written < to)
    {
      r = rounded % base;
      rounded -= r;
      rounded /= base;
      out[written] = r + 32;
      written++;
    }
    while (written < to)
    {
      out[written] = 31;
      written++;
    }
  }
  void PrintDecimal(uint16_t Xstart, uint16_t Ystart, float Decimal, sFONT *Font, sFONT *DecFont, uint16_t f_Color, uint16_t b_Color, short digits, short decimals, char *unit, short unitLength)
  {
    char buff[10];
    sprintf(buff, "%*d", digits, ((int)(Decimal / 1)));
    Paint_DrawString_EN((127 - (Font->Width * (digits + 1)) - (decimals * DecFont->Width)) - Xstart, Ystart, buff, Font, f_Color, b_Color);
    char buffDec[3];
    sprintf(buffDec, "%0*d", decimals, ((int)(Decimal * 100)) % 100);
    Paint_DrawString_EN((127 - (Font->Width * 1) - (decimals * DecFont->Width)) - Xstart, Ystart, ".", Font, f_Color, b_Color);
    Paint_DrawString_EN((127 - (decimals * DecFont->Width)) - Xstart, Ystart + (Font->Height - DecFont->Height), buffDec, DecFont, f_Color, b_Color);
    Paint_DrawString_EN(127 - (Font->Width * unitLength) - Xstart, Ystart + 5 + Font->Height, unit, Font, f_Color, b_Color);
  }

  void initializeDisplay()
  {
    DEV_Module_Init();
    EPD_2IN9D_Init();
    EPD_2IN9D_Clear();
    DEV_Delay_ms(500);
  }
  void initializaeBT()
  {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  }
  void initializeADC()
  {
    adc_init();
    adc_gpio_init(VOLTAGE_PIN);
    adc_select_input(VOLTAGE_NUMBER_INPUT);
  }
  void configureBluetooth()
  {
    sleep_ms(200);
    uart_puts(uart0, "AT+NAME=CO2 SENSOR");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+RFPM=2");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+CONT=1");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+RESET");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+LED=1");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+MODE=0");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+AINT=1600");
    sleep_ms(200);
  }
  void initializeSensor()
  {
    sensirion_i2c_init();
  }

  void pairSensor()
  {
    while (true)
    {
      int16_t err = scd30_probe();
      if (err == NO_ERROR)
      {
        break;
      }
      //printf("SCD30 sensor probing failed error: %i\n", err);
      sensirion_sleep_usec(50000u);
    }

    //printf("SCD30 sensor probing successful\n");
  }

  void startSensor()
  {
    scd30_enable_automatic_self_calibration(1);
    scd30_set_measurement_interval(INTERVAL_IN_SECONDS);
    sensirion_sleep_usec(20000u);
    scd30_start_periodic_measurement(0);
  }

  void showLoading(uint8_t *image)
  {

    Paint_NewImage(image, EPD_2IN9D_WIDTH, EPD_2IN9D_HEIGHT, 90, BLACK);
    Paint_SelectImage(image);
    Paint_Clear(BLACK);
    EPD_2IN9D_DisplayPart_RYI(image);
    EPD_2IN9D_DisplayPart(image);
    Paint_Clear(WHITE);
    EPD_2IN9D_DisplayPart_RYI(image);
    EPD_2IN9D_DisplayPart(image);
    Paint_Clear(BLACK);
    EPD_2IN9D_DisplayPart_RYI(image);
    EPD_2IN9D_DisplayPart(image);
    Paint_Clear(WHITE);
    EPD_2IN9D_DisplayPart_RYI(image);
    EPD_2IN9D_DisplayPart(image);
    Paint_Clear(BLACK);
    EPD_2IN9D_DisplayPart_RYI(image);
    EPD_2IN9D_DisplayPart(image);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(63, 52, "Loading...", &Font24, WHITE, BLACK);
    EPD_2IN9D_DisplayPart_RYI(image);
    sensirion_sleep_usec(1200000);

    for (int i = 0; i < ((INTERVAL_IN_SECONDS - 1)); i++)
    {
      Paint_ClearWindows(0, 0, EPD_2IN9D_HEIGHT, EPD_2IN9D_WIDTH, WHITE);
      EPD_2IN9D_DisplayPart_RYI(image);
      Paint_DrawString_EN(63, 52, "Loading", &Font24, WHITE, BLACK);
      for (int j = 0; j < i % 4; j++)
      {
        Paint_DrawString_EN(63 + (Font24.Width * (7 + j)), 52, ".", &Font24, WHITE, BLACK);
      }

      EPD_2IN9D_DisplayPart_RYI(image);
      sensirion_sleep_usec(1200000);
    }
  }

  void printGUI(uint8_t *image)
  {
    Paint_NewImage(image, EPD_2IN9D_WIDTH, EPD_2IN9D_HEIGHT, 0, BLACK);
    Paint_Clear(BLACK);
    EPD_2IN9D_Clear();
    Paint_ClearWindows(CO2_LABEL_WINDOW_START_X, CO2_LABEL_WINDOW_START_Y, CO2_LABEL_WINDOW_END_X, CO2_LABEL_WINDOW_END_Y, WHITE);
    Paint_DrawString_EN(CO2_LABEL_START_X, CO2_LABEL_START_Y, "CO2", &Font24, WHITE, BLACK);
    Paint_ClearWindows(TMP_LABEL_WINDOW_START_X, TMP_LABEL_WINDOW_START_Y, TMP_LABEL_WINDOW_END_X, TMP_LABEL_WINDOW_END_Y, WHITE);
    Paint_DrawString_EN(TMP_LABEL_START_X, TMP_LABEL_START_Y, "TEMP.", &Font24, WHITE, BLACK);
    Paint_ClearWindows(RH_LABEL_WINDOW_START_X, RH_LABEL_WINDOW_START_Y, RH_LABEL_WINDOW_END_X, RH_LABEL_WINDOW_END_Y, WHITE);
    Paint_DrawString_EN(RH_LABEL_START_X, RH_LABEL_START_Y, "RH.", &Font24, WHITE, BLACK);
  }

  void clearValueSections(uint8_t *image)
  {
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, BLACK);
    EPD_2IN9D_DisplayPart(image);
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, WHITE);
    EPD_2IN9D_DisplayPart(image);
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, BLACK);
    EPD_2IN9D_DisplayPart(image);
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, WHITE);
    EPD_2IN9D_DisplayPart(image);
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, WHITE);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, BLACK);
    EPD_2IN9D_DisplayPart(image);
    Paint_ClearWindows(CO2_VALUE_WINDOW_START_X, CO2_VALUE_WINDOW_START_Y, CO2_VALUE_WINDOW_END_X, CO2_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(TMP_VALUE_WINDOW_START_X, TMP_VALUE_WINDOW_START_Y, TMP_VALUE_WINDOW_END_X, TMP_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(RH_VALUE_WINDOW_START_X, RH_VALUE_WINDOW_START_Y, RH_VALUE_WINDOW_END_X, RH_VALUE_WINDOW_END_Y, BLACK);
    Paint_ClearWindows(BATTERY_LABEL_WINDOW_START_X, BATTERY_LABEL_WINDOW_START_Y, BATTERY_LABEL_WINDOW_END_X, BATTERY_LABEL_WINDOW_END_Y, WHITE);
  }

#ifdef __cplusplus
}
#endif