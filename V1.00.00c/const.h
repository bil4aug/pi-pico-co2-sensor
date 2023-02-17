#ifdef __cplusplus
 extern "C" {
#endif
#define UART_ID uart0
#define BAUD_RATE 9600

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define CO2_LABEL_WINDOW_START_X 0
#define CO2_LABEL_WINDOW_START_Y 0
#define CO2_LABEL_WINDOW_END_X 127
#define CO2_LABEL_WINDOW_END_Y Font24.Height + 4
#define CO2_LABEL_START_X 4
#define CO2_LABEL_START_Y 4
#define CO2_VALUE_WINDOW_START_X 0
#define CO2_VALUE_WINDOW_START_Y CO2_LABEL_WINDOW_END_Y
#define CO2_VALUE_WINDOW_END_X 127
#define CO2_VALUE_WINDOW_END_Y CO2_VALUE_WINDOW_START_Y + (Font24.Height * 2) + 10
#define CO2_VALUE_START_X 2
#define CO2_VALUE_START_Y CO2_VALUE_WINDOW_START_Y + 4
#define TMP_LABEL_WINDOW_START_X 0
#define TMP_LABEL_WINDOW_START_Y CO2_VALUE_WINDOW_END_Y
#define TMP_LABEL_WINDOW_END_X 127
#define TMP_LABEL_WINDOW_END_Y TMP_LABEL_WINDOW_START_Y + Font24.Height + 5
#define TMP_LABEL_START_X 4
#define TMP_LABEL_START_Y TMP_LABEL_WINDOW_START_Y + 5
#define TMP_VALUE_WINDOW_START_X 0
#define TMP_VALUE_WINDOW_START_Y TMP_LABEL_WINDOW_END_Y
#define TMP_VALUE_WINDOW_END_X 127
#define TMP_VALUE_WINDOW_END_Y TMP_VALUE_WINDOW_START_Y + (Font24.Height * 2) + 10
#define TMP_VALUE_START_X 2
#define TMP_VALUE_START_Y TMP_VALUE_WINDOW_START_Y + 4
#define RH_LABEL_WINDOW_START_X 0
#define RH_LABEL_WINDOW_START_Y TMP_VALUE_WINDOW_END_Y
#define RH_LABEL_WINDOW_END_X 127
#define RH_LABEL_WINDOW_END_Y RH_LABEL_WINDOW_START_Y + Font24.Height + 5
#define RH_LABEL_START_X 4
#define RH_LABEL_START_Y RH_LABEL_WINDOW_START_Y + 5
#define RH_VALUE_WINDOW_START_X 0
#define RH_VALUE_WINDOW_START_Y RH_LABEL_WINDOW_END_Y
#define RH_VALUE_WINDOW_END_X 127
#define RH_VALUE_WINDOW_END_Y RH_VALUE_WINDOW_START_Y + (Font24.Height * 2) + 10
#define RH_VALUE_START_X 2
#define RH_VALUE_START_Y RH_VALUE_WINDOW_START_Y + 4
#define BATTERY_LABEL_WINDOW_START_X 0
#define BATTERY_LABEL_WINDOW_START_Y RH_VALUE_WINDOW_END_Y
#define BATTERY_LABEL_WINDOW_END_X 127
#define BATTERY_LABEL_WINDOW_END_Y EPD_2IN9D_HEIGHT
#define RUNNING_AVERAGE_COUNT 10
#define INTERVAL_IN_SECONDS 60
#define BATTERY_CALIBRATION 0
#define VOLTAGE_PIN 29
#define VOLTAGE_NUMBER_INPUT 3
#define BT_DELAY_MS 200
#define CONVERSION_FACTOR 3.3f / (1 << 12) * 3
 #ifdef __cplusplus
}
#endif