#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED


#include <Arduino.h>
#include <SSD1306.h>

class Display {
    public:
        /**
         * Inits the display.
        */
        void init();
        /**
         * Sets the SSID.
        */
        void setSsid(char* ssid);
        /**
         * Sets the readSuccessfull variable.
        */
        void setReadSuccessful(bool successful);
        /**
         * Sets the temperature.
        */
        void setTemperature(float temperature);
        /**
         * Sets the pressure.
        */
        void setPressure(float pressure);
        /**
         * Sets the date and time.
        */
        void setDateTime(String time);

    private:
        SSD1306 display = SSD1306(0x3C, 4, 15);
        bool lastReadSuccessful = false;
        float lastTemperature = 999.99;
        float lastPressure = 999.99;
        String dateTime = "";
        String wifiSsid = "not connected";
        void refreshContent();
};


#endif