#include <MsTimer2.h>
#include <DCF77.h>
#include <Time.h>

/* ROBA DEL DISPLAY */
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

/*ROBA DEL DCF77*/ 
// Connect the clock module to pin D19 / A5
const uint8_t dcf77_sample_pin = 19; // A5
const uint8_t dcf77_analog_sample_pin = 5;
 
const uint8_t filtered_dcf77_pin = 2;
 
const uint8_t bit0_pin = 3;
const uint8_t bit15_pin = bit0_pin + 15;
 
void low_pass_filter() {
    // http://en.wikipedia.org/wiki/Low-pass_filter#Continuous-time_low-pass_filters
 
    // I will use fixed point arithmetics with 5 decimals
    const uint16_t decimal_offset = 10000;
    static uint32_t smoothed = 0*decimal_offset;
 
    //const uint32_t input = digitalRead(dcf77_sample_pin) * decimal_offset;
    const uint32_t input = analogRead(dcf77_analog_sample_pin)>200? decimal_offset: 0;
 
    // compute N such that the smoothed signal will always reach 50% of
    // the input after at most 50 samples (=50ms).
    // N = 1 / (1- 2^-(1/50)) = 72.635907286
    const uint16_t N = 72;
    smoothed = ((N-1) * smoothed + input) / N;
 
    // introduce some hysteresis
    static uint8_t square_wave_output = 0;
 
    if ((square_wave_output == 0) == (smoothed >= decimal_offset/2)) {
        // smoothed value more >= 50% away from output
        // ==> switch output
        square_wave_output = 1-square_wave_output;
        // ==> max the smoothed value in order to introduce some
        //     hysteresis, this also ensures that there is no
        //     "infinite memory"
        smoothed = square_wave_output? decimal_offset: 0;
    }
 
    digitalWrite(filtered_dcf77_pin, square_wave_output);    
}
 
DCF77 DCF = DCF77(filtered_dcf77_pin, 0);
 
volatile boolean sync = false;
volatile time_t time = 0;
 
unsigned long getDCFTime() {        
    const time_t DCFTime = DCF.getTime();
 
    sync = (DCFTime != 0);
    return DCFTime;
}
 
void diagnosis() {
    Serial.println("Waiting for DCF77 time ... ");
     
    int rising_edge = 0;
    int falling_edge = 0;
    int previous_rising_edge;
    bool was_high = false;
 
    while (timeStatus() == timeNotSet) {
        const uint8_t sensor_value = digitalRead(filtered_dcf77_pin);
        if (sensor_value) {
             if (!was_high) {
                rising_edge = millis();
                was_high = true;
            }
        } else {
            if (was_high) {
                falling_edge = millis();
                Serial.print("Cycle, Pulse: ");
 
                const int cycle = rising_edge - previous_rising_edge;
                if (cycle < 1000) {
                    Serial.print(' ');
                }
                Serial.print(cycle);                
                Serial.print(',');
                Serial.print(' ');
 
                const int pulse = falling_edge - rising_edge;
                if (pulse < 100) {
                    Serial.print(' ');
                }                
                Serial.print(pulse);
                previous_rising_edge = rising_edge;
                was_high = false;
 
                Serial.print(' ');
                Serial.print(pulse < 180? '.': 'X');
 
                Serial.print(' ');
                Serial.print(cycle <1800? ' ': 'm');
 
                Serial.println();
             }
        }
    }
     
    Serial.println();
}
 
void setup() {   
   initialize();
   digitalWrite(2, HIGH);
   digitalWrite(7, HIGH); 
    for (uint8_t pin=bit0_pin; pin < bit15_pin; ++pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    delay(500);
    for (uint8_t pin=bit0_pin; pin < bit15_pin; ++pin) {
        digitalWrite(pin, LOW);
    }  
 
     
    pinMode(dcf77_sample_pin, INPUT);
    digitalWrite(dcf77_sample_pin, HIGH);
 
    pinMode(filtered_dcf77_pin, OUTPUT);
 
    MsTimer2::set(1, low_pass_filter);
    MsTimer2::start();
 
     
    DCF.Start();
    setSyncInterval(30);
    setSyncProvider(getDCFTime);
 
     
    Serial.begin(9600);;
    diagnosis();    
}
 
void loop() {
    static time_t prevDisplay = 0;    
    
    if( now() != prevDisplay) {
        prevDisplay = now();
        digitalClockDisplay();
    }
    
    int sube = hour();
    String suby = String(sube);
    String sss = suby.substring(0,1);
    String www = suby.substring(1,2);
    invia(sss.toInt(),0);
    Serial.println("Uscita 1 display: "+sss);
    refresh(0);
    invia(www.toInt(),0);
    Serial.println("Uscita 2 display: "+www);
    refresh(1);

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
      digitalWrite(pin_enable, LOW);
      digitalWrite(pin_enable, HIGH);
      break;
    case 1:
      digitalWrite(pin_enable_uno, LOW);
      digitalWrite(pin_enable_uno, HIGH);
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

void digitalClockDisplay() {
    if (sync) {
        Serial.println("DCF sync good");
        sync = false;
    }
 
    Serial.print(day());
    Serial.print('.');
    Serial.print(month());
    Serial.print('.');
    Serial.print(year());
    Serial.print(' ');
    Serial.print(' ');
 
 
    const uint8_t minutes = minute();
    const uint8_t hours = hour();
     
    print_2_digits(hours);
    Serial.print(':');
    print_2_digits(minutes);
    Serial.print(':');
    print_2_digits(second());
 
    display_digit(minutes % 10, bit0_pin + 0, 4);
    display_digit(minutes / 10, bit0_pin + 5, 3);
         
    display_digit(hours % 10, bit0_pin +  9, 4);
    display_digit(hours / 10, bit0_pin + 14, 2);
     
    Serial.println();
}
 
void print_2_digits(const uint8_t digits) {
    if (digits < 10) {
        Serial.print('0');
    }
    Serial.print(digits, DEC);
}
 
void display_digit(const uint8_t digit, const uint8_t base_pin, const uint8_t bits) {
    for (uint8_t bit=0; bit < bits; ++bit) {
        digitalWrite(base_pin + bit, bitRead(digit, bit));
    }    
}
