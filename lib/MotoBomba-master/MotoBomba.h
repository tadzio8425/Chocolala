
#ifndef MotoBomba_h
#define MotoBomba_h
#include "Arduino.h"

class MotoBomba
{
  public:
    MotoBomba(int pwm_pin);
    void set_speed(int duty_cycle); //0-255 duty cycle

  private:
    int _pwm_pin;
};

#endif