// Projeto Semaforo
// Tempo(s) | Viario                | Pedestres
//   0      | Liga led vermelho     | Liga led verde
//   15     |                       | Tempo >= 15
//          |                       | Desliga led verde
//          |                       | Pisca led vermelho (1Hz)
//   20     |                       | Tempo >=20
//          |                       | Liga led vermelho
//   23     | Aguarda 3s            |
//          | Desliga led vermelho  |
//          | Liga led verde        |
//   38     | Aguarda 15s           |
//          | Desliga led verde     |
//          | Liga led amarelo      |
//   43     | Aguardar 5s           |
//          | Desliga led amarelo   |
//          | Liga led vermelho     |
//   48     | Aguardar 3s           |
//   --> LOOP!

// ---------------------------------------------------- //
//  CLASSE - BOTAO
// ---------------------------------------------------- //

// Classe Botao
struct PushButton
{
  const int pino;
  bool status;
};

// Instanciando um objeto 'botao'
PushButton solicitarTravessia = {5, false};

//variables to keep track of the timing of recent interrupts
volatile bool isTravessiaSolicitada = false;
volatile unsigned long buttonTime = 0;  
volatile unsigned long lastButtonTime = 0; 

// Função Temporizada - Millis()
unsigned long tempo = 0;


// ---------------------------------------------------- //
//  CLASSE - SEMAFORO
// ---------------------------------------------------- //

// Classe Semaforo
struct Semaforo
{
  // GPIO
  const int pinVermelho;
  const int pinAmarelo;
  const int pinVerde;

  // Status GPIO
  bool statusVermelho = false;
  bool statusAmarelo = false;
  bool statusVerde = false;

  // Tempos dos leds
  int tempoLedVermelho = 0;
  int tempoLedAmarelo = 0;
  int tempoLedVerde = 0;
};


// Tempo de abertura e fechamento seguro entre as transições
int tempoSeguranca = 3000; // 3s = 3000ms

// Instanciando um novo objeto 'viario' do tipo 'Semaforo'
Semaforo viario = {32, 33, 25,
                   false, false, false
                  };

// Instanciando um novo objeto 'pedestres' do tipo 'Semaforo'                  
Semaforo pedestres = {19, 0, 18,
                      false, false, false
                     };


// ---------------------------------------------------- //
//  TEMPORIZADOR
// ---------------------------------------------------- //

// Definindo um Timer
hw_timer_t *Tempo = NULL;

// Controlador de Eventos
int indiceEvento = 0;


// ---------------------------------------------------- //
//  TIMER - ISR
// ---------------------------------------------------- //

// Timer - Interrupt Service Routine
void IRAM_ATTR TimerISR()
{
  // Incrementa o indiceEvento
  indiceEvento++;

  // Reset do indiceEvento
  if(indiceEvento > 5)
  {
    indiceEvento = 0;
  }

  Serial.printf("--> TimerISR Alarm, Evento: %d\n", indiceEvento);

  switch(indiceEvento)
  {
    // t = 15s
    case 0:
      timerWrite(Tempo, 0); // Reseta o timer
      timerAlarm(Tempo, 1000000 * 15, true, 0);
      break;

    // t = 5s
    case 1:
      timerWrite(Tempo, 0); // Reseta o timer
      timerAlarm(Tempo, 1000000 * 5, true, 0);
      break;

    // t = 3s
    case 2:
      timerWrite(Tempo, 0); // Reseta o timer
      timerAlarm(Tempo, 1000000 * 3, true, 0);
      break;

    // t = 15s
    case 3:
      timerWrite(Tempo, 0); // Reseta o timer

      // Se a travessia for solicitada entao,
      // diminuir 5s do tempo viario.Verde
      if(isTravessiaSolicitada)
      {
        timerAlarm(Tempo, 1000000 * 10, true, 0);
      }

      // Caso contrario, entao manter o tempo de 15s
      else
      {
        timerAlarm(Tempo, 1000000 * 15, true, 0);
      }

      break;

    // t = 5s
    case 4:
      timerWrite(Tempo, 0); // Reseta o timer

      // Se a travessia for solicitada entao,
      // diminuir 2s do tempo viario.Amarelo
      if(isTravessiaSolicitada)
      {
        timerAlarm(Tempo, 1000000 * 3, true, 0);

        isTravessiaSolicitada = false;
      }

      // Caso contrario, entao manter o tempo de 15s
      else
      {
        timerAlarm(Tempo, 1000000 * 5, true, 0);
      }

      break;

    // t = 3s (fechamento seguro de ambos semaforos)
    case 5:
      timerWrite(Tempo, 0); // Reseta o timer
      timerAlarm(Tempo, 1000000 * 3, true, 0);
      break;
  }

  
}

