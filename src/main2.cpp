//Librerias
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>
#include <PID_v1.h>
#include <VolumeController.h>
#include <Wire.h>

#include <ChocolalaREST.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <random>

HardwareSerial SerialPort(2); // use UART2

int control_val = 0;

//Vector de pasos
std::vector<double> steps;

//Variables - Pin Motor Stepper
#define DIR 26
#define STEP 32
#define ENCODER 13

//HARDWIRED A GND - NO CONECTAR!
#define MS1 26
#define MS2 27
#define MS3 14

//Pin analogo otra ESP32
#define espMaster 33

//Pin digital otra ESP32
#define espMasterDir 14

bool controlShift = false;

//Variables necesarias
double realRPM;
int valorEncoder = 0;
double tolerance = 0.001;
double time_difference;

//Timer del contador de encoder
double initialEncoderTime;
bool toggleEncoderTimer = false;
int prevEncoderValue = 1;
double encoder_counter = 0;

double* rpmPointer;
double desiredRPM = 0;
double prevRPM = 0; // Variable to hold previous value of desiredRPM
char stringRPM[4]; 

auto rng = std::default_random_engine {};

double FULL_STEP = 1;

int count_control = 0;

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
          realRPM = upRPM;

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


      Serial.println(upRPM);
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


void smartPulse(float step){
      if(step >= 1.0){
        setMicrostep(LOW, LOW, LOW);
        //Serial.println("FULL");
        motorPulse(1);
      }
      else if(step >= 0.5){
        setMicrostep(HIGH, LOW, LOW);
        //Serial.println("HALF");
        motorPulse(1);
      }
      else if(step >= 0.25){
        setMicrostep(LOW, HIGH, LOW);
        //Serial.println("QUARTER");
        motorPulse(1);
      }
      else if(step >= 0.125){
        setMicrostep(HIGH, HIGH, LOW);
        //Serial.println("EIGHT");
        motorPulse(1);
      }
      else if(step >= 0.0625){
        setMicrostep(HIGH, HIGH, HIGH);
        //Serial.println("SIXTEENTH");
        motorPulse(1);
      }
      else if(step >= 0){
        //Serial.println("DEAD");
        delay(1);
      }

}

std::vector<double> getSteps(float RPM, float stepWindow, float tolerancia) {

    float desiredStep = (RPM*360.0)/(1.8*60.0*1000.0);

    std::vector<double> responseSteps = {};

  	std::vector<double> microsteps = {1, 0.5, 0.25, 0.125, 0.0625, 0};

    for(int i = 0; i < stepWindow; i++){
      for(double step: microsteps){
        if((float) step/stepWindow <= desiredStep){
          responseSteps.push_back(step);
          desiredStep -= (float)step/stepWindow;
          break;
        }
      }
    }

    if(desiredStep > tolerancia && stepWindow <= 29){
      return getSteps(RPM, stepWindow+1, tolerancia);}

    return responseSteps;
}


void runSteps(std::vector<double> stepList){

    for(double step: stepList){
      smartPulse(step);
      //Serial.println(step);
    }
}

void receiveEvent(int numBytes) {
  int value = 0; // Variable to hold incoming integer value
  while (Wire.available() >= sizeof(value)) {
    Wire.readBytes((uint8_t*)&value, sizeof(value)); // Read incoming integer value

    if (value != prevRPM) {
      prevRPM = desiredRPM;
      desiredRPM = value;
      control_val = 0;}
  }

}

//a
void setup(){
  Serial.begin(115200);

  Wire.begin(8); // Address of this ESP32 = 8
  Wire.onReceive(receiveEvent); // Register receive event

  pinMode(espMaster, INPUT);

  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  
  pinMode(ENCODER, INPUT);
  digitalWrite(DIR,HIGH);


  steps = getSteps(desiredRPM, 1, tolerance);

  int controlIndex = steps.size() - 1;
  bool performControl = false;

  std::shuffle(std::begin(steps), std::end(steps), rng);
  
  initialEncoderTime = millis();
}

void loop(){

  int del = RPMToDelay(desiredRPM, control_val);

  if(del != -1){
    microPulse(del); 
  }
 
}