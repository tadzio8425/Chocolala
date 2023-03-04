//Librerias
#include <Arduino.h>
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>
#include <PID_v1.h>
#include <VolumeController.h>
#include <ChocolalaREST.h>

using namespace ChocolalaREST;

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 14;

//Variable - calibración
int pendiente = 627.643083;
double referencia_volumen = 150; //Valor deseado en mL

//¿Conectar a WiFi y Firebase?
bool wireless_mode = false;

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

  //Modo wireless (WiFi + Firebase)
  if(wireless_mode){
    iotHandler.setWiFi("Chocolala", "eusebio8425");
    iotHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
    iotHandler.setWebServer(80);
    ChocolalaREST::linkServer(iotHandler.getServerPointer());
  }

  //Configuración de la motobomba y pwm
  motoBomba.setUp();
  
  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);

  //Inicialización del controlador PID
  controladorVolumen.setUp(referencia_volumen); //Ref
}


void loop() {

  //Actualización obligatoria del controlador
  controladorVolumen.update();

  //Vincular variables del backend con el API REST
  ChocolalaREST::linkVolume((balanza.get_volumenPointer()));
  ChocolalaREST::linkWeight((balanza.get_weightPointer()));

  //Vincular el API REST con el servidor WiFi
  iotHandler.addGETtoWeb("/volume", ChocolalaREST::GETVolume);
  iotHandler.addGETtoWeb("/weight", ChocolalaREST::GETWeight);

  delay(20);

}










