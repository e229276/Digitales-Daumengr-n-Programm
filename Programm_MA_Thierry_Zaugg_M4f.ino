//DHT Bibliotheken
#include <DHT.h>
#include <DHT_U.h>

//BH1750 Bibliotheken
#include <Wire.h>
#include <BH1750.h>

//DHT
const int DHTPIN = 3;

const int DHTTYPE = DHT11;

DHT dht(DHTPIN, DHTTYPE);

int hum = 0;
int temp = 0;

//BH1750
BH1750 lightMeter;

//Erdfeuchtigkeits-Sensor
const int soilPin = A0;
int soil = 0;

//LED Pins
const int sLED = 8;
const int hLED = 9;
const int tLED = 10;
const int lLED = 11;

//Zeitzählungen
int dlux = 0; //Menge an Licht die die Pflanze am gesamten Tag erhalten hat
int t = 0; //Tageszeit in halben Stunden Wichtig: Dieser Wert muss der Tageszeit beim Start des Programms angepasst werden!

void setup() {
  //Starten der Kommunikation mit dem Arduino
  Serial.begin(9600);

  //Sensoren Setup
  dht.begin();

  Wire.begin();

  lightMeter.begin();

  //LED Setup
  pinMode(sLED, OUTPUT);
  pinMode(hLED, OUTPUT);
  pinMode(tLED, OUTPUT);
  pinMode(lLED, OUTPUT);

  //LED Test
  digitalWrite(sLED, HIGH);
  digitalWrite(hLED, HIGH);
  digitalWrite(tLED, HIGH);
  digitalWrite(lLED, HIGH);
  delay(2000);
  digitalWrite(sLED, LOW);
  digitalWrite(hLED, LOW);
  digitalWrite(tLED, LOW);
  digitalWrite(lLED, LOW);
}

void loop() {
  //DHT Messungen
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  //BH1750 Messungen
  uint16_t lux = lightMeter.readLightLevel();
  
  //Erdfeuchtigkeits-Sensor Messungen
  soil = analogRead(soilPin);
  soil=map(soil,0,1023,100,0);//Stellt Werte des Sensors als abgerundete ganzzahlige Werte von 0 - 100 dar

  //Serieller Monitor Ausgabe
  Serial.print("Feuchtigkeit der Erde: ");
  Serial.print(soil);
  Serial.println("%");
  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(hum);
  Serial.println("%");
  Serial.print("Temperatur: ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Lichtmenge jetzt: ");
  Serial.print(lux);
  Serial.println(" lx");

  //LED Steuerung
  //Erdfeuchtigkeits LED
  if (soil > 75 || soil < 25) {
    //Der gemessene Wert wenn die Blattfahne die Blätter hängen liess war bei 21%; Nach dem Giessen war er meistens zwischen 60% und 70%
    digitalWrite(sLED, HIGH);
  }
  else {
    digitalWrite(sLED, LOW);
  }

  //Luftfeuchtigkeits LED
  if (hum > 80 || hum < 45) {
    //eigentlich wäre 60-80% Luftfeuchtigkeit optimal aber das ist schwierig zu halten ohne ein Gewächshaus, die höchsten gemessenen Werte im inneren lagen bei 50%
    digitalWrite(hLED, HIGH);
  }
  else{
    digitalWrite(hLED, LOW);
  }

  //Temperatur LED
  if (temp > 28 || temp < 15) {
    digitalWrite(tLED, HIGH);
  }
  else {
    digitalWrite(tLED, LOW);
  }

  //Licht LED
  dlux += lux;

  if (dlux >= 3600) { 
    //3600 ist die mindest Lichtmenge (300lx) mal die benötigten Lichtstunden zur Blütezeit (12h) (Da es eine Zimmerpflanze ist kann sie theoretisch das ganze Jahr Blüten tragen)
    digitalWrite(lLED,LOW);
  }

  else if (lux >= 300 && lux <= 1000) {
    //Dieser Bereich ist die Lichtmenge bei der die Blattfahne überlebt und die Lichtmenge bei der sie wachsen kann zusammen
    digitalWrite(lLED, LOW);
  }

  else if (lux > 1000) {
    //1000 lux werden im inneren eines Gebäudes meistens nur durch direkte Sonneneinstrahlung erreicht welche bei der Blattfahne vermieden werden sollte
    digitalWrite(lLED, HIGH);
  }

  else if (lux < 300) {
    //Bei 300 lx sollte die Pflanze zwar überleben aber vermutlich kaum wachsen (das reicht aber auch für mich) 
    if (t < 20) {
      //sorgt dafür dass die LED nicht schon leuchtet bevor die Sonne überhaupt aufgeht
      digitalWrite(lLED, LOW);
    }
    else {
      digitalWrite(lLED, HIGH); 
    }
  }

  if (t == 48) {
    t = 0;
    dlux = 0;
    //setzt die Zeit und Lichtmenge des gesamten Tages zurück
  }

  Serial.print("Lichtmenge an diesem Tag: ");
  Serial.print(dlux);
  Serial.println(" lx");

  float time = t/2.0;
  Serial.print("Zeit seit Reset: ");
  Serial.print(time);
  Serial.println(" Stunden");
  Serial.println("----------------"); //Trennt die Daten verschiedener Messungen durch eine gestrichelte Linie

  //Zeitzählung
  t += 1;

  delay(1800000); //wartet eine halbe Stunde
}
