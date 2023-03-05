#include "Arduino.h"
#include <MotoBomba.h>
#include <PID_v1.h>
#include <Balanza.h>
#include <VolumeController.h>


//Funciones AUX
double getArrayMean(double* arrayToMean){
  double len = sizeof(arrayToMean);
  double sum = 0;

  for(int i = 0; i < len; i++){
    sum += arrayToMean[i];
  }
  return sum/len;
}

VolumeController::VolumeController(Balanza* balanzaPointer, MotoBomba* motoBombaPointer)
{
    _myPIDPointer = new PID(&_entrada_pid, &_salida_pid, &_referencia_pid, _Kp, _Ki, _Kd, DIRECT);

    _balanzaPointer = balanzaPointer;
    _motoBombaPointer = motoBombaPointer;
}

void VolumeController::setUp(double* refPointer){
  (*_myPIDPointer).SetOutputLimits(45, 255);
  _entrada_pid = (*_balanzaPointer).get_volumen(1, 10);
  _referencia_pid = (*refPointer); //Valor de volumen (mL) a alcanzar en estado estable

  if(_referencia_pid<250){
    _Kp_prueba=35;
  } 
  else if(_referencia_pid<300){
    _Kp_prueba=18;
  }
  else{
    _Kp_prueba=6.2;
  }
  (*_myPIDPointer).SetTunings(_Kp_prueba,0.1,(1.5/100)*_referencia_pid);
  

  (*_myPIDPointer).SetMode(AUTOMATIC);

  _timer_start = millis(); 

  _valor_pasado = _entrada_pid;
}

void VolumeController::update(){
  //Actualización del PID
  _entrada_pid = (*_balanzaPointer).get_volumen(1, 1);
  updateLast10Volumes();


  int timer_actual = millis();
  _tiempo = timer_actual - _timer_start;

  
  float valor_actual = _entrada_pid;

  if(abs(valor_actual-_valor_pasado) < 20 && !_volumen_alcanzado){


    if(abs(_entrada_pid - _referencia_pid) < 0.1 || (_entrada_pid - _referencia_pid) > 0){
      _volumen_alcanzado = true;
    }

    if(true){
      (*_myPIDPointer).Compute();
      (*_motoBombaPointer).set_speed(_salida_pid);
    }
  else{
    (*_motoBombaPointer).set_speed(0);
  }


  if(_toggle){
    _valor_pasado = _entrada_pid;
    _toggle = false;
  }  
  if(!_toggle){
    _toggle = true;
  }
}
else{
    (*_motoBombaPointer).set_speed(0);

    
}

}


void VolumeController::updateLast10Volumes(){
    if(_cycle_10 + 1 >= 10){
        _cycle_10 = -1;
    }
    _cycle_10 = _cycle_10 + 1;
    _last_10_volumes[_cycle_10] = _entrada_pid; 
}

void VolumeController::printVolumeMean(int print_interval){
if(_timer_print >= print_interval){
  double last_10_mean = getArrayMean(_last_10_volumes);
  Serial.print(last_10_mean);
  Serial.print(",");
  Serial.println(_tiempo);

  _ref_print = millis();
  }

  _timer_print = millis() - _ref_print; 
}