// ---------------------------------------------------- //
//  BOTAO - EXT_ISR
// ---------------------------------------------------- //

// Botao 'PushButton.solicitarTravessia' - Interrupt Service Routine
void IRAM_ATTR Botao_ISR()
{
  // Software Debounce
  buttonTime = tempo;
  
  if((buttonTime - lastButtonTime) > 350)
  {
    Serial.println("--> Botao_ISR Event");

    isTravessiaSolicitada = true;

    lastButtonTime = buttonTime;
  }
}

void Inicializacao()
{
  // Ambos semáforos inicializam-se no modo 'Fechado'

  // Semaforo Viario
  viario.statusVermelho = true;
  viario.statusAmarelo = false;
  viario.statusVerde = false;

  digitalWrite(viario.pinVermelho, HIGH);
  digitalWrite(viario.pinAmarelo, LOW);
  digitalWrite(viario.pinVerde, LOW);

  // Semaforo Pedestres
  pedestres.statusVermelho = true;
  pedestres.statusVerde = false;

  digitalWrite(pedestres.pinVermelho, HIGH);
  digitalWrite(pedestres.pinVerde, LOW);
  
  // Tempo de segurança entre as transições de estado
  delay(tempoSeguranca);

  // Primeira iteração dos semaforos
  ControleSemaforo("Viario", "Vermelho", "Estatico");
  ControleSemaforo("Pedestres", "Verde", "Estatico");

  // Inicializa o Timer com o tempo de 15s
  timerAlarm(Tempo, 1000000 * 15, true, 0);

}

void CicloAutomatico()
{
  switch(indiceEvento)
  {
    // 0
    case 0:
      ControleSemaforo("Viario", "Vermelho", "Estatico");
      ControleSemaforo("Pedestres", "Verde", "Estatico");
      break;

    // 1
    case 1:
      ControleSemaforo("Viario", "Vermelho", "Estatico");
      ControleSemaforo("Pedestres", "Vermelho", "Oscilante");
      break;
    
    // 2
    case 2:
      ControleSemaforo("Viario", "Vermelho", "Estatico");
      ControleSemaforo("Pedestres", "Vermelho", "Estatico");
      break;

    // 3
    case 3:
      ControleSemaforo("Viario", "Verde", "Estatico");
      ControleSemaforo("Pedestres", "Vermelho", "Estatico");
      break;

    // 4
    case 4:
      ControleSemaforo("Viario", "Amarelo", "Estatico");
      ControleSemaforo("Pedestres", "Vermelho", "Estatico");
      break;

    // 5
    case 5:
      ControleSemaforo("Viario", "Vermelho", "Estatico");
      ControleSemaforo("Pedestres", "Vermelho", "Estatico");
      break;

  }
  
}

// Metodo para controlar o Semaforo
// Parametros(String semaforo, String luz, String modo):
// --> semaforo: Recebe o nome do semaforo (Viario ou Pedestres)
// --> luz: informar qual luz acender (Vermelho, Amarelo ou Verde)
// --> modo: informar o modo (Estatico, Oscilante)

