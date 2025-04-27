#include <Arduino.h>
#include <Wire.h>
#include <limits>
#include <sensor.h>

#define I2C_SDA 21
#define I2C_SCL 13
#define I2C_ADDR 0x76

// Chip id and chip id register from BMP280
#define BMP280_CHIP_ID 0x58
#define BMP280_CHIP_ID_REG 0xD0

// Trimming values registers
#define BMP_DIG_T1_REG 0x88
#define BMP_DIG_T2_REG 0x8A
#define BMP_DIG_T3_REG 0x8C
#define BMP_DIG_P1_REG 0x8E
#define BMP_DIG_P2_REG 0x90
#define BMP_DIG_P3_REG 0x92
#define BMP_DIG_P4_REG 0x94
#define BMP_DIG_P5_REG 0x96
#define BMP_DIG_P6_REG 0x98
#define BMP_DIG_P7_REG 0x9A
#define BMP_DIG_P8_REG 0x9C
#define BMP_DIG_P9_REG 0x9E

// Configuration register
#define BMP_CONFIG_REG 0xF4
#define BMP_FILTER_REG 0XF5

// Temperature: 16 Bit Resolution
// Pressure: 16 Bit Resolution
// Mode: Forced Mode
// Filter: off
// -> see page 14 and 19 in Datasheet
#define BMP_MODES 0b00100101
#define BMP_FILTER_MODES 0b00000000

// temperature registers
#define BMP_TEMP_MSB_REG 0xFA
#define BMP_TEMP_LSB_REG 0xFB
#define BMP_TEMP_XLSB_REG 0xFC

// pressure registers
#define BMP_PRESS_MSB_REG 0xF7
#define BMP_PRESS_LSB_REG 0xF8
#define BMP_PRESS_XLSB_REG 0xF9


uint16_t Sensor::read16(byte reg)
{
  uint8_t lsb = read8(reg);
  uint8_t msb = read8(reg + 1);
  
  return (uint16_t) (msb << 8) | lsb ;
}

uint8_t Sensor::read8(byte reg)
{
  try 
  {
    i2c.beginTransmission(I2C_ADDR);
    i2c.write(reg);
    i2c.endTransmission(false);
    i2c.requestFrom(I2C_ADDR, 1);

    if (i2c.available() == 1)
    {
      i2c.endTransmission();
      return i2c.read();
    }
    else 
    {
      throw "No data available!";
    } 
  } 
  catch(const std::exception& e) 
  {
    Serial.print("Error while reading from register: ");
    Serial.println(e.what());
    return 0;
  }
}

void Sensor::write8(byte reg, uint8_t val)
{
  i2c.beginTransmission(I2C_ADDR);
  i2c.write(reg);
  i2c.write(val);
  i2c.endTransmission();
}

void Sensor::initSensor()
{
  // I2C Starten mit SDA, SCL (sind bereits als globale Varaiblen deklariert)
  // BMP280 suchen
  // Sensor konfigurieren (Richtige Modi wählen)
  // Korrekturwerte (trimming values) lesen und in dig_T1 - T3... dig_P1 - P9 speichern (in sensor.h schauen)
  
  i2c.begin(I2C_SDA, I2C_SCL);

  uint8_t chipID = read8(BMP280_CHIP_ID_REG);

  if (chipID == BMP280_CHIP_ID)
  {
    Serial.println("BMP280 found!");
  }
  else
  {
    Serial.println("BMP280 not found. Please check the connections!");
  }

  dig_T1 = read16(BMP_DIG_T1_REG);
  dig_T2 = read16(BMP_DIG_T2_REG);
  dig_T3 = read16(BMP_DIG_T3_REG);

  dig_P1 = read16(BMP_DIG_P1_REG);
  dig_P2 = read16(BMP_DIG_P2_REG);
  dig_P3 = read16(BMP_DIG_P3_REG);
  dig_P4 = read16(BMP_DIG_P4_REG);
  dig_P5 = read16(BMP_DIG_P5_REG);
  dig_P6 = read16(BMP_DIG_P6_REG);
  dig_P7 = read16(BMP_DIG_P7_REG);
  dig_P8 = read16(BMP_DIG_P8_REG);
  dig_P9 = read16(BMP_DIG_P9_REG);

  Serial.println("dig_T1: " + String(dig_T1));
  Serial.println("dig_T2: " + String(dig_T2));
  Serial.println("dig_T3: " + String(dig_T3));

  Serial.println("dig_P1: " + String(dig_P1));
  Serial.println("dig_P2: " + String(dig_P2));
  Serial.println("dig_P3: " + String(dig_P3));
  Serial.println("dig_P4: " + String(dig_P4));
  Serial.println("dig_P5: " + String(dig_P5));
  Serial.println("dig_P6: " + String(dig_P6));
  Serial.println("dig_P7: " + String(dig_P7));
  Serial.println("dig_P8: " + String(dig_P8));
  Serial.println("dig_P9: " + String(dig_P9));

  write8(BMP_FILTER_REG, BMP_FILTER_MODES);
  write8(BMP_CONFIG_REG, BMP_MODES);
}

