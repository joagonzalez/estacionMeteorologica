#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>

#define WIFI_SSID "X Files"
#define WIFI_PASSWORD "password"
#define INFLUXDB_URL "http://116.203.129.206:8086"
#define INFLUXDB_DB_NAME "unsam_sensores"
uint8_t pin_led = 16;
int message = 0;
int channel = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

Point sensor1("temperature");
Point sensor2("co2");
Point sensor3("pressure");

void setup() {

  Serial.begin(115200);
  pinMode(pin_led, OUTPUT);
  // Connect WiFi
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME);

  // Add constant tags - only once
  sensor1.addTag("device", DEVICE);
  sensor1.addTag("SSID", WiFi.SSID());
  
  sensor2.addTag("device", DEVICE);
  sensor2.addTag("SSID", WiFi.SSID());

  sensor3.addTag("device", DEVICE);
  sensor3.addTag("SSID", WiFi.SSID());


  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  boolean messageReady = false;
  //String message = "";
  // Store measured value into point
  sensor1.clearFields();
  sensor2.clearFields();
  sensor3.clearFields();

  // read data from edu-ciaa
  while(messageReady == false){
    if(Serial.available()){
        // message = Serial.readString();
        message = Serial.read();
        messageReady = true;
        digitalWrite(pin_led, !digitalRead(pin_led));
    }
  }
  // if previous value was 1, 2 or 3 then was channel info
  if(channel == 1 || channel == 2 || channel == 3){  
    // set sensor tag
    if(channel == 1){
      sensor1.addField("rssi", WiFi.RSSI());
      sensor1.addField("channel", channel);
      sensor1.addField("value", message);
      sensor1.addField("uptime", millis());

      // Print what are we exactly writing
      Serial.print("Writing: ");
      Serial.println(sensor1.toLineProtocol());

      // Write point
      if (!client.writePoint(sensor1)) {
        Serial.print("InfluxDB write failed for sensor 1: ");
        Serial.println(client.getLastErrorMessage());
      }
    }else if(channel == 2){
      sensor2.addField("rssi", WiFi.RSSI());
      sensor2.addField("channel", channel);
      sensor2.addField("value", message);
      sensor2.addField("uptime", millis());

      // Print what are we exactly writing
      Serial.print("Writing: ");
      Serial.println(sensor2.toLineProtocol());

      // Write point
      if (!client.writePoint(sensor2)) {
        Serial.print("InfluxDB write failed for sensor 1: ");
        Serial.println(client.getLastErrorMessage());
      }
    }else{
      sensor3.addField("rssi", WiFi.RSSI());
      sensor3.addField("channel", channel);
      sensor3.addField("value", message);
      sensor3.addField("uptime", millis());

      // Print what are we exactly writing
      Serial.print("Writing: ");
      Serial.println(sensor3.toLineProtocol());

      // Write point
      if (!client.writePoint(sensor3)) {
        Serial.print("InfluxDB write failed for sensor 1: ");
        Serial.println(client.getLastErrorMessage());
      }
    }
 
    // If no Wifi signal, try to reconnect it
    if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED))
      Serial.println("Wifi connection lost");

    Serial.print("Datos recibidos EDU-CIAA ADC channel ");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(message);
  }
  channel = message;
  //Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
}