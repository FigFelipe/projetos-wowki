#define potenciometro 36

int valorAnalogico = 0;
float tensao = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
 
  // Define a resolução do ADC 
  // (0-1023 bits) --> (0V - 3.3V)
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000); // this speeds up the simulation

  valorAnalogico = analogRead(potenciometro);

  tensao = 3.3 * valorAnalogico / 1023;

  Serial.print("ADC: ");
  Serial.print(valorAnalogico);
  Serial.print(" --> ");
  Serial.print(tensao, 2);
  Serial.println(" Vdc");


}
