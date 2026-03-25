#include "motor.h"

namespace {

// 2000 - (4) 5000
constexpr double maxStep = 1.0/2500.0; // trial and error; 2.5 seconds for full ramp

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

  // arm
  m_actualSpeed = 1;
  updateDuty();

  delay(500);

  m_actualSpeed = 0;
  updateDuty();

  setSpeed(0);
}

void Motor::setSpeed(double targetSpeed)
{
  m_targetSpeed = clamp(targetSpeed, MinSpeed, MaxSpeed);
}

void Motor::update()
{
  if (m_actualSpeed < m_targetSpeed) { // delta > 0
    // Serial.printf("up %d from %g + %g (%g %g %g)\n", m_pin, m_actualSpeed, maxStep, m_actualSpeed + maxStep, m_targetSpeed, MaxSpeed);
    m_actualSpeed = min({m_actualSpeed + maxStep, m_targetSpeed, MaxSpeed});
  } else if (m_actualSpeed > m_targetSpeed) { // delta < 0
    // Serial.printf("down %d\n", m_pin);
    m_actualSpeed = max({m_actualSpeed - maxStep, m_targetSpeed, MinSpeed});
  } else { // m_actualSpeed == m_targetSpeed
    // Serial.printf("ignore %d\n", m_pin);
    return;
  }

  updateDuty();
}

void Motor::updateDuty()
{
  ledcWrite(m_pin, speedToDuty(m_actualSpeed));
}