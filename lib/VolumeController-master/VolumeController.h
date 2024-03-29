#ifndef VolumeController_h
#define VolumeController_h
#include "Arduino.h"
#include <PID_v1.h>
#include <Balanza.h>
#include <MotoBomba.h>

class VolumeController
{
  public:
    VolumeController(Balanza* balanzaPointer, MotoBomba* motoBombaPointer);
    void setUp();
    void update();
    void updateLast10Volumes();
    void printVolumeMean(int print_interval);
    void setReference(double* refPointer);
    void resetToggle();

  private:
    double _Kp, _Ki, _Kd;
    double _Kp_prueba;
    double _referencia_pid, _entrada_pid, _salida_pid;
    
    PID *_myPIDPointer;
    Balanza *_balanzaPointer;
    MotoBomba *_motoBombaPointer;

    int _timer_start;
    double _valor_pasado;

    bool _volumen_alcanzado = false;

    bool _toggle = false;

    double _last_10_volumes[10];
    int _timer_print = 60000;
    int _ref_print = 0;
    int _cycle_10 = -1;
    int _tiempo;


};


#endif