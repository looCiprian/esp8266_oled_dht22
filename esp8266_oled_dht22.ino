#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define OLED_HEIGHT 48
#define OLED_WIDTH  64

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

int offset_graph = 0;

// vettore per il grafico temperatura
int temperature[64];
// indice per aggiornare man mano il vettore
int temperature_pos = 0;


// vettore per il grafico umidità
int humidit[64];
// indice per aggiornare man mano il vettore
int humidit_pos = 0;

// pixel massimo e minio in cui si può disegnare il grafico
int data_max_display = 47;
int data_min_display = 8;

void setup()   {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
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

}

void loop() {

  //delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  h = h - 3;
  float t = dht.readTemperature();
  t = t - 3;
  float hic = dht.computeHeatIndex(t, h, false);
  String temp = String(t);
  String hum = String(h);
  String heat = String(hic);

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
  delay(2000);


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
  delay(2000);


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
  delay(2000);


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
  delay(2000);


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
  delay(2000);

}
