//Librerias
#include <Arduino.h>
#include <FirebaseT.h>
#include <Balanza.h>
#include <MotoBomba.h>
#include <PID_v1.h>

//Variables - Controlador PID -> Relación peso - flujo de agua
double referencia_pid, entrada_pid, salida_pid;

//Definición del objeto PID así como de las constantes

double Kp=6.1, Ki=0.3, Kd=0.75; //
double Kp_prueba;
PID myPID(&entrada_pid, &salida_pid, &referencia_pid, Kp, Ki, Kd, DIRECT);

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 14;

double valor_pasado;

//Variable - calibración
int pendiente = 627.643083;

//¿Conectar a WiFi y Firebase?
bool wireless_mode = false;

bool toggle = false;

bool first_time = true;
int time_mark;
bool volumen_alcanzado = false;

//Instanciación de objetos
Balanza balanza(DOUT, CLK);
MotoBomba motoBomba(pump_PWM);
//FirebaseT firebaseHandler;

//Aux Vars
int timer_start;
int timer_print = 2000;
int ref_print = 0;
double last_10_volumes[10];
int cycle_10 = -1;


//Funciones AUX
void saveLast10(double value, int cycle_10){
  last_10_volumes[cycle_10] = value; 
}

double getArrayMean(double* arrayToMean){
  double len = sizeof(arrayToMean);
  double sum = 0;

  for(int i = 0; i < len; i++){
    sum += arrayToMean[i];
  }
  return sum/len;
}



void setup() {


  //Baudiaje de la comunicación serial
  Serial.begin(115200);

  //Modo wireless (WiFi + Firebase)
  /*if(wireless_mode){
    firebaseHandler.setWiFi("Chocolala", "eusebio8425");
    firebaseHandler.setFirebase("AIzaSyAiaVAvazH57Fce9ZsE9Cm06BxBMsoJXXw", "https://chocolala-e8384-default-rtdb.firebaseio.com/");
  }
*/
  //Configuración de la motobomba y pwm
  motoBomba.setUp();
  motoBomba.set_speed(0);

  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);


  //Inicialización del controlador PID
  myPID.SetOutputLimits(45, 255);
  entrada_pid = balanza.get_volumen(1, 10);
  referencia_pid = 150; //Valor de volumen (mL) a alcanzar en estado estable

  if(referencia_pid<250){
    Kp_prueba=35;
  } 
  else if(referencia_pid<300){
    Kp_prueba=18;
  }
  else{
    Kp_prueba=6.2;
  }
  myPID.SetTunings(Kp_prueba,0.1,(1.5/100)*referencia_pid);
  
   

  myPID.SetMode(AUTOMATIC);

  timer_start = millis(); 

  valor_pasado = entrada_pid;
}


void loop() {

  //Actualización del PID
  entrada_pid = balanza.get_volumen(1, 1);

  if(cycle_10 + 1 >= 10){
    cycle_10 = -1;
  }
    cycle_10 = cycle_10 + 1;
  saveLast10(entrada_pid, cycle_10);

  int timer_actual = millis();
  int tiempo = timer_actual - timer_start;

  
  float valor_actual = entrada_pid;

  if(abs(valor_actual-valor_pasado) < 20 && !volumen_alcanzado){


    if(abs(entrada_pid - referencia_pid) < 0.1 || (entrada_pid - referencia_pid) > 0){
      volumen_alcanzado = true;
    }

    if(true){
      myPID.Compute();
      motoBomba.set_speed(salida_pid);
    }
  else{
    motoBomba.set_speed(0);
  }


  if(toggle){
    valor_pasado = entrada_pid;
    toggle = false;
  }  
  if(!toggle){
    toggle = true;
  }
}
else{
    motoBomba.set_speed(0);

    
}

if(timer_print >= 2000){
  double last_10_mean = getArrayMean(last_10_volumes);
  Serial.print(last_10_mean);
  Serial.print(",");
  Serial.println(tiempo);

  ref_print = millis();
  }

timer_print = millis() - ref_print; 

  
  delay(20);


}










