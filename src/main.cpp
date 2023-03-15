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

//Variable - calibración
int pendiente = 627.643083;
double* referenciaPointer; //Valor deseado en mL
double default_ref = 150; //Referencia default (mL)

//¿Conectar a WiFi y Firebase?
bool wireless_mode = true;

//¿Iniciar el proceso de vertir el agua?
bool* waterFillPointer;
bool defaultFill  = false;

//Instanciación de objetos
Balanza balanza(DOUT, CLK);
MotoBomba motoBomba(pump_PWM);

Balanza* balanzaPointer = &balanza;
MotoBomba* motoBombaPointer = &motoBomba;

VolumeController controladorVolumen(balanzaPointer, motoBombaPointer);

FirebaseT iotHandler;


void setup() {

  //Baudiaje de la comunicación serial
  Serial.begin(115200);
  referenciaPointer = &default_ref; 
  waterFillPointer = &defaultFill;

  //Modo wireless (WiFi + Firebase)
  if(wireless_mode){
    iotHandler.setWiFi("Chocolala", "chocolele");
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

  (iotHandler.getServerPointer())->begin();
}


void loop() {
  //Actualización obligatoria del controlador
  if(*waterFillPointer){
    controladorVolumen.update();
    controladorVolumen.setReference(referenciaPointer);
  }
  
  controladorVolumen.printVolumeMean(2000);

  (iotHandler.getServerPointer())->handleClient();

  delay(20);

}










