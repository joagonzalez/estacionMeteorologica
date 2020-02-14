## Estacion Meteorológica
El proyecto consiste en la utilización del conversor analógico-digial (ADC) de la placa EDU-CIAA con el objetivo de muestrear sensores de temperatura, humedad y presión. 

Esta información será enviada al UART (universal async receiver-transmitter), el cual enviará datos de forma serial con payloads de hasta 8 bits. Como el ADC funciona con datos de 10 bits, se truncarán datos no significativos para ser enviados correctamente por el UART. A la placa EDU-CIAA, se conectará un módulo ESP8266 que recibirá por comuniación serie los datos recolectados con los sensores, esta información será transmitida via Wi-Fi a una base de datos de series de tiempo (influxDB) que luego será graficada con el objetivo de monitorear en tiempo real las variables deseadas.







## Material de consulta

- http://www.circuitbasics.com/basics-uart-communication/
- https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#softwareserial
- https://www.youtube.com/playlist?list=PLNFq0T6Z3JPsHwzvPQncip-kMIdWpnnip