void ControleSemaforo(String semaforo, String luz, String modo)
{
  if (semaforo == "Viario")
  {
    if (luz == "Vermelho" && modo == "Estatico")
    {
      digitalWrite(viario.pinVermelho, HIGH);
      viario.statusVermelho = true;

      digitalWrite(viario.pinAmarelo, LOW);
      viario.statusAmarelo = false;

      digitalWrite(viario.pinVerde, LOW);
      viario.statusVerde = false;

    }
    else if (luz == "Vermelho" && modo == "Oscilante")
    {
      digitalWrite(viario.pinVermelho, !digitalRead(viario.pinVermelho));
      viario.statusVermelho = digitalRead(viario.pinVermelho);

      digitalWrite(viario.pinAmarelo, LOW);
      viario.statusAmarelo = false;

      digitalWrite(viario.pinVerde, LOW);
      viario.statusVerde = false;

    }
    else if (luz == "Amarelo" && modo == "Estatico")
    {
      digitalWrite(viario.pinVermelho, HIGH); // RED
      viario.statusVermelho = false;

      digitalWrite(viario.pinAmarelo, LOW); // BLUE
      viario.statusAmarelo = true;

      digitalWrite(viario.pinVerde, HIGH); // GREEN
      viario.statusVerde = false;
    }
    else if (luz == "Verde" && modo == "Estatico")
    {
      digitalWrite(viario.pinVermelho, LOW);
      viario.statusVermelho = false;

      digitalWrite(viario.pinAmarelo, LOW);
      viario.statusAmarelo = false;

      digitalWrite(viario.pinVerde, HIGH);
      viario.statusVerde = true;
    }

  }
  else if (semaforo == "Pedestres")
  {
    if (luz == "Vermelho" && modo == "Estatico")
    {
      digitalWrite(pedestres.pinVermelho, HIGH);
      pedestres.statusVermelho = true;

      digitalWrite(pedestres.pinAmarelo, LOW);
      pedestres.statusAmarelo = false;

      digitalWrite(pedestres.pinVerde, LOW);
      pedestres.statusVerde = false;

    }
    else if (luz == "Vermelho" && modo == "Oscilante")
    {
      digitalWrite(pedestres.pinVermelho, !digitalRead(pedestres.pinVermelho));
      pedestres.statusVermelho = digitalRead(pedestres.pinVermelho);

      digitalWrite(pedestres.pinAmarelo, LOW);
      pedestres.statusAmarelo = false;

      digitalWrite(pedestres.pinVerde, LOW);
      pedestres.statusVerde = false;

    }
    else if (luz == "Amarelo" && modo == "Estatico")
    {
      digitalWrite(pedestres.pinVermelho, HIGH); // RED
      pedestres.statusVermelho = false;

      digitalWrite(pedestres.pinAmarelo, LOW); // BLUE
      pedestres.statusAmarelo = true;

      digitalWrite(pedestres.pinVerde, HIGH); // GREEN
      pedestres.statusVerde = false;
    }
    else if (luz == "Verde" && modo == "Estatico")
    {
      digitalWrite(pedestres.pinVermelho, LOW);
      pedestres.statusVermelho = false;

      digitalWrite(pedestres.pinAmarelo, LOW);
      pedestres.statusAmarelo = false;

      digitalWrite(pedestres.pinVerde, HIGH);
      pedestres.statusVerde = true;
    }
  }
}

void TempoDecorrido()
{
  tempo = millis();

  Serial.printf(" %ums | ", tempo);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // PinModes
  // Entradas
  pinMode(solicitarTravessia.pino, INPUT);

  // Saídas
  pinMode(viario.pinVermelho, OUTPUT);
  pinMode(viario.pinAmarelo, OUTPUT);
  pinMode(viario.pinVerde, OUTPUT);

  pinMode(pedestres.pinVermelho, OUTPUT);
  pinMode(pedestres.pinVerde, OUTPUT);

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
  attachInterrupt(digitalPinToInterrupt(solicitarTravessia.pino), Botao_ISR, FALLING);

  // Inicializar o Timer
  Tempo = timerBegin(1000000);
  timerAttachInterrupt(Tempo, &TimerISR);
  //timerAlarm(Tempo, 1000000 * quantidadeTempo, true, 0);

  // Modo de inicializacao
  Inicializacao();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500); // this speeds up the simulation
  
  TempoDecorrido();

  // Ciclo automatico de funcionamento
  CicloAutomatico();

  // Exibe o status de ambos semaforos 'viario' e 'pedestres'
  Serial.printf("Viario [VM:%d AM:%d VD:%d]  Pedestres [VM:%d VD:%d BT:%d]\n",
                viario.statusVermelho,
                viario.statusAmarelo,
                viario.statusVerde,
                pedestres.statusVermelho,
                pedestres.statusVerde,
                isTravessiaSolicitada);

}
