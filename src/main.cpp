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

bool pulseToggle = false;
//Pulsos del motor
double errorTol = 0.1;
float delayDeseado = 1;
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
    setMicrostep(LOW, LOW, LOW);
    digitalWrite(STEP, HIGH);
    delay(del);
    digitalWrite(STEP, LOW);
  }

void motorSmartPulse(double mean, double numPulsos, double numerator, double delayDeseado, double invDelayCalculado){

     double delayDeseadoInner = delayDeseado;
     int delayCast = 1;

    if(delayDeseado >= 2){
      delayCast = floor(delayDeseado);;
      delayDeseadoInner = 1 + delayDeseado - delayCast;
      mean = 1/delayDeseadoInner;
    }

    if(0.01 < abs(invDelayCalculado - 1 / delayDeseadoInner) && invDelayCalculado >= 1 /delayDeseadoInner){

      if(FULL_STEP - mean <= mean){
        setMicrostep(LOW, LOW, LOW);
        //Serial.println("FULL");
        numerator += FULL_STEP;
        mean = FULL_STEP - mean;
      }
      else if((FULL_STEP/(double)2) - mean <= mean){
        setMicrostep(HIGH, LOW, LOW);
        //Serial.println("HALF");
        numerator += FULL_STEP/(double)2;
        mean = (FULL_STEP/(double)2) - mean/(double)2;
      }

      else if((FULL_STEP/(double)4) - mean <= mean){
        setMicrostep(LOW, HIGH, LOW);
        //Serial.println("QUARTER");
        numerator += FULL_STEP/(double)4;
        mean = (FULL_STEP/(double)4) - mean/(double)4;
      }

      else if((FULL_STEP/(double)8) - mean <= mean){
        setMicrostep(HIGH, HIGH, LOW);
        //Serial.println("EIGHT");
        numerator += FULL_STEP/(double)8;
        mean = (FULL_STEP/(double)8) - mean/(double)8;
      }

      else if((FULL_STEP/(double)16) - mean <= mean){
        setMicrostep(HIGH, HIGH, HIGH);
        //Serial.println("SIXTEENTH");
        numerator += FULL_STEP/(double)16;
        mean = (FULL_STEP/(double)16) - mean/(double)16;
      }

      invDelayCalculado = numerator/numPulsos;
      
      //Serial.println(numPulsos);

      digitalWrite(STEP, HIGH);
      delay(delayCast);
      digitalWrite(STEP, LOW);

      motorSmartPulse(mean, numPulsos + 1, numerator, delayDeseadoInner, invDelayCalculado);
    }
    else{
      //Serial.println("Fuera");
    }
    
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

void motorTaskCode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());



  for(;;){

    motorSmartPulse(1.0/1.2, 1, 0, 1.2, 1000);

    int valorEncoder = digitalRead(ENCODER);

    double stepsInTime = motorCount(valorEncoder, 5000);
    if(stepsInTime != 0){

      Serial.println(stepsInTime);
    }
  } 
}


void setup() {

    delayDeseado = RPMToDelay(RPMDeseado);
 

    xTaskCreatePinnedToCore(
                    motorTaskCode,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
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











