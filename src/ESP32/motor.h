#ifndef DRONE_MOTOR_H
#define DRONE_MOTOR_H

class Motor
{
public:
  explicit Motor() : m_pin(NOT_A_PIN), m_speed(0) {}

  void attach(int pin);
  void setSpeed(double speed);

  int pin() { return m_pin; }
  bool attached() { return m_pin != NOT_A_PIN; }

private:
  int m_pin;
  double m_speed;
};

#endif // DRONE_MOTOR_H