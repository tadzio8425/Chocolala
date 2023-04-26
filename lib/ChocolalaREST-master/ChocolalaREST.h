#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include <VolumeController.h>

// JSON data buffer
StaticJsonDocument<500> jsonDocument;
char buffer[500];


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
  bool* _waterFillPointer;
  bool* _calibratePointer;
  bool* _stopPointer;
  int* _rpmPointer;
  int* _realRPMPoiner;

  VolumeController* _controllerVolPointer;

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

    void GETWaterFill(){
    Serial.println("Get waterFill");
    create_json("waterFill", (*_waterFillPointer), "bool");
    (*_serverPointer).send(200, "application/json", buffer);
  }

    void GETRealRPM(){
    Serial.println("Get realRPM");
    create_json("realRPM", (*_realRPMPoiner), "RPM");
    (*_serverPointer).send(200, "application/json", buffer);
  }

  void GETAll(){
    jsonDocument.clear();
    add_json_object("reference", (*_referencePointer), "mL");
    add_json_object("waterFill", (*_waterFillPointer), "bool");
    add_json_object("volume", (*_volumePointer), "mL"); 
    add_json_object("weight", (*_weightPointer), "g");
    add_json_object("desiredRPM", (*_rpmPointer), " RPM");
    add_json_object("realRPM", (*_realRPMPoiner), " RPM");
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
      (*_controllerVolPointer).setReference((_referencePointer));
      (*_controllerVolPointer).setUp();

      //Se responde con la nueva referencia
      GETReference();
      
  }

  void PUTWaterFill(){
    if ((*_serverPointer).hasArg("plain") == false) {
      Serial.println("Esperaba un booleano, recibí: nada.");
    }
      String body = (*_serverPointer).arg("plain");
      deserializeJson(jsonDocument, body);
      
      //Obtener referencia
      (*_waterFillPointer) = (bool) jsonDocument["waterFill"];
    
      //Se responde con la nueva referencia
      GETWaterFill();
  }


  void PUTCalibrate(){
        if ((*_serverPointer).hasArg("plain") == false) {
      Serial.println("Esperaba un booleano, recibí: nada.");
    }
      String body = (*_serverPointer).arg("plain");
      deserializeJson(jsonDocument, body);
      
      //Obtener si se desea calibrar
      (*_calibratePointer) = (bool) jsonDocument["calibrate"];

      GETAll();
  }

  void PUTStop(){
        if ((*_serverPointer).hasArg("plain") == false) {
      Serial.println("Esperaba un booleano, recibí: nada.");
    }
      String body = (*_serverPointer).arg("plain");
      deserializeJson(jsonDocument, body);
      
      //Obtener si se desea calibrar
      (*_stopPointer) = (bool) jsonDocument["stop"];

      GETAll();
  }


  void PUTRpm(){
        if ((*_serverPointer).hasArg("plain") == false) {
      Serial.println("Esperaba un double, recibí: nada.");
    }
      String body = (*_serverPointer).arg("plain");
      deserializeJson(jsonDocument, body);
      
      //Obtener si se desea calibrar
      (*_rpmPointer) = (double) jsonDocument["rpm"];

      GETAll();
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

  void linkWaterFill(bool* waterFillPointer){
      _waterFillPointer = waterFillPointer;
  }

  void linkCalibrate(bool* calibratePointer){
      _calibratePointer = calibratePointer;
  }

  void linkStop(bool* stopPointer){
      _stopPointer = stopPointer;
  }

  void linkControladorVol(VolumeController* controllerVolPointer){
    _controllerVolPointer = controllerVolPointer;
  }

  void linkRPM(int* rpmPointer){
    _rpmPointer = rpmPointer;
  }
}
