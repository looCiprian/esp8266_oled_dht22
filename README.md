# esp8266_oled_dht22
Simple program to display temperature, humidity, heat temperature, temperature graph and humidity graph.
Thanks to the WiFi module it is possible to connect to the AP (essid_wemos:12345678) and view all data on the following address (http://192.168.4.1).

## Hardware
1. Wemos d1 mini
2. Wemos shield DHT22
3. Wemos OLED shield (V2.0.0)

## Software
### Libraries
1. SPI.h
2. Wire.h
3. Adafruit_GFX.h
4. Adafruit_SSD1306.h
5. DHT.h
6. ESP8266WiFi.h
7. WiFiClient.h
8. ESP8266WebServer.h

## Preview

### Temperature
![](images/temp.jpg)
![](images/temp_graph.jpg)

### Humidity
![](images/hum.jpg)
![](images/hum_graph.jpg)

### Heat
![](images/heat.jpg)
