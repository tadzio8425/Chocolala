#ifndef Stepper_h
#define Stepper_h
#include "Arduino.h"

class Stepper{
    public:
        Stepper(int DIR, int STEP, int MS1, int MS2, int MS3, int ENCODER){}

        void setUp();
        void runToRPM();

        int getPrevRPM();
        void setPrevRPM(int newPrev);

        int getDesiredRPM();
        void setDesiredRPM(int newDesired);

        int getRealRPM();

        void resetControlVal();

        void setMicrostep(int MS1val, int MS2val, int MS3val);
        float RPMToDelay(int rpm);
        double motorCount(int valorEncoder, double timeInterval);
        void microPulse(int del);
        void getRPM();

    private:

        //Pines
        int _DIR;
        int _STEP;
        int _MS1;
        int _MS2;
        int _MS3;
        int _ENCODER;

        //RPM
        double _desiredRPM = 0;
        int _realRPM;
        double _prevRPM = 0; 

        //Variavbles de control
        int _valorEncoder = 0;
        int _control_val = 0;
        double _time_difference;
        double _initialEncoderTime;
        int _prevEncoderValue = 1;
        double _encoder_counter = 0;
        int _count_control = 0;
};


#endif