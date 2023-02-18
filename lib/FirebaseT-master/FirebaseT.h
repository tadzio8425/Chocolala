#include "Arduino.h"
#ifndef FirebaseT_h
#define FirebaseT_h

class FirebaseT
{
  public:
    void setWiFi(char* wifi_ssid, char* wifi_password);
    void setFirebase(char* api_key, char* database_url);
  private:
};

#endif