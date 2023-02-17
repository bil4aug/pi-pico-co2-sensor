#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

bool awake = false;

void awakeCallBack(void)
{
    awake = true;
}

void rtc_sleep(void)
{
    // Start on Friday 5th of June 2020 15:45:00
    datetime_t t = {
        .year = 2020,
        .month = 06,
        .day = 05,
        .dotw = 5, // 0 is Sunday, so 5 is Friday
        .hour = 15,
        .min = 45,
        .sec = 00};

    datetime_t alarm = {
        .year = 2020,
        .month = 06,
        .day = 05,
        .dotw = 5, // 0 is Sunday, so 5 is Friday
        .hour = 15,
        .min = 45,
        .sec = 10};

        rtc_init();
        rtc_set_datetime(&t);

    sleep_goto_sleep_until(&alarm, &awakeCallBack);
}

int main()
{
    set_sys_clock_khz(10000, true);
    rtc_sleep();
    while (!awake)
    {
        tight_loop_contents();
    }
    while (awake)
    {
        tight_loop_contents();
    }
}