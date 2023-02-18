//Librerias
#include <Arduino.h>
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 14;

//Variable - calibración
int pendiente = 627.643083;

//¿Conectar a WiFi y Firebase?
bool wireless_mode = false;

//Instanciación de objetos
Balanza balanza(DOUT, CLK);
MotoBomba motoBomba(pump_PWM);
FirebaseT firebaseHandler;

//Aux Vars
int timer_start;

void setup() {
  Serial.begin(9600);

  if(wireless_mode){
    firebaseHandler.setWiFi("Chocolala", "eusebio8425");
    firebaseHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
  }

  balanza.calibrar(pendiente);

  timer_start = millis(); 
}

void loop() {

  motoBomba.set_speed(255);

  int timer_actual = millis();
  int tiempo = timer_actual - timer_start;

  balanza.print_peso(); //Imprime el peso en el formato: "Peso: yyy g"
  Serial.print(" g, Tiempo: ");
  Serial.print(tiempo);
  Serial.println(" ms");
  delay(100);
}





