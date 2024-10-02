#define led 18

// Criando um timer de nome 'tempo'
hw_timer_t *Tempo = NULL;

// Metodo da interrupcao
void IRAM_ATTR ciclo1s()
{
  digitalWrite(led, !digitalRead(led));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Saidas
  pinMode(led, OUTPUT);

  // Inicializar o Timer
  // TimerBegin(numero_do_timer, frequencia, direçao)
  // Onde:
  // Numero do Timer, varia entre os valores 0 e 3
  // Frequencia, valor em MHz (80 padrão)
  // Direção, 'true' (crescente), 'false' (decrescente)
  Tempo = timerBegin(1000000);

  // timerAttachInterrupt(Nome_do_Timer, Metodo_ISR, Habilita)
  // Onde:
  // Nome do Timer
  // Método de Callback
  // Habilita a interrupção
  timerAttachInterrupt(Tempo, &ciclo1s);

  // Quando haverá a interrupção
  // Parametros do Timer
  // Tout = TimerTicks * (Prescaler / APB_CLK)
  // 1s = TimerTicks * (80 / 80.000.000)
  // 1s = TimerTicks * 0,000001 
  // TimerTicks = 1s / 0,0000001
  // TimerTicks = 1.000.000
  timerAlarm(Tempo, 1000000, true, 0);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}