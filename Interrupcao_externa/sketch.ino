#define ledVermelho 4
#define ledAmarelo 16
#define ledVerde 17

struct PushButton {
	const uint8_t pino;
	uint32_t numeroVezesPressionado;
	bool pressionado;
};

PushButton botao1 = {21, 0, false};

//variables to keep track of the timing of recent interrupts
volatile unsigned long buttonTime = 0;  
volatile unsigned long lastButtonTime = 0; 

// Função Temporizada - Millis()
unsigned long tempo = 0;

void TempoDecorrido()
{
  tempo = millis();

  Serial.printf("Tempo decorrido: %ums\n", tempo);
}

// Variável gravada na memória RAM
// pois a velocidade da RAM é maior
volatile int contador = 0;

// Interrupt Service Routine
void IRAM_ATTR isr() 
{
  // Software Debounce
  buttonTime = tempo;
  
  if((buttonTime - lastButtonTime) > 350)
  {
    botao1.numeroVezesPressionado++;
    botao1.pressionado = true;
    lastButtonTime = buttonTime;
    //contador++;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Saídas
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // Entradas
  pinMode(botao1.pino, INPUT);
  
  // Interrupçoes
  // attachInterrupt(GPIOPin, ISR, Mode);

  // Onde:
  // - GPIO, define o pino da interrupção
  // - ISR, nome da função que executa qdo a interrupção ocorre
  // - Mode, define quando a interrupção deve ser ocorrida
  //    1. LOW (qdo o pino é nível 0)
  //    2. HIGH (qdo o pino é nível 1)
  //    3. CHANGE (qdo o nível do pino é alterado)
  //    4. FALLING (qdo o pino vai do estado 1 para o estado 0)
  //    5. RISING (qdo o pino vai do estado 0 para o estado 1)
  attachInterrupt(digitalPinToInterrupt(botao1.pino), isr, FALLING);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000); // this speeds up the simulation

  Serial.printf("Botão desliga foi pressionado %u vezes\n", botao1.numeroVezesPressionado);
  botao1.pressionado = false;

  TempoDecorrido();

  // Função TOGGLE
  digitalWrite(ledVerde, !digitalRead(ledVerde));
  
}

