#include <stdio.h>
#include "utils.h"

bool readValues = false;
float co2_ppm, temperature, relative_humidity;
uint16_t result;
float voltage = 0;
float RunningAverageBuffer[RUNNING_AVERAGE_COUNT];
int NextRunningAverage = 0;
int battery = 0;

void measure(void)
{
    readValues = true;
}

void setMeasureTimer()
{
    readValues = false;
    datetime_t t = {
        .year = 2020,
        .month = 01,
        .day = 13,
        .dotw = 3, // 0 is Sunday, so 3 is Wednesday
        .hour = 11,
        .min = 20,
        .sec = 00};

    // Start the RTC

    rtc_set_datetime(&t);

    // Alarm 5 seconds later
    datetime_t alarm = {
        .year = 2020,
        .month = 01,
        .day = 13,
        .dotw = 3, // 0 is Sunday, so 3 is Wednesday
        .hour = 11,
        .min = 20,
        .sec = 18};

    rtc_set_alarm(&alarm, &measure);
}

// Core 0 Main Code
int main(void)
{

    stdio_init_all();
    initializeDisplay();
    initializaeBT();
    configureBluetooth();
    initializeSensor();
    initializeADC();
    rtc_init();
    // Allocate Memory to Display
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2IN9D_WIDTH % 8 == 0) ? (EPD_2IN9D_WIDTH / 8) : (EPD_2IN9D_WIDTH / 8 + 1)) * EPD_2IN9D_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        sleep_ms(6000);
        return -1;
    }
    
    pairSensor();
    startSensor();
    showLoading(BlackImage);
    printGUI(BlackImage);
    measure();
    while (1)
    {
        result = adc_read();
        float raw_voltage = result * CONVERSION_FACTOR + BATTERY_CALIBRATION;
        RunningAverageBuffer[NextRunningAverage++] = raw_voltage;
        if (NextRunningAverage >= RUNNING_AVERAGE_COUNT)
        {
            NextRunningAverage = 0;
        }
        voltage = 0;
        for (int i = 0; i < RUNNING_AVERAGE_COUNT; ++i)
        {
            voltage += RunningAverageBuffer[i];
        }
        voltage /= RUNNING_AVERAGE_COUNT;
        // voltage = round(voltage * 10) / 10;
        voltage = voltage > 4.19 ? 4.19 : voltage;
        battery = mapfloat(voltage, 2.8, 4.19, 0, 100);
        battery = battery > 100 ? 100 : battery;
        battery = battery < 0 ? 0 : battery;
        // printf("V: %f\nB:%i\n", voltage, battery);
        int16_t err;
        uint16_t data_ready = 0;
        if (readValues)
        {

            err = scd30_get_data_ready(&data_ready);
            if (err != NO_ERROR)
            {
                // printf("Error reading data_ready flag: %i\n", err);
            }
            if (!data_ready)
            {
                // printf("Timeout waiting for data_ready flag %i\n", data_ready);
                setMeasureTimer();
                continue;
            }
            err =
                scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
            // relative_humidity = roundf(relative_humidity * 100) / 100;
            // temperature = roundf(temperature * 100) / 100;
            // co2_ppm = roundf(co2_ppm * 100) / 100;
            if (err != NO_ERROR)
            {
                // printf("error reading measurement\n");
            }
            else
            {

                // printf("measured co2 concentration: %0.2f ppm, "
                //     "measured temperature: %0.2f degreeCelsius, "
                //     "measured humidity: %0.2f %%RH"
                //     "Battery: %i\n"
                //     "voltage: %f\n",
                //     co2_ppm, temperature, relative_humidity, battery, voltage);

                uint8_t lv[20];
                lv[0] = 'A';
                lv[1] = 'T';
                lv[2] = '+';
                lv[3] = 'A';
                lv[4] = 'V';
                lv[5] = 'D';
                lv[6] = 'A';
                lv[7] = '=';
                encode(128, lv, 8, 12, co2_ppm);
                if (temperature < 0)
                {
                    lv[12] = 0x31;
                }
                else
                {
                    lv[12] = 0x30;
                }
                encode(128, lv, 13, 16, temperature < 0 ? -temperature : temperature);
                encode(128, lv, 16, 18, relative_humidity);
                encode(128, lv, 18, 20, battery);
                uart_write_blocking(UART_ID, lv, 20);
                uart_puts(UART_ID, "AT");
                clearValueSections(BlackImage);

                co2_ppm = co2_ppm >= ((float)100000) ? 99999.99 : co2_ppm;
                if (temperature >= 1000)
                {
                    temperature = 999.99;
                }
                if (temperature <= -1000)
                {
                    temperature = -999.99;
                }
                if (relative_humidity >= 100)
                {
                    relative_humidity = 100;
                }
                if (relative_humidity <= 0)
                {
                    relative_humidity = 0;
                }
                PrintDecimal(CO2_VALUE_START_X, CO2_VALUE_START_Y, co2_ppm, &Font24, &Font16, BLACK, WHITE, 5, 2, "PPM", 3);
                PrintDecimal(TMP_VALUE_START_X, TMP_VALUE_START_Y, temperature, &Font24, &Font16, BLACK, WHITE, 4, 2, "C", 1);
                Paint_DrawString_EN(127 - (Font24.Width * 1) - Font16.Width - (TMP_VALUE_START_X), TMP_VALUE_START_Y + Font24.Height, "o", &Font16, BLACK, WHITE);
                PrintDecimal(RH_VALUE_START_X, RH_VALUE_START_Y, relative_humidity, &Font24, &Font16, BLACK, WHITE, 3, 2, "%", 1);

                char bBuff[11];
                sprintf(bBuff, "Bat: %3d%%", battery);
                Paint_DrawString_EN(BATTERY_LABEL_WINDOW_START_X + 1, BATTERY_LABEL_WINDOW_START_Y + 4, bBuff, &Font20, WHITE, BLACK);
                EPD_2IN9D_DisplayPart(BlackImage);
            }
            setMeasureTimer();
        }
    }
}