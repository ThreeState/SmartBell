/*
Nome: Software Arduino SmartBell
Autore: Manuel Romei
Ultima revisione: 16/11/2015
Versione: 0.2
*/
const int pin[4] = {3, 4, 5, 6}; // Pin di uscita
const int pin_1[4] = {50, 51, 52, 53};  // Pin di uscita del secondo display
const int pin_enable = 7;
const int pin_enable_uno = 2;
const byte numPins = 4;
const int pin_punto_uno = 10;
const int pin_punto = 8;
int numero = 0;
int flag_punto = 0;
int flag_punto_uno = 24;

void setup()
{
  initialize();
}

void loop()
{
  invia(5, 0);
  refresh(1);
  refresh(1);
  delay(1000);
  invia(3, 0);
  refresh(0);
  refresh(0);
  delay(1000);
  //punto_toggle(0);
}

void initialize()
{
  int e;
  for(e = 0; e < 4; e++)
  {
    pinMode(pin[e], OUTPUT);
    pinMode(pin_1[e], OUTPUT);
  }
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_enable_uno, OUTPUT);
  pinMode(pin_punto, OUTPUT);
  pinMode(pin_punto_uno, OUTPUT);
}

void invia(int numero, int numero_display)
{
  byte num = numero; // Numero di ingresso
  for (byte i = 0; i < numPins; i++)
  {
    byte state = bitRead(num, i);
    switch(numero_display)
    {
      case 0:
        digitalWrite(pin[i], state);
        break;
      case 1:
        digitalWrite(pin_1[i], state);
        break;
      default:
        digitalWrite(pin[i], state);
        break;
    }
  }
}

void refresh(int nume)
{
  switch(nume)
  {
    case 0:
      digitalWrite(pin_enable, HIGH);
      digitalWrite(pin_enable, LOW);
      break;
    case 1:
      digitalWrite(pin_enable_uno, HIGH);
      digitalWrite(pin_enable_uno, LOW);
      break;
  }
}

void punto_toggle(int disnum)
{
  int pin_point;
  switch(disnum)
  {
  case 0:
    pin_point = pin_punto;
    break;
  case 1:
    pin_point = pin_punto_uno;
    break;
  }
  if(flag_punto == 0)
  {
    digitalWrite(pin_point, HIGH);
    flag_punto = 1;
  }
  else if(flag_punto == 1)
  {
    digitalWrite(pin_point, LOW);
    flag_punto = 0;
  }
}

