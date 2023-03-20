#include "Arduino.h"
#include "FirebaseT.h"
#include <Firebase_ESP_Client.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

void FirebaseT::setWiFi(char* wifi_ssid, char* wifi_password, bool wokwi)
{

  if (!wokwi){
    /* Put IP Address details */
    IPAddress local_ip(192,168,1,1);
    IPAddress gateway(192,168,1,1);
    IPAddress subnet(255,255,255,0);

    WiFi.softAP(wifi_ssid, wifi_password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  }

  else{
    Serial.print("Connecting to WiFi");
    WiFi.begin("Wokwi-GUEST", "", 6);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println(" Connected!");
  }

}

void FirebaseT::setFirebase(char* api_key, char* database_url)
{
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;
  unsigned long sendDataPrevMillis = 0;
  int count = 0;
  bool signupOK = false;

  config.api_key = api_key;
  config.database_url = database_url;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void FirebaseT::setWebServer(int port)
{
  _serverPointer = new WebServer(port);
  _serverPointer->enableCORS();
}


/***Función que agrega un path REST de tipo GET al servidor WEB
 *  @param path: Debe ser del tipo "\variableName"
 *  @param function: Función a llamar cuando se ejecute el GET
 * ***/
void FirebaseT::addGETtoWeb(String path, void (*function)()){
  _serverPointer->on(path, HTTP_GET, *function);
}

void FirebaseT::addPUTtoWeb(String path, void (*function)()){
  _serverPointer->on(path, HTTP_PUT, *function);
}

WebServer* FirebaseT::getServerPointer(){
  return _serverPointer;
}
