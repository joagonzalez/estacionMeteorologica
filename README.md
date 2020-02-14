## Estacion Meteorológica
El proyecto consiste en la utilización del conversor analógico-digial (ADC) de la placa EDU-CIAA con el objetivo de muestrear sensores de temperatura, humedad y presión. 

Esta información será enviada al UART (universal async receiver-transmitter), el cual enviará datos de forma serial con payloads de hasta 8 bits. Como el ADC funciona con datos de 10 bits, se truncarán datos no significativos para ser enviados correctamente por el UART. A la placa EDU-CIAA, se conectará un módulo ESP8266 que recibirá por comuniación serie los datos recolectados con los sensores, esta información será transmitida via Wi-Fi a una base de datos de series de tiempo (influxDB) que luego será graficada con el objetivo de monitorear en tiempo real las variables deseadas.


![Figura 1](https://github.com/joagonzalez/unsam-meteorologia/blob/master/doc/diagrama_high_level_v2.png)


### Notas
En caso de tener error de permisos a la hora de grabar sketch en esp8266, dar permisos correspodientes al puerto sobre el que se esta trabajando

```
chmod 777 /dev/ttyUSB0
```



### Material de consulta
- https://www.youtube.com/watch?v=G6CqvhXpBKM (programming esp8266 with arduino ide)
- https://www.youtube.com/watch?v=6-RXqFS_UtU&list=PLNFq0T6Z3JPsMWtVgmqPnpIu41LNMDPiA&index=8 (serial communication esp8266 and arduino)
- https://www.youtube.com/watch?v=8xqgdXvn3yw&list=PLNFq0T6Z3JPsHwzvPQncip-kMIdWpnnip&index=7 (api access esp8266)
- http://www.circuitbasics.com/basics-uart-communication/
- https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#softwareserial
- https://www.youtube.com/playlist?list=PLNFq0T6Z3JPsHwzvPQncip-kMIdWpnnip (esp8266 projects)
- https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer (webserver esp8266 arduino library)