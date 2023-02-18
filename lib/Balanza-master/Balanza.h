
#ifndef Balanza_h
#define Balanza_h
#include "Arduino.h"
#include "HX711.h"

class Balanza : public HX711
{
  public:
    Balanza(int dout, int clk);
    
    void calibrar(int pendiente);

    float get_peso(int mean);

    void print_peso();
  private:
};

#endif