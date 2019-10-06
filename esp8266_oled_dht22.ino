#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

int rele_d1 = D8;   // Digital pin connected to the rele_d1 D1

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define OLED_HEIGHT 48
#define OLED_WIDTH  64

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
/* Set these to your desired credentials. */
const char *ssid = "essid_wemos";
const char *password = "12345678";

ESP8266WebServer server(80);

// Temperature variables
float h = 0;
float t = 0;
float hic = 0;
String temp = "";
String hum = "";
String heat = "";

// Refresh display time
int startTime = 0;
// Token per scegliere cosa visualizzare
int roundToken = 0;


#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
//#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
//#define WHITE           0xFFFF

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


// Vettore per il grafico temperatura
int temperature[64];
// Indice per aggiornare man mano il vettore
int temperature_pos = 0;


// Vettore per il grafico umidità
int humidit[64];
// Indice per aggiornare man mano il vettore
int humidit_pos = 0;

// Pixel massimo e minio in cui si può disegnare il grafico
int data_max_display = 47;
int data_min_display = 8;


void handleRoot() {
  String page = "";
  page += "<h1>Temp: " + temp + "</h1>";
  page += "<br>";
  page += "<h1>Humidity: " + hum + "</h1>";
  page += "<br>";
  page += "<h1>Heat: " + heat + "</h1>";
  page += "<meta http-equiv='refresh' content='5;url=/'>";
  server.send(200, "text/html", page);
}


void setup()   {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  pinMode(rele_d1, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  //wifi server init
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();

  //display default, we'll generate the high voltage from the 3.3v line internally! (neat!) init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

  //dht init
  dht.begin();
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  // LOADING
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.println("Loading.");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.println("Loading..");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.println("Loading...");
  display.display();
  delay(1000);

  //Inizializzo i vettori a 0
  for (int i = 0; i < 64; i++) {
    temperature[i] = 0;
  }

  for (int i = 0; i < 64; i++) {
    humidit[i] = 0;
  }
  ///


  startTime = millis();
}


void displayTemp() {
  // Stampo temperatura
  display.clearDisplay();
  display.drawRoundRect(0, 0, OLED_WIDTH, OLED_HEIGHT, 8, WHITE);
  display.setCursor(19, 10);
  display.setTextSize(1);
  display.println("Temp:");
  display.setCursor(4, OLED_HEIGHT / 2);
  display.setTextSize(2);
  display.println(temp);
  display.display();

}

void displayHum() {
  // Stampo umidità
  display.clearDisplay();
  display.drawRoundRect(0, 0, OLED_WIDTH, OLED_HEIGHT, 8, WHITE);
  display.setCursor(22, 10);
  display.setTextSize(1);
  display.println("Hum:");
  display.setCursor(4, OLED_HEIGHT / 2);
  display.setTextSize(2);
  display.println(hum);
  display.display();

}

void displayHeat() {
  // Stampo temperatura percepita
  display.clearDisplay();
  display.drawRoundRect(0, 0, OLED_WIDTH, OLED_HEIGHT, 8, WHITE);
  display.setCursor(19, 10);
  display.setTextSize(1);
  display.println("Heat:");
  display.setCursor(4, OLED_HEIGHT / 2);
  display.setTextSize(2);
  display.println(heat);
  display.display();
}

void displayGraphTemp() {

  // Disegno grafico temperatura
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Temp graph");
  // Aggiorno array temperatua
  temperature[temperature_pos] = (int)t;
  // Per la lunghezza dell'array stampo
  for (int i = 0; i < 64; i++) {
    // Mappo la temperatura e la inverto per il display
    int temperature_display = map(temperature[i], 0, 40, data_max_display, data_min_display);
    display.drawLine(i, temperature_display, i, data_max_display, WHITE);
  }
  temperature_pos++;
  // Array circolare
  if (temperature_pos > 63) {
    temperature_pos = 0;
  }
  display.display();
}

void displayGraphHum() {
  // Disegno grafico umidità
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Hum graph");
  // Aggiorno array umidità
  humidit[humidit_pos] = (int)h;
  // Per la lunghezza dell'array stampo
  for (int i = 0; i < 64; i++) {
    // Mappo l'umidità e la inverto per il display
    int humidit_display = map(humidit[i], 0, 100, data_max_display, data_min_display);
    display.drawLine(i, humidit_display, i, data_max_display, WHITE);
    //display.drawPixel(i,humidit_display,WHITE);
  }
  humidit_pos++;
  // Array circolare
  if (humidit_pos > 63) {
    humidit_pos = 0;
  }
  display.display();
}

void checkRele() {
  // Gestisco il rele
  if (t > 29) {
    digitalWrite(BUILTIN_LED, HIGH);
    digitalWrite(rele_d1, HIGH);
  } else {
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(rele_d1, LOW);
  }
}

void getData() {

  // Reading temperature or humidity takes about 250 milliseconds!
  h = dht.readHumidity();
  h = h - 3;
  t = dht.readTemperature();
  t = t - 2;
  hic = dht.computeHeatIndex(t, h, false);
  temp = String(t);
  hum = String(h);
  heat = String(hic);
  Serial.print("Temp:");
  Serial.print(temp);
  Serial.print("Hum:");
  Serial.print(hum);
  Serial.print("Heat:");
  Serial.println(heat);

  //Controllo lettura dei sensori
  if (isnan(h) || isnan(t)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Sensors");
    display.setTextSize(2);
    display.setCursor(0, OLED_HEIGHT / 2);
    display.print("FAIL");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Retry in 4 sec.");
    display.display();
    delay(4000);
    return;
  }

}

void loop() {

  int loopTime = millis();

  server.handleClient();

  if ((loopTime - startTime) > 2000) {
    switch (roundToken) {
      case 0:
        getData();
        displayTemp();
        checkRele();
        startTime = loopTime;
        roundToken = 1;
        break;
      case 1:
        getData();
        displayHum();
        startTime = loopTime;
        roundToken = 2;
        break;
      case 2:
        getData();
        displayHeat();
        checkRele();
        startTime = loopTime;
        roundToken = 3;
        break;
      case 3:
        getData();
        displayGraphTemp();
        checkRele();
        startTime = loopTime;
        roundToken = 4;
        break;
      case 4:
        getData();
        displayGraphHum();
        checkRele();
        startTime = loopTime;
        roundToken = 0;
        break;
    }

  }

  //  getData();
  //  displayHeat();
  //  getData();
  //  displayGraphTemp();
  //  getData();
  //  displayGraphHum();

  //  delay(2000);


}
