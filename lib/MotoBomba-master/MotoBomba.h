
#ifndef MotoBomba_h
#define MotoBomba_h
#include "Arduino.h"

class MotoBomba
{
  public:
    MotoBomba(int pwm_pin);
    void set_speed(int duty_cycle); //0-255 duty cycle
    int get_duty_cycle();
  private:
    int _pwm_pin;
    int _duty_cycle;
};

#endif