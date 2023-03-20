#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#ifndef FirebaseT_h
#define FirebaseT_h

class FirebaseT
{
  public:
    void setWiFi(char* wifi_ssid, char* wifi_password, bool wokwi);
    void setFirebase(char* api_key, char* database_url);
    void setWebServer(int port);
    void addGETtoWeb(String path, void (*function)());
    void addPUTtoWeb(String path, void (*function)());
    WebServer* getServerPointer();
  private:
    WebServer* _serverPointer;
};

#endif