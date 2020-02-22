#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server; // webserver object instance

char* ssid = "X Files";
char* password = "mulderscully2011";
uint8_t pin_led = 16;

void setup(){
  pinMode(pin_led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.begin(9600);
  // Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.println("Local IP Address: ");
  Serial.println(WiFi.localIP());

  // root endpoint
  server.on("/", rootEndpoint);

  // hello endpoint
  server.on("/hello",[](){
    server.send(200, "text/plain", "hello world!");
    }
  );

  // toggle led endpoint
  server.on("/toggle", toggleEndpoint);

  server.begin();
}

void loop(){
  server.handleClient();
    
}

void toggleEndpoint(){
  char* msg = "endpoint: /toggle";

  digitalWrite(pin_led, !digitalRead(pin_led));
  
  server.send(200, "text/plain",msg);
  Serial.println(msg);
}

void rootEndpoint(){
    char* msg = "endpoint: /";

    // send JSON from API to arduino through serial communication
    // then, parse arduino JSON response and send it back to the client
    DynamicJsonDocument doc(1024);
    double gas = 0;
    double temperature = 0;
    boolean messageReady = false;
    String message = "";

    // esp8266 -> arduino
    doc["type"] = "request"; // assume by default, but could be a parameter in the payload
    doc["toggle"] = "true";
    serializeJson(doc, Serial);
    
    // arduino -> esp8266
    while(messageReady == false){
      if(Serial.available()){
        message = Serial.readString();
        messageReady = true;
      }
    }

    DeserializationError error = deserializeJson(doc, message); // in doc JSON save what arduino answer
    if(error){
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    temperature = doc["temperature"];
    gas = doc["gas"];

    String output = "temperature: " + String(temperature) + "\n" + "CO level: " + String(gas);

    // Send data to the server
    server.send(200, "text/plain", output);
}