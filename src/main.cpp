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

#include <OneWire.h>
#include <DallasTemperature.h>


using namespace ChocolalaREST;

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 33;

//Variable - Pin OneWire para el sensor de temperatura
#define ONE_WIRE_BUS 32

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

//Instanciación de objetos
Balanza balanza(DOUT, CLK);
MotoBomba motoBomba(pump_PWM);

Balanza* balanzaPointer = &balanza;
MotoBomba* motoBombaPointer = &motoBomba;

VolumeController controladorVolumen(balanzaPointer, motoBombaPointer);
VolumeController* controladorVolPointer = &controladorVolumen;

FirebaseT iotHandler;

OneWire oneWire(ONE_WIRE_BUS);	

// Conexion al sensor de temperatura con la libreria DallasTemperature
DallasTemperature sensors(&oneWire);

//RPM DESEADO (Pasar valor a la otra ESP32)
int* rpmPointer;
int desiredRPM = 0;

int* realRpmPointer;
int realRPM = 0; // Variable to hold incoming integer valu

double* temperaturePointer;
double defaultTemp = 0;

int value = 0;

void setup() {

  Wire.begin(21, 22); // SDA pin = GPIO 21, SCL pin = GPIO 22

  //Baudiaje de la comunicación serial
  Serial.begin(115200);
  referenciaPointer = &default_ref; 
  waterFillPointer = &defaultFill;
  calibrarPointer = &defaultCalibrar;
  stopPointer = &defaultStop;
  rpmPointer = &desiredRPM;
  realRpmPointer = &realRPM;
  temperaturePointer = &defaultTemp;

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
  ChocolalaREST::linkRPM((rpmPointer));
  ChocolalaREST::linkRealRPM((realRpmPointer));
  ChocolalaREST::linkTemperature((temperaturePointer));


  //Vincular el API REST con el servidor WiFi

  //GET
  iotHandler.addGETtoWeb("/volume", ChocolalaREST::GETVolume);
  iotHandler.addGETtoWeb("/weight", ChocolalaREST::GETWeight);
  iotHandler.addGETtoWeb("/reference", ChocolalaREST::GETReference);
  iotHandler.addGETtoWeb("/waterFill", ChocolalaREST::GETWaterFill);
  iotHandler.addPUTtoWeb("/realRpm", ChocolalaREST::GETRealRPM);
  iotHandler.addPUTtoWeb("/temperature", ChocolalaREST::GETTemperature);
  iotHandler.addGETtoWeb("/", ChocolalaREST::GETAll);

  //PUT
  iotHandler.addPUTtoWeb("/reference", ChocolalaREST::PUTReference);
  iotHandler.addPUTtoWeb("/waterFill", ChocolalaREST::PUTWaterFill);
  iotHandler.addPUTtoWeb("/calibrate", ChocolalaREST::PUTCalibrate);
  iotHandler.addPUTtoWeb("/stop", ChocolalaREST::PUTStop);
  iotHandler.addPUTtoWeb("/rpm", ChocolalaREST::PUTRpm);

  (iotHandler.getServerPointer())->begin();

}


void loop() {

  //Medición de temperatura
  sensors.requestTemperatures(); 
  *temperaturePointer = sensors.getTempCByIndex(0);

  //¿Calibrar manualmente?
  if(*calibrarPointer){
    balanza.calibrar(pendiente);
    *calibrarPointer = false;
      Serial.println("Calibrado."); 
  }

  //Actualización obligatoria del controlador
  if(*waterFillPointer){
    controladorVolumen.setReference(referenciaPointer);
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
    desiredRPM = 0;
    Serial.println("Stop!");

    *stopPointer = false;
  }

  //Se revisan las posibles peticiones REST del cliente
  (iotHandler.getServerPointer())->handleClient(); 


  //Envía el RPM deseado a la segunda ESP32 (slave)
  Wire.beginTransmission(8); 
  Wire.write((uint8_t*)&desiredRPM, sizeof(desiredRPM)); 
  Wire.endTransmission();

  //Se solicita el RPM Real
  Wire.requestFrom(8, sizeof(value));
  unsigned long startMillis = millis();
  while (Wire.available() < sizeof(value)) {
    if (millis() - startMillis > 100) {
      break;
    }
  }
  if (Wire.available() == sizeof(value)) {
    Wire.readBytes((uint8_t*)&value, sizeof(value)); 
    *realRpmPointer = value;
  }

}