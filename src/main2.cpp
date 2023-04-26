//Librerias
#include <Arduino.h>
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
#include <Stepper.h>

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

//Instanciación STEPPER
Stepper stepper(DIR, STEP, MS1, MS2, MS3, ENCODER);

void receiveEvent(int numBytes) {
  int value = 0;
  while (Wire.available() >= sizeof(value)) {
    Wire.readBytes((uint8_t*)&value, sizeof(value));

    if (value != stepper.getPrevRPM()) {
      stepper.setPrevRPM(stepper.getDesiredRPM());
      stepper.setDesiredRPM(value);
      stepper.resetControlVal();}
  }
}

void requestEvent() {
  int realRPM = stepper.getRealRPM();
  Wire.write((uint8_t*)&realRPM, sizeof(realRPM));
}

void setup(){
  Serial.begin(115200);

  pinMode(espMaster, INPUT); 
  stepper.setUp();

  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent);
}

void loop(){
  stepper.runToRPM();
}