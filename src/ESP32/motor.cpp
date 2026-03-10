#include "motor.h"

#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <esp_err.h>

#include <Arduino.h>

namespace
{

inline uint32_t us_to_duty(uint32_t us)
{
    return (us * MAX_DUTY) / 20000; // 20ms period (50Hz)
}

} // namespace

Motor::Motor(int pin, int channel)
  : m_gpio(pin)
  , m_channel(channel)
{
  init();
}

void Motor::setDuty(int duty)
{
    Serial.printf("setting duty %d p/c(%d, %d)\n", duty, m_gpio, m_channel);

    ledc_set_duty_and_update(LEDC_MODE, (ledc_channel_t) m_channel, duty, MAX_DUTY);
    /*
    ledc_set_duty(LEDC_MODE, (ledc_channel_t)m_channel, duty);

    Serial.printf("updading duty\n");
    ledc_update_duty(LEDC_MODE, (ledc_channel_t)m_channel);*/

    m_duty = duty;

    Serial.println("exit");
}

void Motor::init()
{
    ledc_channel_config_t ch = {};
    ch.gpio_num = m_gpio;
    ch.speed_mode = LEDC_MODE;
    ch.channel = (ledc_channel_t)m_channel;
    ch.timer_sel = LEDC_TIMER;
    ch.duty = us_to_duty(1000); // minimum throttle
    ch.hpoint = 0;
    ledc_channel_config(&ch);
}
