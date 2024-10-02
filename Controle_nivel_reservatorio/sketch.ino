// ENTRADAS
#define chaveAutomaticoManual 36
#define botaoLiga 39
#define botaoDesliga 35
#define botaoEmergencia 22
#define sensorNivel 34

// SAÍDAS
#define ledAzul 2
#define ledVerde1 16
#define ledVerde2 4
#define ledAmarelo 17
#define ledVermelho 5

#define ledAutomatico 25
#define ledLigado 33
#define ledEmergencia 32

#define motorTanque 27

// Variaveis Globais

// Modo:
// 1 - Automatico
// 0 - Manual
bool modo = false;

bool emergenciaToggle = false;

bool retornarStatusAnterior = false;
bool isBombaLigada = false;

int valorAnalogico = 0;
int duty = 0;

int porcentagemNivel = 0;
int porcentagemDuty = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // GPIO modo Entrada
  pinMode(chaveAutomaticoManual, INPUT);
  pinMode(botaoLiga, INPUT);
  pinMode(botaoDesliga, INPUT);
  pinMode(botaoEmergencia, INPUT);
  pinMode(sensorNivel, INPUT);

  // GPIO modo Saída
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVerde1, OUTPUT);
  pinMode(ledVerde2, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  pinMode(ledAutomatico, OUTPUT);
  pinMode(ledLigado, OUTPUT);
  pinMode(ledEmergencia, OUTPUT);

  pinMode(motorTanque, OUTPUT);

  // ADC
  analogReadResolution(10);

  // Configurando o PWM
  // Função utilizada para configurar o LEDC
  // Parametros:
  //1. pin select LEDC pin.
  //2. freq select frequency of pwm.
  //3. resolution select resolution for LEDC channel.
  //4. channel select LEDC channel.
  ledcAttachChannel(motorTanque, 2000, 10, 0);
  
  // Modo de inicialização
  ModoInicializacao();

}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(10); // this speeds up the simulation

  Emergencia();
  
  SelecionaModo();

  medicaoNivel();

}

void ModoInicializacao()
{
  // Se for modo MANUAL, deve iniciar DESLIGADO
  bool status = digitalRead(chaveAutomaticoManual);

  if(status) // MODO AUTOMATICO
  {
   
  }
  else // MODO MANUAL
  {
    digitalWrite(ledAutomatico, LOW);
    digitalWrite(ledLigado, LOW);
    digitalWrite(ledEmergencia, HIGH);

    Serial.println("Bomba desligada");
  }
}

bool SelecionaModo()
{
    // Lê o status da chave Automatica-Manual
    bool status = digitalRead(chaveAutomaticoManual);

    if(status)
    {
      // Modo: 1 - Automatico
      modo = true;
      Serial.println("Modo AUTOMÁTICO...");

      Automatico();
    }
    else
    {
      // Modo: 0 - Manual
      modo = false;
      Serial.println("Modo MANUAL...");

      Manual();
    }

    return modo;
}

void Automatico()
{
  // Condição inicial
  retornarStatusAnterior = true;

  if(!emergenciaToggle)
  {
    digitalWrite(ledLigado, LOW);
    digitalWrite(ledEmergencia, LOW);
    digitalWrite(ledAutomatico, HIGH);

    controleNivel();
    controleVelocidadeMotor();
    
  }
}

void Manual()
{
  // Condição inicial
  if(!emergenciaToggle)
  {
    // Função para obter o estado anterior
    // quando é modificado de Automatico para Manual
    // ou vice-versa
    if(retornarStatusAnterior)
    {
      if(isBombaLigada)
      {
        digitalWrite(ledAutomatico, LOW);
        digitalWrite(ledLigado, HIGH);
        digitalWrite(ledEmergencia, LOW);

        isBombaLigada = true;
      }
      else
      {
        digitalWrite(ledAutomatico, LOW);
        digitalWrite(ledLigado, LOW);
        digitalWrite(ledEmergencia, HIGH);

        isBombaLigada = false;
      }

      retornarStatusAnterior = false;
    }

    Ligar();
    Desligar();

    if(isBombaLigada)
    {
      controleNivel();
      controleVelocidadeMotor();
    }
    else
    {
      // Função para determinar o 'duty' do pino LEDC
      ledcWrite(motorTanque, 0);
    }
  }
}

void Ligar()
{
  // Debounce delay
  delay(200);

  bool status = digitalRead(botaoLiga);

  if(status)
  {
    digitalWrite(ledAutomatico, LOW);
    digitalWrite(ledLigado, HIGH);
    digitalWrite(ledEmergencia, LOW);

    isBombaLigada = true;

    Serial.println("Bomba ligada manualmente");
  }

}

void Desligar()
{
  // Debounce delay
  delay(200);

  bool status = digitalRead(botaoDesliga);

  if(status)
  {
    digitalWrite(ledAutomatico, LOW);
    digitalWrite(ledLigado, LOW);
    digitalWrite(ledEmergencia, HIGH);

    isBombaLigada = false;

    Serial.println("Bomba desligada manualmente");
  }
}

