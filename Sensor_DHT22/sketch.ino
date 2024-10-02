// Bibliotecas
#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>

// DHT22
#define DHT_SDA_Pin 15

// LCD 16x2 (I2C)
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2

// Objetos
DHTesp dht22;
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Custom char
uint8_t customChar[8] = {
  B11111, B01110, B00100, B00000, B00000, B00100, B01110, B11111
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // PinMode

  // Setup 'dht22'
  dht22.setup(DHT_SDA_Pin, DHTesp::DHT22);

  // Setup 'lcd'
  lcd.init();
  lcd.backlight();
  lcd.createChar(3, customChar);
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000); // this speeds up the simulation

  // Instanciando um objeto 'data'
  TempAndHumidity data = dht22.getTempAndHumidity();

  Serial.println("T: " + String(data.temperature, 2) + "ºC");
  Serial.println("U: " + String(data.humidity, 1) + "%");

  // Escrevendo a informação no display lcd
  // Temperatura
  lcd.setCursor(0,0); // Col, Row
  lcd.print(String(data.temperature, 2) + " \xDF" + "C");

  // Umidade
  lcd.setCursor(0,1);
  lcd.print(String(data.humidity, 1) + " %rh");

  // Exibe o caractere customizado
  lcd.setCursor(14, 1);
  lcd.print("\x03");
  
}
