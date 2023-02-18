#include "Arduino.h"
#include <MotoBomba.h>

MotoBomba::MotoBomba(int pwm_pin)
{
    _pwm_pin = pwm_pin;

    //Variables PWM
    const int freq = 10000;
    const int pwmChannel = 0;
    const int resolution = 8;
    int dutyCycle = 200;

    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(pwm_pin, pwmChannel);
}

void MotoBomba::set_speed(int duty_cycle){
      ledcWrite(_pwm_pin, duty_cycle);
}

