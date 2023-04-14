//Librerias
#include <Arduino.h>
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>
#include <PID_v1.h>
#include <VolumeController.h>
#include <ChocolalaREST.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>


using namespace ChocolalaREST;

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 14;

//Variables - Pin Motor Stepper
#define DIR 32
#define STEP 33
#define ENCODER 34

//HARDWIRED A GND - NO CONECTAR!
#define MS1 15
#define MS2 2
#define MS3 4

double FULL_STEP = 1;

//VARIABLES CONTROL MOTOR
double encoder_counter = 0;

//Variable - calibración
int pendiente = 627.643083;
double* referenciaPointer; //Valor deseado en mL
double default_ref = 0; //Referencia default (mL)

//¿Conectar a WiFi y Firebase?
bool wireless_mode = true;

//¿Iniciar el proceso de vertir el agua?
bool* waterFillPointer;
bool defaultFill  = false;

//¿Calibrar nuevamente?
bool* calibrarPointer;
bool defaultCalibrar = false;

//Botón de stop
bool* stopPointer;
bool defaultStop = false;

//Timer del contador de encoder
double initialEncoderTime;
bool toggleEncoderTimer = false;
int prevEncoderValue = 1;

//Instanciación de objetos
Balanza balanza(DOUT, CLK);
MotoBomba motoBomba(pump_PWM);

Balanza* balanzaPointer = &balanza;
MotoBomba* motoBombaPointer = &motoBomba;

VolumeController controladorVolumen(balanzaPointer, motoBombaPointer);
VolumeController* controladorVolPointer = &controladorVolumen;

FirebaseT iotHandler;

TaskHandle_t motorTask;

std::vector<float> microStep_list = {0.0625, 0.125, 0.25, 0.5, 1};


bool pulseToggle = false;
//Pulsos del motor
double errorTol = 0.1;
float delayDeseado = 3/2;
int RPMDeseado = 60;
int RPMargen = 3;


float RPMToDelay(int rpm){
  return -31.73*log(rpm) + 136.38;
}


void setMicrostep(int ms1Val, int ms2Val, int ms3Val){
  digitalWrite(MS1, ms1Val);
  digitalWrite(MS2, ms2Val);
  digitalWrite(MS3, ms3Val);
}

void motorPulse(int del){
    digitalWrite(STEP, HIGH);
    delay(del);
    digitalWrite(STEP, LOW);
  }


double motorCount(int valorEncoder, double timeInterval){


  double actual_time = millis();

  double time_difference = actual_time - initialEncoderTime;

  if(valorEncoder == 1 && valorEncoder != prevEncoderValue){
    encoder_counter += 1;
  }

  prevEncoderValue = valorEncoder; 

  if(time_difference > timeInterval){
    double RPM = ((encoder_counter/12)/(timeInterval/1000))*60;
    initialEncoderTime = millis();
    encoder_counter = 0;
    return RPM;
  }
  else{
    return 0;
  }
}


void smartPulse(float step){
      if(step == 1.0){
        setMicrostep(LOW, LOW, LOW);
        Serial.println("FULL");
        motorPulse(1);
      }
      else if(step == 0.5){
        setMicrostep(HIGH, LOW, LOW);
        Serial.println("HALF");
        motorPulse(1);
      }
      else if(step == 0.25){
        setMicrostep(LOW, HIGH, LOW);
        Serial.println("QUARTER");
        motorPulse(1);
      }
      else if(step == 0.125){
        setMicrostep(HIGH, HIGH, LOW);
        Serial.println("EIGHT");
        motorPulse(1);
      }
      else if(step == 0.0625){
        setMicrostep(HIGH, HIGH, HIGH);
        Serial.println("SIXTEENTH");
        motorPulse(1);
      }
      else if(step == 0){
        Serial.println("DEAD");
        delay(1);
      }

}


#include <iostream>
#include <vector>

