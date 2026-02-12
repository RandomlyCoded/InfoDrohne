#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_MODE       LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_FREQ       50
#define LEDC_RES        LEDC_TIMER_16_BIT
#define MAX_DUTY        65535

// Motor pins
#define M1_GPIO 32
#define M2_GPIO 27
#define M3_GPIO 25
#define M4_GPIO 26

uint32_t us_to_duty(uint32_t us)
{
    return (us * MAX_DUTY) / 20000; // 20ms period (50Hz)
}

void setup_motor(int channel, int gpio)
{
    ledc_channel_config_t ch = {};
    ch.gpio_num = gpio;
    ch.speed_mode = LEDC_MODE;
    ch.channel = (ledc_channel_t)channel;
    ch.timer_sel = LEDC_TIMER;
    ch.duty = us_to_duty(1000); // minimum throttle
    ch.hpoint = 0;
    ledc_channel_config(&ch);
}

void set_motor(int channel, uint32_t us)
{
    ledc_set_duty(LEDC_MODE, (ledc_channel_t)channel, us_to_duty(us));
    ledc_update_duty(LEDC_MODE, (ledc_channel_t)channel);
}

extern "C" void app_main(void)
{
    // Timer setup
    ledc_timer_config_t timer = {};
    timer.speed_mode = LEDC_MODE;
    timer.timer_num = LEDC_TIMER;
    timer.duty_resolution = LEDC_RES;
    timer.freq_hz = LEDC_FREQ;
    timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timer);

    // Setup 4 motors
    setup_motor(0, M1_GPIO);
    setup_motor(1, M2_GPIO);
    setup_motor(2, M3_GPIO);
    setup_motor(3, M4_GPIO);

    // ESC arm delay
    vTaskDelay(pdMS_TO_TICKS(3000));

    // -------- Individual test --------
    for(int i = 0; i < 4; i++)
    {
        set_motor(i, 1300);
        vTaskDelay(pdMS_TO_TICKS(2000));
        set_motor(i, 1000);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // -------- Group test --------
    set_motor(0, 1400);
    set_motor(1, 1400);
    set_motor(2, 1400);
    set_motor(3, 1400);

    vTaskDelay(pdMS_TO_TICKS(4000));

    // Stop all
    for(int i = 0; i < 4; i++)
        set_motor(i, 1000);
}

