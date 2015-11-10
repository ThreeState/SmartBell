/*
Nome: Prima versione software Arduino
Autore: Manuel Romei
Ultima revisione: 10/11/2015
*/
const int pin[4] = {3, 4, 5, 6}; // Pin di uscita
const int pin_enable = 7;  // Pin di abilitazione
const byte numPins = 4;  // Numero di pin
const int pin_punto = 8;  // Pin del punto
int numero = 0;  // Numero temporaneo da visualizzare
int flag_punto = 0;   // Flag di abilitazione del punto

void setup()
{
  initialize();   // Inizializzazione
}

void loop()
{
  toggle_enable();
  numero = numero++;
  if(numero == 10)
  {
    numero = 0;
  }
  invia(numero);
  delay(1000);
  punto_toggle();
}

void initialize()
{
  int e;
  for(e = 0; e < 4; e++)
  {
    pinMode(pin[e], OUTPUT);
  }
  pinMode(pin_enable, OUTPUT);
  pinMode(pin_punto, OUTPUT);
}

void invia(int a)
{
   byte num = a; // Get num from somewhere
   for (byte i=0; i<numPins; i++) 
   {
     byte state = bitRead(num, i);
     digitalWrite(pin[i], state);
   }
}

void toggle_enable()  // Funzione che fa il refresh del display
{
  digitalWrite(pin_enable, HIGH);
  digitalWrite(pin_enable, LOW);
}

void punto_toggle()   // Cambiamo lo stato del punto
{
  if(flag_punto == 0)
  {
    digitalWrite(pin_punto, HIGH);
    flag_punto = 1;
  }
  else if(flag_punto == 1)
  {
    digitalWrite(pin_punto, LOW);
    flag_punto = 0;
  }
}
