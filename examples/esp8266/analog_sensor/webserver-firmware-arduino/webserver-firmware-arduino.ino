#include <ArduinoJson.h>

String message = "";
bool messageReady = false;
uint8_t pin_led = 45;

void setup(){
  Serial.begin(9600);
  pinMode(45, OUTPUT); // configure port 45 for led
}

void loop(){
  // Monitor serial communication
  while(Serial.available()){
    message = Serial.readString();
    messageReady = true;
  }

  // only if I could read a message will be formatted to JSON
  if(messageReady){
      DynamicJsonDocument doc(1024); // define JSON doc variable
      // decode message
      DeserializationError error = deserializeJson(doc, message);
      if(error){
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          messageReady = false;
          return;
      }
      if(doc["type"] == "request"){
          doc["type"] = "response";
          // get data from analog sensor
          doc["temperature"] = analogRead(A0);
          doc["gas"] = analogRead(A1);
          serializeJson(doc,Serial); //send data through Serial port in JSON
          Serial.println(" ");
          if(doc["toggle"] == "true"){
            toggleLed();
          }
      }
      messageReady = false;
  }

}

void toggleLed(){
  char* msg = "endpoint: /toggle";

  digitalWrite(pin_led, !digitalRead(pin_led));
  Serial.println(msg);
}