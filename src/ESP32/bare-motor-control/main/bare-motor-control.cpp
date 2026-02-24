#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

class MotorPWM {
private:
    ledc_mode_t _mode;
    ledc_channel_t _channel;
    uint32_t _max_duty;

public:
    MotorPWM(int gpio_pin, ledc_channel_t channel) 
        : _mode(LEDC_LOW_SPEED_MODE), _channel(channel) {
        
        _max_duty = 1023; // 10-bit resolution (2^10 - 1)

        // 1. Timer Configuration
        ledc_timer_config_t timer_conf = {
            .speed_mode      = _mode,
            .duty_resolution = LEDC_TIMER_10_BIT,
            .timer_num       = LEDC_TIMER_0,
            .freq_hz         = 5000,
            .clk_cfg         = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_conf);

        // 2. Channel Configuration
        ledc_channel_config_t chan_conf = {
            .gpio_num   = gpio_pin,
            .speed_mode = _mode,
            .channel    = _channel,
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = LEDC_TIMER_0,
            .duty       = 0,
            .hpoint     = 0
        };
        ledc_channel_config(&chan_conf);
    }

    void setSpeed(uint32_t duty) {
        if (duty > _max_duty) duty = _max_duty;
        ledc_set_duty(_mode, _channel, duty);
        ledc_update_duty(_mode, _channel);
    }
};

extern "C" void app_main(void) {
    // Initialize motor on GPIO 13, Channel 0
    MotorPWM myMotor(13, LEDC_CHANNEL_0);

    int current_duty = 0;
    int fade_amount = 25;

    while (true) {
        printf("Current Speed: %d\n", current_duty);
        myMotor.setSpeed(current_duty);

        current_duty += fade_amount;

        // Reverse fade direction
        if (current_duty <= 0 || current_duty >= 1023) {
            fade_amount = -fade_amount;
        }

        // FreeRTOS delay (100ms)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
