#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];


//Funciones de manejo JSON Auxiliares
void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}
 

namespace ChocolalaREST{

  WebServer* _serverPointer;
  float* _volumePointer;
  float* _weightPointer;
  double* _referencePointer;

  void add_json_object(char *tag, float value, char *unit) {
    JsonObject obj = jsonDocument.createNestedObject();
    obj["type"] = tag;
    obj["value"] = value;
    obj["unit"] = unit; 
  }

  void linkServer(WebServer* serverPointer){
      _serverPointer = serverPointer;
  }

  //MÉTODOS GET

  void GETVolume(){
    Serial.println("Get volume");
    create_json("volume", (*_volumePointer), "mL");
    (*_serverPointer).send(200, "application/json", buffer);
  }

  void GETWeight(){
    Serial.println("Get weight");
    create_json("weight", (*_weightPointer), "g");
    (*_serverPointer).send(200, "application/json", buffer);
  }

    void GETReference(){
    Serial.println("Get reference");
    create_json("reference", (*_referencePointer), "mL");
    (*_serverPointer).send(200, "application/json", buffer);
  }

  void GETAll(){
    jsonDocument.clear();
    Serial.println("Get all");
    add_json_object("reference", (*_referencePointer), "mL");
    add_json_object("volume", (*_volumePointer), "mL"); 
    add_json_object("weight", (*_weightPointer), "g");
    serializeJson(jsonDocument, buffer); 
    (*_serverPointer).send(200, "application/json", buffer);
  }


  //MÉTODOS PUT
  void PUTReference(){
    if ((*_serverPointer).hasArg("plain") == false) {
      Serial.println("Esperaba una referencia, recibí: nada.");
    }
      String body = (*_serverPointer).arg("plain");
      deserializeJson(jsonDocument, body);
      
      //Obtener referencia
      (*_referencePointer) = (double) jsonDocument["reference"];
    
      //Se responde con la nueva referencia
      GETReference();
  }



  void linkVolume(float* volumePointer){
      _volumePointer = volumePointer;
  }


  void linkWeight(float* weightPointer){
      _weightPointer = weightPointer;
  }

  void linkReference(double* referencePointer){
      _referencePointer = referencePointer;
  }
}
