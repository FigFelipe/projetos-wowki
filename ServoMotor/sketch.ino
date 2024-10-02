// Bibliotecas
#include <ESP32Servo.h>

// GPIO
#define servoPwmPin 15
#define potenciometroPosicao 32

// Variáveis globais
int valorAnalogicoPosicao = 0;
int angulo = 0;

// Objetos
Servo servo_1;

void PosicionarViaPotenciometro()
{
  // Converte o valor ADC para angulo
  angulo = valorAnalogicoPosicao * 180 / 1023;

  // Posiciona o servomotor através do angulo informado
  servo_1.write(angulo);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // ADC
  analogReadResolution(10);

  // ESP32Servo.h
  servo_1.attach(servoPwmPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation

  // Realiza a leitura do ADC
  valorAnalogicoPosicao = analogRead(potenciometroPosicao);

  // Posiciona o servomotor via potenciometro
  PosicionarViaPotenciometro();

  Serial.printf("--> ADC [Pos: %i] [Angulo: %i]\n",
                 valorAnalogicoPosicao,
                 angulo);

}
