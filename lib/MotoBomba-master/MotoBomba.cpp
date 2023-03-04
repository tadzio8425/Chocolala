#include "Arduino.h"
#include <MotoBomba.h>

MotoBomba::MotoBomba(int pwm_pin)
{
    _pwm_pin = pwm_pin;
}


void MotoBomba::setUp(){
      //Variables PWM
      const int freq = 30000;
      const int pwmChannel = 0;
      const int resolution = 8;

      _pwm_channel = pwmChannel;

      ledcSetup(pwmChannel, freq, resolution);
      ledcAttachPin(_pwm_pin, pwmChannel);
      set_speed(0);
}

void MotoBomba::set_speed(int duty_cycle){
      ledcWrite(_pwm_channel, duty_cycle);
      _duty_cycle = duty_cycle;
}

int MotoBomba::get_duty_cycle(){
      return _duty_cycle;
}