std::vector<double> getSteps(float RPM, double stepWindow, double tolerancia) {

    double desiredStep = (RPM*360)/(1.8*60*1000);

    std::vector<double> responseSteps = {};

  	std::vector<double> microsteps = {1, 0.5, 0.25, 0.125, 0.0625};

    for(int i = 0; i < stepWindow; i++){
      for(double step: microsteps){
        if(step/stepWindow <= desiredStep){
          responseSteps.push_back(step);
          desiredStep -= step/stepWindow;
          break;
        }
      }
    }

    if(desiredStep > tolerancia){
      return getSteps(RPM, stepWindow+1, tolerancia);
    }

    return responseSteps;
}


void runSteps(std::vector<double> stepList){

    for(double step: stepList){
      smartPulse(step);
    }
}

void motorTaskCode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  std::vector<double> steps = getSteps(82.5, 1, 0.01);

  for(;;){
    runSteps(steps);

    int valorEncoder = digitalRead(ENCODER);

    double stepsInTime = motorCount(valorEncoder, 5000);
    if(stepsInTime != 0){
      
      Serial.println(stepsInTime);
    }
  } 
}


void setup() {

    xTaskCreatePinnedToCore(
                    motorTaskCode,   /* Task function. */
                    "Task1",     /* name of task. */
                    40000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &motorTask,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
  
  pinMode(ENCODER, INPUT);
  digitalWrite(DIR,HIGH);

  //Baudiaje de la comunicación serial
  Serial.begin(115200);
  referenciaPointer = &default_ref; 
  waterFillPointer = &defaultFill;
  calibrarPointer = &defaultCalibrar;
  stopPointer = &defaultStop;

  //Modo wireless (WiFi + Firebase)
  if(wireless_mode){
    iotHandler.setWiFi("Chocolala", "chocolele", false);
    //iotHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
    iotHandler.setWebServer(80);
    ChocolalaREST::linkServer(iotHandler.getServerPointer());
  }

  //Configuración de la motobomba y pwm
  motoBomba.setUp();
  
  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);

  //Inicialización del controlador PID
  controladorVolumen.setReference(referenciaPointer);
  controladorVolumen.setUp(); //Ref

  //Vincular los apuntadores a las variables del backend con el API REST
  ChocolalaREST::linkVolume((balanza.get_volumenPointer()));
  ChocolalaREST::linkWeight((balanza.get_weightPointer()));
  ChocolalaREST::linkReference((referenciaPointer));
  ChocolalaREST::linkWaterFill((waterFillPointer));
  ChocolalaREST::linkCalibrate((calibrarPointer));
  ChocolalaREST::linkStop((stopPointer));
  ChocolalaREST::linkControladorVol((controladorVolPointer));


  //Vincular el API REST con el servidor WiFi

  //GET
  iotHandler.addGETtoWeb("/volume", ChocolalaREST::GETVolume);
  iotHandler.addGETtoWeb("/weight", ChocolalaREST::GETWeight);
  iotHandler.addGETtoWeb("/reference", ChocolalaREST::GETReference);
  iotHandler.addGETtoWeb("/waterFill", ChocolalaREST::GETWaterFill);
  iotHandler.addGETtoWeb("/", ChocolalaREST::GETAll);

  //PUT
  iotHandler.addPUTtoWeb("/reference", ChocolalaREST::PUTReference);
  iotHandler.addPUTtoWeb("/waterFill", ChocolalaREST::PUTWaterFill);
  iotHandler.addPUTtoWeb("/calibrate", ChocolalaREST::PUTCalibrate);
  iotHandler.addPUTtoWeb("/stop", ChocolalaREST::PUTStop);

  (iotHandler.getServerPointer())->begin();

  initialEncoderTime = millis();

}


void loop() {


  //¿Calibrar manualmente?
  if(*calibrarPointer){
    balanza.calibrar(pendiente);
    *calibrarPointer = false;
      Serial.println("Calibrado."); 
  }

  //Actualización obligatoria del controlador
  if(*waterFillPointer){
    controladorVolumen.update();
  }
  else{
    (*balanzaPointer).get_volumen(1, 1);
  }

  //Botón de STOP
  if(*stopPointer){
    *waterFillPointer = false;
    motoBomba.set_speed(0);
    *referenciaPointer = 0;
    Serial.println("Stop!");

    *stopPointer = false;
  }

  //controladorVolumen.printVolumeMean(2000);
  (iotHandler.getServerPointer())->handleClient();

 
  
  
}











