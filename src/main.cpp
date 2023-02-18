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
  Serial.begin(115200);

  //Modo wireless (WiFi + Firebase)
  if(wireless_mode){
    firebaseHandler.setWiFi("Chocolala", "eusebio8425");
    firebaseHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
  }

  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);

  //Configuración de la motobomba y pwm
  motoBomba.setUp();

  //Inicialización del controlador PID
  entrada_pid = balanza.get_volumen(1, 10);
  referencia_pid = 200; //Valor de volumen (mL) a alcanzar en estado estable
  myPID.SetMode(AUTOMATIC);

  timer_start = millis(); 
}

void loop() {

  //Actualización del PID
  entrada_pid = balanza.get_volumen(1, 10);
  int timer_actual = millis();
  int tiempo = timer_actual - timer_start;
  myPID.Compute();
  motoBomba.set_speed(salida_pid);
  Serial.println(salida_pid);

  Serial.print(entrada_pid);
  Serial.print(",");
  Serial.println(tiempo);
  delay(100);
}





