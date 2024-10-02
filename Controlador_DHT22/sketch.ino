// Bibliotecas
#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>

// DHT22
#define DHT_SDA_Pin 15

// LCD 16x2 (I2C)
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2

// Joystick (analog)
#define JOY_VERTICAL 34 // analogico
#define JOY_HORIZONTAL 35 // analogico
#define JOY_SELECT 32 // digital

int joyVerticalAnalogValue = 0;
int joyHorizontalAnalogValue = 0;
volatile bool joySelectButton = false;
char joyPosicao = '-'; // idle

// Interrupcao
//variables to keep track of the timing of recent interrupts
volatile unsigned long buttonTime = 0;
volatile unsigned long lastButtonTime = 0;

// Função Temporizada - Millis()
unsigned long tempo = 0;

// Timer
hw_timer_t *Timer0 = NULL;

bool isMenuAtivo = false;
int indiceMenu = 0;
int paginaMenu = 0;
int linhaMenu = 0;
bool textoEscrito = false;

// Objetos
DHTesp dht22;
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
TempAndHumidity data;

// Custom char
uint8_t customChar[8] = {
  B11111, B01110, B00100, B00000, B00000, B00100, B01110, B11111
};

byte upCustomPixel[] = { B11111, B11011, B10001, B01010, B11011, B11011, B11011, B11111 };
byte downCustomPixel[] = { B11111, B11011, B11011, B11011, B01010, B10001, B11011, B11111 };
byte rightCustomPixel[] = { B11111, B10111, B10011, B10001, B10001, B10011, B10111, B11111 };
byte leftCustomPixel[] = { B11111, B11101, B11001, B10001, B10001, B11001, B11101, B11111 };

byte alertNormal[] = { B00000, B00100, B00100, B00100, B00100, B00000, B00100, B00000 };
byte alertInverted[] = { B11111, B11011, B11011, B11011, B11011, B11111, B11011, B11111 };

// Interrupt Service Routine
void IRAM_ATTR isr()
{
  // Software Debounce
  buttonTime = tempo;

  if ((buttonTime - lastButtonTime) > 300)
  {
    //Serial.println("Int ISR");
    joySelectButton = true;
    isMenuAtivo = true;
    textoEscrito = false;
    lastButtonTime = buttonTime;
  }
}

void ExibirPagina_5()
{
  joySelectButton = false;

}

void ExibirPagina_4()
{
  joySelectButton = false;

}

void ExibirPagina_3()
{
  joySelectButton = false;

}

void ExibirPagina_2()
{
  // Controla os limites de indice da linha
  if (linhaMenu < 0)
  {
    linhaMenu = 0;
  }
  else if (linhaMenu > 3)
  {
    linhaMenu = 3;
  }

  // Navegação através do botao 'JOY_SEL'
  // Para o menu ser escolhido deve ser pressionado
  // o botão do meio do joystick
  if (joySelectButton)
  {
    isMenuAtivo = true;
    joySelectButton = false;

    switch (linhaMenu)
    {
      case 0:
        //linhaMenu = 0;
        //paginaMenu = 2;
        break;

      case 1:
        //ExibirPagina_2();
        break;

      case 2:
        //ExibirPagina_3();
        break;

      case 3:
        //ExibirPagina_4();
        break;
    }
  }
  else
  {
    // Controlar o conteudo das linhas
    switch (linhaMenu)
    {
      case 0:
        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("\x06\x07 SP Temp.");

          lcd.setCursor(0, 1);
          lcd.print("   SP Umid.    \x05");

          textoEscrito = true;
        }

        break;

      case 1:
        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("   SP Temp.");

          lcd.setCursor(0, 1);
          lcd.print("\x06\x07 SP Umid.    \x05");

          textoEscrito = true;
        }

        break;

      case 2:
        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("   SP Umid.    \x04");

          lcd.setCursor(0, 1);
          lcd.print("\x06\x07 Retornar");

          textoEscrito = true;
        }

        break;

      
    }
  }


}

