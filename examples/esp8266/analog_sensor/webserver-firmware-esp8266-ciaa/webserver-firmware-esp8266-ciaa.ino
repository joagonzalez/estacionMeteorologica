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

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

Point sensor("wifi_status");

void setup() {
  Serial.begin(115200);

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
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());

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
  // String message = "";
  int message = 0;
  // Store measured value into point
  sensor.clearFields();

  // read data from edu-ciaa
  while(messageReady == false){
    if(Serial.available()){
        // message = Serial.readString();
        message = Serial.read();
        messageReady = true;
    }
  }
  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());
  sensor.addField("temperature", message);
  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());
  // If no Wifi signal, try to reconnect it
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED))
    Serial.println("Wifi connection lost");
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.print("Datos recibido de EDU-CIAA: ");
  Serial.println(message);

  //Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
}