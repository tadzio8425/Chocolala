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
PID myPID(&entrada_pid, &salida_pid, &referencia_pid, Kp, Ki, Kd, DIRECT);

//Variables - Pines Balanza
#define DOUT  26
#define CLK  27

//Variable - Pin PWM Bomba
int pump_PWM = 14;

float valor_pasado;

//Variable - calibración
int pendiente = 627.643083;

//¿Conectar a WiFi y Firebase?
bool wireless_mode = false;

bool toggle = false;

bool first_time = true;
int time_mark;

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

  //Configuración de la motobomba y pwm
  motoBomba.setUp();
  motoBomba.set_speed(0);

  //Calibración inicial de la pendiente (Tare + scale -> Offset + pendiente)
  balanza.calibrar(pendiente);


  //Inicialización del controlador PID
  myPID.SetOutputLimits(85, 255);
  entrada_pid = balanza.get_volumen(1, 10);
  referencia_pid = 200; //Valor de volumen (mL) a alcanzar en estado estable

  if(referencia_pid <= 50){
    myPID.SetTunings(16,3,0.5);
    myPID.SetOutputLimits(180, 255);
  }
  else if(referencia_pid <= 100){
    myPID.SetTunings(6.1,0.32,0.75);
  }
  else if(referencia_pid <= 200){
    myPID.SetTunings(6.2,0.1,2.69);
  }
  else if(referencia_pid <= 300){
    myPID.SetTunings(6.2,0.1,3.82);
  }
  else if(referencia_pid <= 400){
    myPID.SetTunings(6.2,0.1,4.1);
  }
  else if(referencia_pid <=500){
    myPID.SetTunings(6.2,0.1,6.1);
  }
  else if(referencia_pid<=600){
    myPID.SetTunings(6.2,0.1,7.2);
  }
  
   

  myPID.SetMode(AUTOMATIC);

  timer_start = millis(); 

  valor_pasado = entrada_pid;
}

void loop() {

  //Actualización del PID
  entrada_pid = balanza.get_volumen(1, 1);
  int timer_actual = millis();
  int tiempo = timer_actual - timer_start;


  
  float valor_actual = entrada_pid;

  if(abs(valor_actual-valor_pasado) < 20){
    if(true){
      myPID.Compute();
      motoBomba.set_speed(salida_pid);
    }
    else{

      if(first_time){
        time_mark  = millis();
        first_time = false;
      }
      

      if(entrada_pid < referencia_pid){
        int time_since_mark = (timer_actual - time_mark);
        if (time_since_mark < 1000){
            motoBomba.set_speed(255);
        }
        else{
            time_mark = millis();
            motoBomba.set_speed(190);
        }


        }
      else{
        motoBomba.set_speed(85);
      }
    }}
  else{
    motoBomba.set_speed(85);
  }
  



  if(toggle){
    valor_pasado = entrada_pid;
    toggle = false;
  }  
  if(!toggle){
    toggle = true;
  }

  Serial.print(entrada_pid);
  Serial.print(",");
  Serial.println(tiempo);
  delay(20);
}





