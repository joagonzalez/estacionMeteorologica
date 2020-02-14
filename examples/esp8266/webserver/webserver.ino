#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server; // webserver object instance

char* ssid = "X Files";
char* password = "mulderscully2011";
uint8_t pin_led = 16;

void setup(){
  pinMode(pin_led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.println("Local IP Address: ");
  Serial.println(WiFi.localIP());

  // hello endpoint
  server.on("/hello",[](){
    server.send(200, "text/plain", "hello world!");
    }
  );

  // toggle led endpoint
  server.on("/toggle", toggleLed);

  server.begin();
}

void loop(){
  server.handleClient();
    
}

void toggleLed(){
  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send(204, "blinkeo ok!");
  Serial.println("blinkeo ok!");
}