void Emergencia()
{
  // Debouncing delay
  delay(200);

  // Lê o status atual do INPUT 'botaoEmergencia'
  bool statusBotao = digitalRead(botaoEmergencia);

  // Toggle status somente quando o botao for acionado
  if(statusBotao)
  {
    if(emergenciaToggle)
    {
      emergenciaToggle = false;
      
      Serial.printf("--> EMERGENCIA desativado... ");

      // Ir para o modo de Inicialização
      ModoInicializacao();

    }
    else
    {
      emergenciaToggle = true;
      Serial.printf("--> EMERGENCIA ativado... ");
    }
  }

  // Se o botaoEmergencia foi acionado,
  // entao piscarLedEmergencia
  if(emergenciaToggle)
  {
    duty = 0;

    // Função para determinar o 'duty' do pino LEDC
    ledcWrite(motorTanque, duty);

    digitalWrite(ledAutomatico, LOW);
    digitalWrite(ledLigado, LOW);
    digitalWrite(ledEmergencia, HIGH);

    isBombaLigada = false;

    piscaLedEmergencia();

    Serial.printf("--> EMERGENCIA ativado... ");
  }
  else
  {
    // caso contrario, desligar ledEmergencia
    //digitalWrite(ledEmergencia, LOW);
  }
  
}

void piscaLedEmergencia()
{
  digitalWrite(ledAutomatico, LOW);
  digitalWrite(ledLigado, LOW);

  digitalWrite(ledEmergencia, HIGH);
  delay(250);
  digitalWrite(ledEmergencia, LOW);
  delay(250);
}

void medicaoNivel()
{
  valorAnalogico = analogRead(sensorNivel);
  int valorVolts = analogReadMilliVolts(sensorNivel);

  // NIVEL | LIGAR_LED
  // 820   | VERMELHO  
  // 615   | AMARELO
  // 410   | VERDE_2
  // 205   | VERDE_1
  // 0     | AZUL  

  if(valorAnalogico >= 820)
  {
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVerde2, HIGH);
    digitalWrite(ledVerde1, HIGH);
    digitalWrite(ledAzul, HIGH);
  }
  else if(valorAnalogico < 820 && valorAnalogico >= 615)
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVerde2, HIGH);
    digitalWrite(ledVerde1, HIGH);
    digitalWrite(ledAzul, HIGH);
  }
  else if(valorAnalogico < 615 && valorAnalogico >= 410)
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVerde2, HIGH);
    digitalWrite(ledVerde1, HIGH);
    digitalWrite(ledAzul, HIGH);
  }
  else if(valorAnalogico < 410 && valorAnalogico >= 205)
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVerde2, LOW);
    digitalWrite(ledVerde1, HIGH);
    digitalWrite(ledAzul, HIGH);
  }
  else
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVerde2, LOW);
    digitalWrite(ledVerde1, LOW);
    digitalWrite(ledAzul, HIGH);
  }
}

void controleVelocidadeMotor()
{
  // Modo:
  if(modo) // 1 - Automatico
  {
    // NIVEL(%)      | ADC Nível       | Duty(%) | Duty(bits)
    // > 70          | 717             | 30      | 307
    // > 40 && < 70  | entre 409 e 716 | 50      | 512
    // >= 0 && < 40  | entre 0 e 408   | 60      | 614

    if(valorAnalogico >= 717)
    {
      duty = 307;
    }
    else if(valorAnalogico > 409 && valorAnalogico < 717)
    {
      duty = 512;
    }
    else
    {
      duty = 614;
    }
  }
  else // 0 - Manual
  {
    // NIVEL(%)       | ADC Nível      | Duty(%) | Duty(bits)
    // >= 0 && <= 100 | entre 0 e 1023 | 100     | 1023
    duty = 1023;
  }
}

void controleNivel()
{
  if(valorAnalogico >= 1023)
  {
    duty = 0;

    // Função para determinar o 'duty' do pino LEDC
    ledcWrite(motorTanque, duty);

    digitalWrite(ledAutomatico, LOW);
    digitalWrite(ledLigado, LOW);
    digitalWrite(ledEmergencia, HIGH);

    isBombaLigada = false;

    Serial.println("Bomba desligada por excesso de nível");
  }
  else
  {
    isBombaLigada = true;

    // Função para determinar o 'duty' do pino LEDC
    ledcWrite(motorTanque, duty);
  }

  // Converte valores para valor em porcentagem
  porcentagemNivel = BitsToPercentual(valorAnalogico);
  porcentagemDuty = BitsToPercentual(duty);

  Serial.printf("B[%d] Nivel:%d[%d%%]  Duty:%d[%d%%]  ",isBombaLigada, valorAnalogico, porcentagemNivel, duty, porcentagemDuty);

}

int BitsToPercentual(int valorBits)
{
  return 100 * valorBits / 1023;
}
