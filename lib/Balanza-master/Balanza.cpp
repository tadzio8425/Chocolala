#include "Arduino.h"
#include "HX711.h"
#include <Balanza.h>

void Balanza::calibrar(int pendiente){
  tare(100);  //El peso actual es considerado "Tara".
  set_scale(pendiente); //Valor de la pendiente de la regresión
  delay(100); 
}

float Balanza::get_peso(int mean){
    float peso = get_units(mean); // Entrega el peso actualment medido en gramos
      if(peso<0) peso = peso*-1;
    _peso_actual = peso;
    return _peso_actual;
}


float Balanza::get_volumen(float densidad, int mean){
    _volumen_actual = get_peso(mean)/densidad;
    return _volumen_actual;
}


void Balanza::print_peso(){
    Serial.print("Peso: ");
    Serial.print(get_peso(1), 2);
}

float* Balanza::get_volumenPointer(){
  return &_volumen_actual;
}

float* Balanza::get_weightPointer(){
  return &_peso_actual;
}