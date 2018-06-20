
// erstellen einiger Variablen
int feuchteLevel = 0;
int port = 0;
int sensorval = 0;
int state = 0;
const int sensorCount = 5;
const int sensors[sensorCount] = {A1, A2, A3, A4, A5};
const int statusLED[] = {13,21,11,10,9};
const String zustand[] = {"nA", "trocken", "feucht", "nass"};

#define thresh_trocken 800
#define thresh_nass 400

#define trocken 1
#define nass 3
#define feucht 2



 
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

  for (int i = 0; i < sensorCount; i++) {    
    pinMode(sensors[i], INPUT_PULLUP);  // set pull-up on analog pin
    pinMode(statusLED[i], OUTPUT); 
    
    digitalWrite(statusLED[i],LOW);
    delay(50);
  }

}
 
void loop()
{


for (int i = 0; i < sensorCount; i++) {
    Serial.print("sensor: ");
    Serial.print(i);
    Serial.print("-");
    int reading = best_feuchteLevel(sensors[i]);
    delay(50);
    Serial.println(zustand[reading]);  
   if (reading == nass)     
    digitalWrite(statusLED[i],HIGH);   
   else
      digitalWrite(statusLED[i],LOW);
}
      

Serial.println("-----------");
delay(1000);
 
 //Hier springt der Loop in den oben angegebenen Programmabschnitt "Tasterstatus" und liest dort den gedrückten Taster aus.

} //Loop beenden

