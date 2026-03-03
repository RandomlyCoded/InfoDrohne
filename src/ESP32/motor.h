#ifndef MOTOR_H
#define MOTOR_H

#define LEDC_MODE       LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_FREQ       50
#define LEDC_RES        LEDC_TIMER_16_BIT
#define MAX_DUTY        65535



class Motor
{
public:
  explicit Motor(int pin, int channel);
  explicit Motor() {}

  /*
  channel unter umständen implizit machen? In main.cpp (ledPWM) sieht es so aus als ob es einfach nur ein Counter ist. Dann würde
  
  int mkChannelId() { 
    static int i = 0;
    return i++;
  }
  channel = mkChannelId()

  gehen
  */

  void setDuty(int duty);
  int duty() { return m_duty; }
  int pin() { return m_gpio; }

private:
  void init();

  int m_gpio = -1;
  int m_channel = -1;

  int m_duty = 0;
};

#endif // MOTOR_H