void ExibirPagina_1()
{
  // Controla os limites de indice da linha
  if (linhaMenu < 0)
  {
    linhaMenu = 0;
  }
  else if (linhaMenu > 4)
  {
    linhaMenu = 4;
  }

  // Navegação através do botao 'JOY_SEL'
  // Para o menu ser escolhido deve ser pressionado
  // o botão do meio do joystick
  if (joySelectButton)
  {
    isMenuAtivo = true;
    joySelectButton = false;

    switch (linhaMenu)
    {
      case 0:
        linhaMenu = 0;
        paginaMenu = 2;
        break;

      case 1:
        //ExibirPagina_2();
        break;

      case 2:
        //ExibirPagina_3();
        break;

      case 3:
        //ExibirPagina_4();
        break;
    }
  }
  else
  {
    // Controlar o conteudo das linhas
    switch (linhaMenu)
    {
      case 0:

        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("\x07 Setpoints...");

          lcd.setCursor(0, 1);
          lcd.print("  Unidades...  \x05");

          textoEscrito = true;
        }

        break;

      case 1:

        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("  Setpoints... \x04");

          lcd.setCursor(0, 1);
          lcd.print("\x07 Unidades...  \x05");

          textoEscrito = true;
        }

        break;

      case 2:

        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("  Unidades...  \x04");

          lcd.setCursor(0, 1);
          lcd.print("\x07 Alarmes...   \x05");

          textoEscrito = true;
        }

        break;

      case 3:

        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("  Alarmes...   \x04");

          lcd.setCursor(0, 1);
          lcd.print("\x07 Debug...     \x05");

          textoEscrito = true;
        }

        break;

      case 4:

        if (textoEscrito == false)
        {
          lcd.clear();

          lcd.setCursor(0, 0); // Col, Row
          lcd.print("  Debug...     \x04");

          lcd.setCursor(0, 1);
          lcd.print("\x07 Retornar");

          textoEscrito = true;
        }

        break;

    }
  }


}

void ExibirPagina_0()
{
  if (joySelectButton)
  {
    isMenuAtivo = true;
    paginaMenu = 1;
    joySelectButton = false;
  }
  else
  {
    // Desativa o controle do joystick
    isMenuAtivo = false;

    Serial.println("T: " + String(data.temperature, 2) + "ºC");
    Serial.println("U: " + String(data.humidity, 2) + "%");

    // Escreve no lcd uma unica vez
    if (!textoEscrito)
    {
      // Escrevendo a informação no display lcd
      lcd.clear();

      // Temperatura
      lcd.setCursor(0, 0); // Col, Row
      lcd.print(String(data.temperature, 1) + " \xDF" + "C");

      // Umidade
      lcd.setCursor(0, 1);
      lcd.print(String(data.humidity, 1) + " %rh");

      textoEscrito = true;
    }

  }

}

void MenuNavegacao()
{
  // [1] Setpoints
  //[1.1] Temp
  //[1.2] Umidade
  // [2] Unidades
  // [2.1] Temp.
  // [2.1.1] Celsius
  // [2.1.2] Farenheit
  // [2.1.3] Kelvin
  // [2.2] Umidade
  // [2.2.1] Relativa (rh)
  // [2.2.2] Absoluta (g/m³)
  // [2.2.3] Específica (g/kg)
  // [3] Alarmes
  //[3.1] Temp ON
  //[3.2] Umidade ON
  // [4] Testar Joystick

  switch (paginaMenu)
  {
    case 0:
      ExibirPagina_0();
      break;

    case 1:
      ExibirPagina_1();
      break;

    case 2:
      ExibirPagina_2();
      break;
  }

}

