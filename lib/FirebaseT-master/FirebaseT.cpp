#include "Arduino.h"
#include "FirebaseT.h"
#include <Firebase_ESP_Client.h>
#include "WiFi.h"

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

void FirebaseT::setWiFi(char* wifi_ssid, char* wifi_password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
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
