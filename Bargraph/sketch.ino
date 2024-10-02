#define potenciometro 34
#define LED_R 12
#define LED_G 14
#define LED_B 27

// Ler o valor analógico do potenciometro
int valorAnalogico = 0;
int valorVolts = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // ENTRADAS
  pinMode(potenciometro, INPUT);

  // SAÍDAS
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // ADC
  analogReadResolution(10);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000); // this speeds up the simulation

  valorAnalogico = analogRead(potenciometro);
  valorVolts = analogReadMilliVolts(potenciometro);

  Serial.printf("ADC: %d bits -->  %dmV\n", valorAnalogico, valorVolts);

  bargraph();
}

void bargraph()
{
  // ADC RANGE
  // ADC  --> LED_R | LED_G | LED_B
  // 1023 -->   1   |   0   |   0
  // 341  -->   0   |   1   |   0
  // 0    -->   0   |   0   |   1

  if(valorAnalogico < 341)
  {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);
  }
  else if(valorAnalogico >= 341 && valorAnalogico < 1000)
  {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);
  }
  else
  {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  }
}