void JoystickPosition()
{
  // [0] Idle   V:512   H:512   (0)
  // [1] Up     V:1023  H:512   (-1)
  // [2] Down   V:0     H:512   (+1)
  // [3] Left   V:512   H:1023  (-10)
  // [4] Right  V:512   H:0     (+10)

  // Monitorando os valores do Joystick
  joyVerticalAnalogValue = analogRead(JOY_VERTICAL);
  joyHorizontalAnalogValue = analogRead(JOY_HORIZONTAL);

  // Habilita os controles de navegação
  // 'cima' e 'baixo' somente se o 'menuAtivo'
  // for verdadeiro
  if (isMenuAtivo)
  {
    if (joyVerticalAnalogValue == 512 &&
        joyHorizontalAnalogValue == 512) // Idle
    {
      joyPosicao = '-';
    }
    else if (joyVerticalAnalogValue == 1023 &&
             joyHorizontalAnalogValue == 512) // Up
    {
      joyPosicao = 'U';
      linhaMenu = linhaMenu - 1;
      textoEscrito = false;
    }
    else if (joyVerticalAnalogValue == 0 &&
             joyHorizontalAnalogValue == 512) // Down
    {
      joyPosicao = 'D';
      linhaMenu = linhaMenu + 1;
      textoEscrito = false;
    }
    else if (joyVerticalAnalogValue == 512 &&
             joyHorizontalAnalogValue == 1023) // Left
    {
      joyPosicao = 'L';
      paginaMenu = paginaMenu - 1;
      textoEscrito = false;
    }
    else if (joyVerticalAnalogValue == 512 &&
             joyHorizontalAnalogValue == 0) // Right
    {
      joyPosicao = 'R';
      paginaMenu = paginaMenu + 1;
      textoEscrito = false;
    }

  }


  Serial.printf("--> JOY: [D]:%c [V]:%d [H]:%d [S]:%d [P]:%i [L]:%i [TE]:%i\n",
                joyPosicao,
                joyVerticalAnalogValue,
                joyHorizontalAnalogValue,
                joySelectButton,
                paginaMenu,
                linhaMenu,
                textoEscrito);

}

void TempoDecorrido()
{
  tempo = millis();

  //Serial.printf("Tempo decorrido: %ums\n", tempo);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // PinMode
  // Entradas
  pinMode(JOY_SELECT, INPUT);

  // Saidas

  // Interrupcoes
  attachInterrupt(digitalPinToInterrupt(JOY_SELECT), isr, FALLING);

  // Define a resolução do ADC
  // (0-1023 bits) --> (0V - 3.3V)
  analogReadResolution(10);

  // Setup 'dht22'
  dht22.setup(DHT_SDA_Pin, DHTesp::DHT22);

  // Setup 'lcd'
  lcd.init();
  delay(200);

  lcd.backlight();
  delay(200);

  // Criando os caracteres customizados
  lcd.createChar(3, customChar);
  lcd.createChar(4, upCustomPixel);
  lcd.createChar(5, downCustomPixel);
  lcd.createChar(6, leftCustomPixel);
  lcd.createChar(7, rightCustomPixel);
  delay(1000);

  // Self test all pixels
  for (int i = 0; i < 16; i++)
  {
    lcd.setCursor(i, 0); // Col, Row
    lcd.print("\xFF");

    lcd.setCursor(i, 1); // Col, Row
    lcd.print("\xFF");
    delay(50);
  }

  delay(1500);

  for (int i = 15; i >= 0; i--)
  {
    lcd.setCursor(i, 0); // Col, Row
    lcd.print("\x02");

    lcd.setCursor(i, 1); // Col, Row
    lcd.print("\x02");
    delay(50);
  }

  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500); // this speeds up the simulation

  // Tempo - Millis
  TempoDecorrido();

  // Instanciando um objeto 'data'
  data = dht22.getTempAndHumidity();

  // Realiza a leitura do joystick
  JoystickPosition();

  // Controla a exibição de menus
  MenuNavegacao();

  // Exibe o caractere customizado
  //lcd.setCursor(10, 1);
  //lcd.print("\x04");

  //lcd.setCursor(11, 1);
  //lcd.print("\x05");

  //lcd.setCursor(12, 1);
  //lcd.print("\x06");

  //lcd.setCursor(13, 1);
  //lcd.print("\x07");

  //lcd.setCursor(14, 1);
  //lcd.print("\x03");

}
