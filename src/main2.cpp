//Librerias
#include <Arduino.h>
#include <Wire.h>
#include <iostream>

//Pines de control del motor
#define DIR 26
#define STEP 32

//Pins de MicroStepping
#define MS1 26
#define MS2 27
#define MS3 14

//Pin del encoder
#define ENCODER 13

//Pin analogo otra ESP32
#define espMaster 33

//Pin digital otra espMasterDir
#define espMasterDir 14

//Variables necesarias
int realRPM;
int valorEncoder = 0;
double time_difference;

//Variables del encoder
double initialEncoderTime;
bool toggleEncoderTimer = false;
int prevEncoderValue = 1;
double encoder_counter = 0;

double desiredRPM = 0;
double prevRPM = 0; 

int count_control = 0;
int control_val = 0;

void setMicrostep(int ms1Val, int ms2Val, int ms3Val){
  digitalWrite(MS1, ms1Val);
  digitalWrite(MS2, ms2Val);
  digitalWrite(MS3, ms3Val);
}

float RPMToDelay(int rpm, int controlVal){

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

  return result + control_val;
}



double motorCount(int valorEncoder, double timeInterval){


  double actual_time = millis();

    time_difference = actual_time - initialEncoderTime;

  if(valorEncoder == 1 && valorEncoder != prevEncoderValue){
    encoder_counter += 1;
  }

  prevEncoderValue = valorEncoder; 

  if(time_difference > timeInterval){
    double RPM = ((encoder_counter/12.0)/(timeInterval/1000.0))*60.0;
    initialEncoderTime = millis();
    encoder_counter = 0;
    return RPM;
  }
  else{
    return 0;
  }
}


void getRPM(){
    valorEncoder = digitalRead(ENCODER);
    double upRPM = motorCount(valorEncoder, 5000);
    if(upRPM != 0){
      count_control += 1;

      if(desiredRPM - realRPM > 0){

        if(desiredRPM - realRPM > 2){
          control_val -= 10;
        }
        else{
          control_val -= 3;
        }
        
      }
      else if(desiredRPM - realRPM < 0){
        if(desiredRPM - realRPM < -2){
           control_val += 10;
        }
        else{
          control_val += 3;
        }
       
      }

      if(desiredRPM - realRPM == 1){
          control_val += 1;
      }

      if(desiredRPM - realRPM == -1){
          control_val -= 1;
        }

      realRPM = upRPM;
  }
}

void motorPulse(int del){
    getRPM();
    digitalWrite(STEP, HIGH);
    delay(del);
    digitalWrite(STEP, LOW);
  }

void microPulse(int del){

    if(del < 0){
      del = 0;
    }
    getRPM();
    digitalWrite(STEP, HIGH);
    delayMicroseconds(del);
    digitalWrite(STEP, LOW);
  }


void receiveEvent(int numBytes) {
  int value = 0;
  while (Wire.available() >= sizeof(value)) {
    Wire.readBytes((uint8_t*)&value, sizeof(value));

    if (value != prevRPM) {
      prevRPM = desiredRPM;
      desiredRPM = value;
      control_val = 0;}
  }
}

void requestEvent() {
  Wire.write((uint8_t*)&realRPM, sizeof(realRPM));
}

void setup(){
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent);

  pinMode(espMaster, INPUT);

  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  
  pinMode(ENCODER, INPUT);
  digitalWrite(DIR,HIGH);

  initialEncoderTime = millis();
}

void loop(){

  int del = RPMToDelay(desiredRPM, control_val);

  if(del != -1){
    microPulse(del); 
  }
 
}