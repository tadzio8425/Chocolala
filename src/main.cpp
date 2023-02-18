//Librerias
#include <Arduino.h>
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>
#include <PID_v1.h>

//Variables - Controlador PID -> Relación peso - flujo de agua
double referencia_pid, entrada_pid, salida_pid;

//Definición del objeto PID así como de las constantes
double Kp=2, Ki=5, Kd=1;
PID myPID(&entrada_pid, &salida_pid, &referencia_pid, Kp, Ki, Kd, DIRECT);

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

  //Baudiaje de la comunicación serial
  Serial.begin(9600);

  //Modo wireless (WiFi + Firebase)
  if(wireless_mode){
    firebaseHandler.setWiFi("Chocolala", "eusebio8425");
    firebaseHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
  }

  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);
  timer_start = millis(); 

  //Inicialización del controlador PID
  entrada_pid = balanza.get_peso(10);
  referencia_pid = 200; //Valor a alcanzar en estado estable
  myPID.SetMode(AUTOMATIC);

}

void loop() {

  //Actualización del PID
  entrada_pid = balanza.get_peso(10);
  myPID.Compute();

  int timer_actual = millis();
  int tiempo = timer_actual - timer_start;

  balanza.print_peso(); //Imprime el peso en el formato: "Peso: yyy g"
  Serial.print(" g, Tiempo: ");
  Serial.print(tiempo);
  Serial.println(" ms");
  delay(100);
}





