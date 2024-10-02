#define LED 22
#define botao 2
#define potenciometro 35

int duty = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // ENTRADAS
  pinMode(botao, INPUT);
  pinMode(potenciometro, INPUT);

  // SAÍDAS
  pinMode(LED, OUTPUT);

  // ADC
  analogReadResolution(10);

  // Configurando o PWM
  // Função utilizada para configurar o LEDC
  // Parametros:
  //1. pin select LEDC pin.
  //2. freq select frequency of pwm.
  //3. resolution select resolution for LEDC channel.
  //4. channel select LEDC channel.
  ledcAttachChannel(LED, 2000, 10, 0);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(200); // this speeds up the simulation

  // Ler o valor analógico do potenciometro
  int valorAnalogico = analogRead(potenciometro);
  int valorVolts = analogReadMilliVolts(potenciometro);

  // Função para determinar o 'duty' do pino LEDC
  ledcWrite(LED, valorAnalogico);
  
  Serial.printf("ADC: %d bits -->  %dmV\n", valorAnalogico, valorVolts);

}
