#ifndef DRONE_MOTOR_H
#define DRONE_MOTOR_H

class Motor
{
public:
  static constexpr auto MinSpeed = 0.0;
  static constexpr auto MaxSpeed = 1.0;

  void attach(int pin);
  void setSpeed(double speed);
  void update();

  [[nodiscard]] int pin()             const { return m_pin; }
  [[nodiscard]] bool attached()       const { return m_pin != NOT_A_PIN; }
  [[nodiscard]] double currentSpeed() const { return m_actualSpeed; }
  [[nodiscard]] double targetSpeed()  const { return m_targetSpeed; }

private:
  int m_pin = NOT_A_PIN;
  double m_targetSpeed = 0;
  double m_actualSpeed = 0;
};

#endif // DRONE_MOTOR_H