#include "Arduino.h"
#include <MotoBomba.h>

MotoBomba::MotoBomba(int pwm_pin)
{
    _pwm_pin = pwm_pin;
}


void MotoBomba::setUp(){
      //Variables PWM
      const int freq = 10000;
      const int pwmChannel = 0;
      const int resolution = 8;
      int dutyCycle = 200;

      ledcSetup(pwmChannel, freq, resolution);
      ledcAttachPin(_pwm_pin, pwmChannel);
      ledcWrite(_pwm_pin, 0); //Se inicia el ciclo como igual 0 
}

void MotoBomba::set_speed(int duty_cycle){
      ledcWrite(_pwm_pin, duty_cycle);
      _duty_cycle = duty_cycle;
}

int MotoBomba::get_duty_cycle(){
      return _duty_cycle;
}