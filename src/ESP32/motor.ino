#include "motor.h"

namespace {

template<typename T>
constexpr T clamp(T value, T low, T high)
{
  return max(low, min(value, high));
}

static_assert(clamp( -1, 0, 255) ==   0);
static_assert(clamp(  0, 0, 255) ==   0);
static_assert(clamp(  1, 0, 255) ==   1);
static_assert(clamp(254, 0, 255) == 254);
static_assert(clamp(255, 0, 255) == 255);
static_assert(clamp(256, 0, 255) == 255);

constexpr int speedToDuty(double speed)
{
  const auto t = (1 + clamp(speed, 0.0, 1.0)) / 20;
  return t * pwmRange + 0.5;
}

static_assert(speedToDuty(0.0)  == 51);
static_assert(speedToDuty(0.1)  == 56);
static_assert(speedToDuty(0.5)  == 77);
static_assert(speedToDuty(0.9)  == 97);
static_assert(speedToDuty(1.0) == 102);

}

void Motor::attach(int pin)
{
  m_pin = pin;

  pinMode(m_pin, OUTPUT);

  static constexpr int resolution = ceil(log2(pwmRange));

  ledcAttach(m_pin, pwmFreq, resolution);

  setSpeed(0);
}

void Motor::setSpeed(double speed)
{
  if (speed < 0 || speed > 51)
    return;

  ledcWrite(m_pin, speedToDuty(speed));
  m_speed = speed;
}