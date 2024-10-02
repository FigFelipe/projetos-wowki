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
};

// Tempo de referencia inicial
long tempoInicial = 0;
bool flagTempoInicial = false;

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

// Variável para a função temporizada - Millis()
int periodo = 1000;
unsigned long tempoAtual = 0;
unsigned long tempoDecorrido = 0;

// Evitar o problema do Overflow do Millis()
// https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/#:~:text=One%20of%20our%20most%20popular%20blog%20posts%20right%20now%20this
void TempoAtual()
{
  if((unsigned long)(millis() - tempoAtual) > periodo)
  {
    tempoAtual = millis();
  }
  
  Serial.printf(" --> %ums", tempoAtual);
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
}

void CicloAutomatico()
{
  // Todo o ciclo de controle é baseado na variável
  // 'tempo' (tem origem da funcao - millis())
  
  // Obtem o 'tempoInicial' somente no primeiro ciclo do programa
  if(flagTempoInicial == false)
  {
    tempoInicial = tempoAtual;
    flagTempoInicial = true;

    Serial.printf("--> Tempo inicial (base): %ums\n", tempoInicial);
  }

  // Tempo inicial - t(0s) até t(15s)
  if(tempoAtual < tempoInicial + 15000)
  {
    ControleSemaforo("Viario", "Vermelho", "Estatico"); // Liga Vermelho do semaforo 'viario'
    ControleSemaforo("Pedestres", "Verde", "Estatico");
  }
  else if(tempoAtual >= tempoInicial + 15000 && tempoAtual < tempoInicial + 20000) // t >= 15 
  {
    ControleSemaforo("Pedestres", "Vermelho", "Oscilante");
  }
  else if(tempoAtual >= tempoInicial + 20000 && tempoAtual < tempoInicial + 23000) // t >= 20
  {
    ControleSemaforo("Pedestres", "Vermelho", "Estatico");
  }
  else if(tempoAtual >= tempoInicial + 23000 && tempoAtual < tempoInicial + 38000)
  {
    ControleSemaforo("Viario", "Verde", "Estatico");  
  }
  else if(tempoAtual >= tempoInicial + 38000 && tempoAtual < tempoInicial + 43000)
  {
    ControleSemaforo("Viario", "Amarelo", "Estatico");  
  }
  else if(tempoAtual >= tempoInicial + 43000 && tempoAtual < tempoInicial + 51000)
  {
    ControleSemaforo("Viario", "Vermelho", "Estatico");  
  }
  else
  {
    // Reset
    // Millis() nao pode ser resetado manualmente
    // Entao atribuir o valor do tempo Millis()
    // ao tempoInicial
    Serial.write(12);//ASCII, limpa a tela do terminal
    flagTempoInicial = false;    
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
      digitalWrite(viario.pinVermelho, LOW);
      viario.statusVermelho = false;

      digitalWrite(viario.pinAmarelo, HIGH);
      viario.statusAmarelo = true;

      digitalWrite(viario.pinVerde, LOW);
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
      digitalWrite(pedestres.pinVermelho, LOW);
      pedestres.statusVermelho = false;

      digitalWrite(pedestres.pinAmarelo, HIGH);
      pedestres.statusAmarelo = true;

      digitalWrite(pedestres.pinVerde, LOW);
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // PinModes
  // Entradas

  // Saídas
  pinMode(viario.pinVermelho, OUTPUT);
  pinMode(viario.pinAmarelo, OUTPUT);
  pinMode(viario.pinVerde, OUTPUT);

  pinMode(pedestres.pinVermelho, OUTPUT);
  pinMode(pedestres.pinVerde, OUTPUT);

  // Modo de inicializacao
  Inicializacao();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500); // this speeds up the simulation

  // Obtem o tempo atual de execução do programa
  TempoAtual();

  // Ciclo automatico de funcionamento
  CicloAutomatico();

  // Exibe o status de ambos semaforos 'viario' e 'pedestres'
  Serial.printf(" Viario [VM:%d AM:%d VD:%d]  Pedestres [VM:%d VD:%d]\n",
                viario.statusVermelho,
                viario.statusAmarelo,
                viario.statusVerde,
                pedestres.statusVermelho,
                pedestres.statusVerde);



}
