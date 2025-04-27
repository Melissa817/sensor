#ifndef SENSOR_H_INCLUDED
#define SENSOR_H_INCLUDED

#include <Wire.h>

class Sensor
{
public:
    void initSensor();

    bool readTemperatureAndPressure();

    /**
     * Returns the last read temperature in degrees celcius. Returns std::numeric_limits<float>::min() if the temperature is not available.
     */
    float getTemperature();

    /**
     * Returns the last read pressure in hPa. Returns std::numeric_limits<float>::min() if the pressure is not available.
     */
    float getPressure();

private:
    TwoWire i2c = TwoWire(1);

    // trimming values
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    // last values
    float temperature = std::numeric_limits<float>::min();
    float pressure = std::numeric_limits<float>::min();

    int32_t t_fine;

    /**
     * Reads 16 Bit (2 Bytes) from BMP280, starting at register reg
    */
    uint16_t read16(byte reg);

    /**
     * Reads 8 Bit (1 Byte) from BMP280, starting at register reg
    */
    uint8_t read8(byte reg);

    /**
     * Writes 8 Bit (1 Byte) to register reg
    */
    void write8(byte reg, uint8_t val);

    /**
     * Returns a composed 32 Bit value of the given registers
    */
    int32_t readRawData(byte msbReg, byte lsbReg, byte xlsbReg);

    /**
     * Reads the raw temperature values from BMP280
    */
    int32_t readRawTemperature();

    /**
     * Reads the raw pressure values from BMP280
    */
    int32_t readRawPressure();

    /**
     * Returns temperature in DegC. Output value of “51.23” equals 51.23 DegC.
    */
    float compensateTemperature(int32_t adc_T);

    /**
     * Returns pressure in hPa. Output value of “963.86” equals 963.86 hPa
    */
    float compensatePressure(int32_t adc_P);
};

#endif
