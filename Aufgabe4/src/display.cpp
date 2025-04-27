#include <display.h>

void Display::refreshContent()
{

  display.clear();

  display.drawString(8, 3, "AI-Sensorik");

  if (lastReadSuccessful)
  {
    display.drawString(8, 20, String(lastTemperature) + "°C   " + String(lastPressure) + "hPa");
  } else {
    display.drawString(8, 20, "Read not successfull!");
  }
  
  display.drawString(8, 35, dateTime);
  display.drawString(8, 50, "Wifi: " + wifiSsid);
  display.display();
}

void Display::init()
{
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  refreshContent();

  delay(400);
}

void Display::setReadSuccessful(bool successful)
{
  lastReadSuccessful = successful;
  refreshContent();
}

void Display::setSsid(char *ssid)
{
  wifiSsid = String(ssid);
  refreshContent();
}

void Display::setTemperature(float temperature)
{
  lastTemperature = temperature;
  refreshContent();
}

void Display::setPressure(float pressure)
{
  lastPressure = pressure;
  refreshContent();
}

void Display::setDateTime(String dateTime)
{
  this->dateTime = dateTime;
}