#include "Arduino.h"
#include "HX711.h"
#include <Balanza.h>

Balanza::Balanza(int dout, int clk)
{
}

void Balanza::calibrar(int pendiente){
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  delay(100);
  Serial.println("No ponga ningun objeto sobre la balanza");

  tare(50);  //El peso actual es considerado "Tara".
  set_scale(pendiente); //Valor de la pendiente de la regresión

  Serial.println("¡¡¡LISTO PARA PESAR!!!"); 
  delay(100); 
}

float Balanza::get_peso(int mean){
    int peso = get_units(mean); // Entrega el peso actualment medido en gramos
      if(peso<0) peso = peso*-1;
    return peso;
}


float Balanza::get_volumen(float densidad, int mean){
    return get_peso(mean)/densidad;
}


void Balanza::print_peso(){
    Serial.print("Peso: ");
    Serial.print(get_peso(1), 2);
}