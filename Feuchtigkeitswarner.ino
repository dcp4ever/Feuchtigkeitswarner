#include <avr/sleep.h>
#include <avr/power.h>

// erstellen einiger Variablen
int feuchteLevel = 0;
int port = 0;
int sensorval = 0;
int state = 0;
int alarm = 0;
const int sensorCount = 8;
const int sensors[sensorCount] = {A0,A1, A2, A3, A4, A5,A6,A7};
const int statusLED[] = {2,3,4,5,6,7,8,9};
const String zustand[] = {"nA", "trocken", "feucht", "nass"};
volatile int toggle = 0;


#define alarmPin 10 //Pin an dem der Piezosummer angeschlossen ist

#define thresh_trocken 800
#define thresh_nass 400
#define trocken 1
#define nass 3
#define feucht 2


ISR(TIMER1_OVF_vect)
  /* Timer1 Interrupt service Routine */
  {
  if(toggle == 0)
    toggle = 1;
  }

void enter_sleep(void)
  /* Arduino schlafen legen */
  {
    Serial.println("Entering Sleepmode");
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  /* Weil diesmal nicht der tiefste Schlafmodus gewaehlt wurde,
   * koennen unbenutzte Komponenten noch zusaetzlich abgeschaltet
   * werden, um Energie zu sparen. Noch wichtiger: Die Komponenten
   * koennten ggf. den Arduino ungewollt aufwecken.
   */
  power_adc_disable();    /* Analog-Eingaenge abschalten */
  power_spi_disable();    /* SPI abschalten */
  power_timer0_disable(); /* Timer0 abschalten */
  power_timer2_disable(); /* Timer0 abschalten */
  power_twi_disable();    /* TWI abschalten */

  sleep_mode();

  sleep_disable(); /* nach dem Schlafen, wird das Programm ab hier weiter ausgeführt*/
  
  power_all_enable();     /* Komponenten wieder aktivieren */
  }

// Bestimmen der Feuchtestaerke der gemessenen Stelle
int best_feuchteLevel(int port)
{
sensorval = analogRead(port); 
if (sensorval >= thresh_trocken) return trocken; 
if (sensorval < thresh_nass) return nass;
if (sensorval < thresh_trocken) return feucht; 
else return 0;
}




void setup()
{
Serial.begin(9600);
 
  pinMode(alarmPin, OUTPUT); 
  digitalWrite(alarmPin, HIGH); //GPIO as drain

  for (int i = 0; i < sensorCount; i++) {    
    pinMode(sensors[i], INPUT_PULLUP);  // set pull-up on analog pin
    pinMode(statusLED[i], OUTPUT);   
    digitalWrite(statusLED[i],HIGH); // GPIO as drain
    delay(50);
  }


/* Timer konfigurieren, dass der Arduino für ca 4sek schläft */
  TCCR1A = 0x00;  /* Normalbetrieb */
  TCNT1 = 0x0000; /* Zaehler loeschen */  
  TCCR1B = 0x05;  /* Prescaler: 1024 */
  TIMSK1 = 0x01;  /* Timer-Interrupt einschalten */

}
 
void loop()
{
if(toggle==1) //für den Schlafmodus benötigter Toggle
  {
  toggle = 0;

  
 
  for (int i = 0; i < sensorCount; i++) {
      Serial.print("sensor: ");
      Serial.print(i);
      Serial.print("-");
      int reading = best_feuchteLevel(sensors[i]);
      delay(50);
      Serial.println(zustand[reading]);  
     if (reading == nass){     
      digitalWrite(statusLED[i],LOW);
      alarm =1;
     }    
     else {
        digitalWrite(statusLED[i],HIGH);
        digitalWrite(alarmPin, HIGH);
     }
  }
Serial.println("-----------");

if (alarm ==1)
   digitalWrite(alarmPin, LOW); //GPIO as drain ==> Alarm an
else
   digitalWrite(alarmPin, HIGH); //GPIO as drain ==> Alarm aus
  


alarm =0;


//delay(1000);
enter_sleep();
}
 
} //Loop beenden

