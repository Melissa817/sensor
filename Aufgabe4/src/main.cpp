#include <Arduino.h>
#include <ESPDateTime.h>
#include <cmath>
#include <sensor.h>
#include <display.h>
#include <backend.h>

Display display;
Sensor sensor;
Backend backend;

char *SSID = const_cast<char *>("FRITZ!Box 7530 KA");                   // TODO replace with own wifi credentials
char *PW = const_cast<char *>("62223197475346691490");                      // TODO replace with own wifi credentials
const String BACKEND_UUID = "uuid";                                 // TODO replace with credentials provided by email
const String BACKEND_TOKEN = "token";                               // TODO replace with credentials provided by email
const float SENSOR_LAT = 49.2779349737392;                          // TODO replace with own coordinates
const float SENSOR_LON = 8.840873399866737;                         // TODO replace with own coordinates
const bool SENSOR_INDOOR = true;

// SSID, PW, UUID, TOKEN, LAT, LON, INDOOR Datenfelder mit passenden Werten ausfüllen 

// Hinweise: Nachfolgende Schritte erst für Temperatur, später dann auch für den Luftdruck (analoges Vorgehen)


// In Sensor::initSensor()  :

// 1. Schritt
// BMP280 finden
// -> read8 implementieren

// 2. Schritt
// Sensor initialisieren (Modus allgemein, Modi für Temperatur und Druck,  Infos im Datenblatt)

// 3. Schritt
// Korrekturwerte der Temperatur (trimming values) vom Senseor auslesen (Auch hier genauere Infos im Datenblatt) 
// Korrekturwerte überprüfen (Tipp: zum Überprüfen Werte auf der Konsole ausgeben lassen)
// -> read16 implementieren


// In Sensor::readTemperatureAndPressure()  :

// 4. Schritt: Lesen der Temperaturwerte (raw)

// 5. Schritt: Konvertieren der "raw" Temperaturwerte in °C und Speichern in "temperature"

// 6. Schritt: 3-5 analog für den Luftdruck


// In Display::refreshContent() :

// 7. Schritt: refreshContent() implementieren (zur Ausgabe der Werte auf dem Display)


// In Main::loop() :

// 8. Schritt: Richtige Zeitzone als Datumsformat wählen
//              --> Server braucht UTC/GMT 0 
//              --> auf dem Display unsere Zeitzone UTC/GMT +2
// Überprüfen ob die Werte + Timestamp richtig am Server ankommen (Auf die Website schauen)


// 9. Schritt: Exception Handling 

void setup()
{

  Serial.begin(9600);
  display.init();
  backend.init(BACKEND_UUID, BACKEND_TOKEN, SENSOR_LAT, SENSOR_LON, SENSOR_INDOOR);
  backend.connectToWifi(SSID, PW);
  display.setSsid(SSID);

  if (!DateTime.begin(15 * 1000))
  {
    Serial.println("Read current time failed!");
  } 
  else {
    // UTC+2 -> add 2 hours (2 * 3600 sec) to now
    Serial.println(DateFormatter::format("%Y-%m-%dT%H:%M:%S.000Z", DateTime.now() + 2*3600));
  }
  sensor.initSensor();
}

void loop()
{

  bool readSuccessful = sensor.readTemperatureAndPressure();
  display.setReadSuccessful(readSuccessful);

  if (readSuccessful)
  {
    float t = sensor.getTemperature();
    float p = sensor.getPressure();

    display.setTemperature(t);
    display.setPressure(p);

    // UTC+2 -> add 2 hours (2 * 3600 sec) to now
    display.setDateTime(DateFormatter::format("%d.%m.%Y %H:%M:%S", DateTime.now() + 2*3600));

    //Hier bitte auskommentieren, sobald UUID und TOKEN mitgeteilt wurden 
    //backend.sendTemperatureMeasurement(t, DateFormatter::format("%Y-%m-%dT%H:%M:%S.000Z", DateTime.now()));
    //backend.sendPressureMeasurement(p, DateFormatter::format("%Y-%m-%dT%H:%M:%S.000Z", DateTime.now()));
  }

  delay(3000);
}