// Resumo:
//
// - Simular uma 'barra de progresso' utilizando os recursos
// de desenho da biblioteca 'Adafruit_GFX.h'


// Bibliotecas
#include <Adafruit_SSD1306.h> 
#include <Adafruit_GFX.h>
#include <Wire.h>

// SSD1306 pixel code generator
//https://arduinogfxtool.netlify.app/#:~:text=Online%20Arduino%20AdafruitGFX%20SSD1306%20OLED%20Code%20Generator.%20For

// SSD1306 OLED
#define LarguraTela 128 // x
#define AlturaTela 64   // y

// Potenciometro
#define potenciometro 34

// Objetos
Adafruit_SSD1306 display(LarguraTela, AlturaTela, &Wire, -1);

// Variaveis
int valorAnalogico = 0;
long temperatura = 0;
int px1 = 0;
int px0 = 0;

void ConversorTemperatura()
{
  // Realiza a leitura do ADC para o potenciometro
  valorAnalogico = analogRead(potenciometro);

  // Converte o ADC para Temperatura
  // ADC  | Temp. |
  // 4095 |  250  |
  // xADC |  t    |
  // 0    |  0    |

  // --> t = (xADC * 250) / 4095   

  temperatura = (valorAnalogico * 250) / 4095;


  // Converte a temperatura para coordenada pixel (px1) no OLED
  // Temp | Pixel (px1)         |
  // 250  |    120              |
  // 150  |     x1              |
  // 1    |     1               |
  // 0    | 119 (cor invertida) |

  // --> pixel [x1] = (temp * 120) / 250
  px1 = (temperatura * 120) / 250;

  Serial.printf("\n[ADC] %i  [Temp.] %i  [px1] %i", valorAnalogico, temperatura, px1);

  // Animação da barra de progresso
  // Fill Retangulo (0%)
  // [0%] @ x1 = 119
  // display.fillRect(5, 17, 119, 13, SSD1306_BLACK);

  // Fill Retangulo (1%)
  // [1%] @ x1 = 1
  // display.fillRect(4, 17, 1, 13, SSD1306_WHITE);

  // Fill Retangulo (100%)
  // [100%] @ x1 = 119
  // display.fillRect(5, 17, 119, 13, SSD1306_WHITE);

  //display.clearDisplay();

  if(temperatura == 0)
  {
    // Fill Retangulo (0%)
    // [0%] @ x1 = 119
    display.fillRect(4, 19, 120, 15, SSD1306_BLACK);
  }
  else if(temperatura > 0 && temperatura < 300)
  {
    display.fillRect(4, 19, px1, 15, SSD1306_WHITE);
    display.fillRect(px1 + 5, 19, 120 - px1, 15, SSD1306_BLACK);
    
  }
  else if(temperatura == 300)
  {
    // Fill Retangulo (100%)
    // [100%] @ x1 = 119
    display.fillRect(5, 19, 119, 15, SSD1306_WHITE);
  }

  // Como apagar somente uma determinada regiao
  // desenhar um fillRectangle invertido (nao recomendado)
  display.fillRect(4, 39, 120, 24, SSD1306_BLACK);

  // Temperatura (valor numerico)
  display.setCursor(25,40);
  display.setTextSize(3);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  // Ajustar o espacamento
  if(temperatura < 10)
  {
    display.printf("  %i", temperatura);
  }
  else if(temperatura >= 10 && temperatura < 100)
  {
    display.printf(" %i", temperatura);
  }
  else
  {
    display.printf("%i", temperatura);
  }

  // Temperatura (simbolo 'ºC')
  display.setCursor(60,42);
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.printf("  \xA7");
  display.print("C");
  
  // Exibe a informação no OLED
  display.display();

}

// Metodos
void customMenu()
{
  display.clearDisplay();

  // Linha superior
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.println(F("Temperatura:"));

  // Desenhar uma linha
  display.drawLine(2, 12, 67, 12, SSD1306_WHITE);

  // Retangulo
  display.drawRect(2, 17, 124, 19, SSD1306_WHITE);

  // Exibe informações no OLED
  display.display();
  delay(2000);
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // ADC
  // (0-1023 bits) --> (0V - 3.3V)
  analogReadResolution(12);

  // SSD1306_SWITCHCAPVCC 
  // generate display voltage from 3.3V internally
  if(!(display.begin(SSD1306_SWITCHCAPVCC,0X3C)))
  {
   Serial.println("OLED SSD1306 allocation failed.");
   for(;;); // Don't proceed, loop forever
  }

  delay(500);

  // Limpa o display
  display.clearDisplay();
  delay(1000);

  // Exibe o menu customizado uma unica vez
  // *reduz o tempo de renderizacao da tela
  customMenu();
 
}

void loop() 
{
  delay(150);

  // Lê o valor no ADC e exibe animação na barra de progresso
  ConversorTemperatura();

}

