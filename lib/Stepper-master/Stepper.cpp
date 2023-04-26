#include "Arduino.h"
#include <Stepper.h>


Stepper::Stepper(int DIR, int STEP, int MS1, int MS2, int MS3, int ENCODER) {
	_DIR = DIR;
    _STEP = STEP;
    _MS1 = MS1;
    _MS2 = MS2;
    _MS3 = MS3;
    _ENCODER = ENCODER;
}

void Stepper::setUp(){


    pinMode(_DIR, OUTPUT);
    pinMode(_STEP, OUTPUT);

    pinMode(_MS1, OUTPUT);
    pinMode(_MS2, OUTPUT);
    pinMode(_MS3, OUTPUT);
    
    pinMode(_ENCODER, INPUT);
    digitalWrite(_DIR,HIGH);

    _initialEncoderTime = millis();
}

void Stepper::runToRPM(){
  int del = RPMToDelay(_desiredRPM);

  if(del != -1){
    microPulse(del); 
  }
}

void Stepper::setMicrostep(int ms1Val, int ms2Val, int ms3Val){
  digitalWrite(_MS1, ms1Val);
  digitalWrite(_MS2, ms2Val);
  digitalWrite(_MS3, ms3Val);
}

float Stepper::RPMToDelay(int rpm){

  float result = 0;

  if(rpm > 150){
    setMicrostep(LOW, LOW, LOW);
    result = -1391*log(rpm) + 8912.1;
  }
  else if(rpm > 75){
    setMicrostep(HIGH, LOW, LOW);
    result =  152215*pow(rpm,-1.004);
  }
  else if(rpm > 35.7){
    setMicrostep(LOW, HIGH, LOW);
    result = 79504*pow(rpm,-1.017);
  }
  else if(rpm > 19){
    setMicrostep(HIGH, HIGH, LOW);
    result = 40254*pow(rpm, -1.024);
  }
  else if(rpm > 0){
    setMicrostep(HIGH, HIGH, HIGH);
    result = 18595*pow(rpm,-1);
  }
  else{
    return -1;
  }

  return result + _control_val;
}


double Stepper::motorCount(int valorEncoder, double timeInterval){


  double actual_time = millis();

    _time_difference = actual_time - _initialEncoderTime;

  if(valorEncoder == 1 && valorEncoder != _prevEncoderValue){
    _encoder_counter += 1;
  }

  _prevEncoderValue = valorEncoder; 

  if(_time_difference > timeInterval){
    double RPM = ((_encoder_counter/12.0)/(timeInterval/1000.0))*60.0;
    _initialEncoderTime = millis();
    _encoder_counter = 0;
    return RPM;
  }
  else{
    return 0;
  }
}


void Stepper::getRPM(){
    _valorEncoder = digitalRead(_ENCODER);
    double upRPM = motorCount(_valorEncoder, 5000);
    if(upRPM != 0){
      _count_control += 1;

      if(_desiredRPM - _realRPM > 0){

        if(_desiredRPM - _realRPM > 2){
          _control_val -= 10;
        }
        else{
          _control_val -= 3;
        }
        
      }
      else if(_desiredRPM - _realRPM < 0){
        if(_desiredRPM - _realRPM < -2){
           _control_val += 10;
        }
        else{
          _control_val += 3;
        }
       
      }

      if(_desiredRPM - _realRPM == 1){
          _control_val += 1;
      }

      if(_desiredRPM - _realRPM == -1){
          _control_val -= 1;
        }

      _realRPM = upRPM;
  }
}


void Stepper::microPulse(int del){

    if(del < 0){
      del = 0;
    }
    getRPM();
    digitalWrite(_STEP, HIGH);
    delayMicroseconds(del);
    digitalWrite(_STEP, LOW);
}

int Stepper::getPrevRPM(){
    return _prevRPM;
}
void Stepper::setPrevRPM(int newPrev){
    _prevRPM = newPrev;
}

int Stepper::getDesiredRPM(){
    return _desiredRPM;
}
void Stepper::setDesiredRPM(int newDesired){
    _desiredRPM = newDesired;
}

int Stepper::getRealRPM(){
    return _realRPM;
}

void Stepper::resetControlVal(){
    _control_val = 0;
}