float Sensor::getTemperature()
{
  return temperature;
}

float Sensor::getPressure()
{
  return pressure;
}

int32_t Sensor::readRawData(byte msbReg, byte lsbReg, byte xlsbReg) 
{
  uint8_t msb = read8(msbReg);
  uint8_t lsb = read8(lsbReg);
  uint8_t xlsb = read8(xlsbReg);

  return (int32_t) (msb << 12) | (lsb << 4) | (xlsb >> 4);
}

int32_t Sensor::readRawTemperature() 
{
  return readRawData(BMP_TEMP_MSB_REG, BMP_TEMP_LSB_REG, BMP_TEMP_XLSB_REG);
}

int32_t Sensor::readRawPressure() 
{
  return readRawData(BMP_PRESS_MSB_REG, BMP_PRESS_LSB_REG, BMP_PRESS_XLSB_REG);
}

float Sensor::compensateTemperature(int32_t adc_T) 
{
  // Taken from data sheet page 44: 
  // 8. appendix 1: Calculation formulae for 32-bit systems
  // 8.1 Compensation formula in floating point
  double var1, var2, T;
  var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
  var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) *
  (((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
  t_fine = (int32_t)(var1 + var2);
  T = (var1 + var2) / 5120.0;
  return T;
}

float Sensor::compensatePressure(int32_t adc_P)
{
  // Taken from data sheet page 44: 
  // 8. appendix 1: Calculation formulae for 32-bit systems
  // 8.1 Compensation formula in floating point
  double var1, var2, p;
  var1 = ((double)t_fine/2.0) - 64000.0;
  var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)dig_P5) * 2.0;
  var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
  var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
  if (var1 == 0.0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576.0 - (double)adc_P;
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double)dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double)dig_P8) / 32768.0;
  p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
  return p/100.0;
}

bool Sensor::readTemperatureAndPressure()
{
  try
  {
    // read temperature (Berechnung der Werte mit trimming values) 

    // read pressure (Berechnung der Werte mit trimming values)

    // Datenfeld für temperature und pressure sind gegeben (in sensor.h Datei schauen)

    int32_t adc_T = readRawTemperature();
    int32_t adc_P = readRawPressure();

    temperature = compensateTemperature(adc_T);
    pressure = compensatePressure(adc_P);
    
    Serial.print("\nRAW_TEMP: ");
    Serial.print(adc_T, BIN);
    Serial.print(", Temp: ");
    Serial.print(temperature);

    Serial.print("\nRAW_PRESS: ");
    Serial.print(adc_P, BIN);
    Serial.print(", Press: ");
    Serial.print(pressure);
    Serial.println("");

    // forced mode -> mode has to be reset every cycle
    write8(BMP_CONFIG_REG, BMP_MODES);

    return true;
  }
  catch (uint8_t err)
  {
    Serial.println("Sensor error: " + String(err));
    return false;
  }
}