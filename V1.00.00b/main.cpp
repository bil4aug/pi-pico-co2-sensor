#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "scd30.h"
#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"
#include "pico-ssd1306/shapeRenderer/ShapeRenderer.h"
#include "hardware/rtc.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "utils.h"
#define SSD1306_ASCII_FULL

#define UART_ID uart0
#define BAUD_RATE 9600

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

bool screenOn = false, readValues = false;
const float CONVERSION_FACTOR = 3.3f / (1 << 12) * 3;

// Core 1 interrupt Handler
void core1_interrupt_handler()
{

    // Receive Raw Value, Convert and Print Temperature Value
    while (multicore_fifo_rvalid())
    {
        uint32_t trig = multicore_fifo_pop_blocking();
        if (trig == 1)
        {
            screenOn = true;
        }
        else
        {
            screenOn = false;
        }
    }

    multicore_fifo_clear_irq(); // Clear interrupt
}

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

// Core 1 Main Code
void core1_entry()
{
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    float co2_ppm, temperature, relative_humidity;
    uint16_t result;
    float voltage = 0;
    const int RunningAverageCount = 10;
    float RunningAverageBuffer[RunningAverageCount];
    int NextRunningAverage = 0;
    int battery = 0;
    float calibration = 0;
    uint16_t interval_in_seconds = 15;
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    rtc_init();
    sensirion_i2c_init();
    pico_ssd1306::SSD1306 display = pico_ssd1306::SSD1306(i2c_default, 0x3C, pico_ssd1306::Size::W128xH64);
    pico_ssd1306::SSD1306 display1 = pico_ssd1306::SSD1306(i2c1, 0x3C, pico_ssd1306::Size::W128xH64);
    display.setOrientation(0);
    display1.setOrientation(0);
    drawText(&display, font_16x32, "Loading.", 0, 16);
    drawText(&display1, font_16x32, "Loading.", 0, 16);
    display.sendBuffer();
    display1.sendBuffer();
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
    uart_puts(uart0, "AT+MODE=1");
    sleep_ms(200);
    uart_puts(uart0, "AT");
    sleep_ms(200);
    uart_puts(uart0, "AT+AINT=4800");
    sleep_ms(200);
    while (true)
    {
        int16_t err = scd30_probe();
        if (err == NO_ERROR)
        {
            break;
        }
        printf("SCD30 sensor probing failed error: %i\n", err);
        sensirion_sleep_usec(50000u);
    }

    printf("SCD30 sensor probing successful\n");
    scd30_enable_automatic_self_calibration(1);
    scd30_set_measurement_interval(interval_in_seconds);
    sensirion_sleep_usec(20000u);
    scd30_start_periodic_measurement(0);
    sensirion_sleep_usec(interval_in_seconds * 1200000);
    measure();
    while (1)
    {
        result = adc_read();
        float raw_voltage = result * CONVERSION_FACTOR + calibration;
        RunningAverageBuffer[NextRunningAverage++] = raw_voltage;
        if (NextRunningAverage >= RunningAverageCount)
        {
            NextRunningAverage = 0;
        }
        voltage = 0;
        for (int i = 0; i < RunningAverageCount; ++i)
        {
            voltage += RunningAverageBuffer[i];
        }
        voltage /= RunningAverageCount;
        voltage = round(voltage * 10) / 10;
        voltage = voltage > 4.19 ? 4.19 : voltage;
        battery = mapfloat(voltage, 2.8, 4.19, 0, 100);
        printf("V: %f\nB:%i\n", voltage, battery);
        int16_t err;
        uint16_t data_ready = 0;
        if (readValues)
        {

            err = scd30_get_data_ready(&data_ready);
            if (err != NO_ERROR)
            {
                printf("Error reading data_ready flag: %i\n", err);
            }
            if (!data_ready)
            {
                printf("Timeout waiting for data_ready flag %i\n", data_ready);
                setMeasureTimer();
                continue;
            }
            err =
                scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
            relative_humidity = roundf(relative_humidity * 100) / 100;
            temperature = roundf(temperature * 100) / 100;
            co2_ppm = roundf(co2_ppm * 100) / 100;
            if (err != NO_ERROR)
            {
                printf("error reading measurement\n");
            }
            else
            {

                printf("measured co2 concentration: %0.2f ppm, "
                       "measured temperature: %0.2f degreeCelsius, "
                       "measured humidity: %0.2f %%RH"
                       "Battery: %i\n"
                       "voltage: %f\n",
                       co2_ppm, temperature, relative_humidity, battery, voltage);

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
                if(temperature < 0){
                    lv[12] = 0x31;
                }
                else{
                    lv[12] = 0x30;
                }
                encode(128, lv, 13, 16, abs(temperature));
                encode(128, lv, 16, 18, relative_humidity);
                encode(128, lv, 18, 20, battery);
                uart_write_blocking(uart0, lv, 20);
                sleep_ms(200);
                uart_puts(uart0, "AT");
            }
            setMeasureTimer();
        }
        display.clear();
        display1.clear();
        if (screenOn)
        {
            char buf[10];
            char batt_buff[10];
            int battery_length = 0;
            sprintf(buf, "%08.2f", co2_ppm);
            battery_length = sprintf(batt_buff, "%i%%", battery);
            drawText(&display, font_16x32, buf, 0, 16);
            drawText(&display, font_12x16, "PPM", 92, 48);
            drawText(&display, font_8x8, batt_buff, 100 - (8 * (battery_length)), 2);
            pico_ssd1306::drawRect(&display, 104, 1, 127, 10, pico_ssd1306::WriteMode::INVERT);
            for (int r = 4; r < 8; r++)
            {
                for (int c = 102; c < 104; c++)
                {
                    display.setPixel(c, r);
                }
            }
            for (int r = 3; r < 9; r++)
            {
                int btStart = 106 + (((10 - battery / 10)) * 2);
                for (int c = btStart; c < 126; c++)
                {
                    display.setPixel(c, r);
                }
            }
            char tmp_buff[9];
            char rh_buff[10];
            int tmp_length = sprintf(tmp_buff, "%05.2f", temperature);
            sprintf(rh_buff, "%05.2f%%RH", relative_humidity);
            drawText(&display1, font_12x16, tmp_buff, 0, 16);
            drawText(&display1, font_5x8, "o", (tmp_length * 12) + 2, 14);
            drawText(&display1, font_12x16, "C", (tmp_length * 12) + 7, 16);
            pico_ssd1306::drawLine(&display1, 0, 34, 127, 34);
            drawText(&display1, font_12x16, rh_buff, 0, 38);
        }
        display.sendBuffer();
        display1.sendBuffer();
    }

    // Infinte While Loop to wait for interrupt
}

// Core 0 Main Code
int main(void)
{

    // set_sys_clock_48mhz();
    stdio_init_all();
    multicore_launch_core1(core1_entry); // Start core 1 - Do this before any interrupt configuration

    gpio_init(16);
    gpio_set_dir(16, GPIO_IN);
    gpio_pull_down(16);
    // setting display properties

    while (1)
    {
        if (gpio_get(16))
        {
            multicore_fifo_push_blocking(1);
            sleep_ms(20000);
            multicore_fifo_push_blocking(0);
        }
    }
}