
// Definindo os pinout
// X --> Saída
#define X_LED 18

// Y --> Entrada
#define Y_Button 39
#define Y_Potenciometro 36

bool buttonState = false;
int contador = 0;

int toggle = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S2!");

  // Configurando o modo do GPIO
  pinMode(Y_Button, INPUT);
  pinMode(X_LED, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(250); // this speeds up the simulation

  // Lê o estado da entrada digital
  buttonState = digitalRead(Y_Button);

  if (buttonState == true)
  {
    if (toggle == 1)
    {
      digitalWrite(X_LED, LOW);
      toggle = 0;

    }
    else
    {
      digitalWrite(X_LED, HIGH);
      toggle = 1;

    }

  }
  else
  {

  }


  Serial.println(toggle);

}
