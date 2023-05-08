/***************************************************
  This is a library for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749

  These sensors use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_MLX90614_B.h"

Adafruit_MLX90614_B::Adafruit_MLX90614_B(uint8_t i2caddr) {
  _addr = i2caddr;
}


boolean Adafruit_MLX90614_B::begin(void) {
  Wire1.begin();

  /*
  for (uint8_t i=0; i<0x20; i++) {
    Serial.print(i); Serial.print(" = ");
    Serial.println(read16(i), HEX);
  }
  */
  return true;
}

//////////////////////////////////////////////////////


double Adafruit_MLX90614_B::readObjectTempF(void) {
  return (readTemp(MLX90614_TOBJ1) * 9 / 5) + 32;
}


double Adafruit_MLX90614_B::readAmbientTempF(void) {
  return (readTemp(MLX90614_TA) * 9 / 5) + 32;
}

double Adafruit_MLX90614_B::readObjectTempC(void) {
  return readTemp(MLX90614_TOBJ1);
}


double Adafruit_MLX90614_B::readAmbientTempC(void) {
  return readTemp(MLX90614_TA);
}

float Adafruit_MLX90614_B::readTemp(uint8_t reg) {
  float temp;

  temp = read16(reg);
  temp *= .02;
  temp  -= 273.15;
  return temp;
}

/*********************************************************************/

uint16_t Adafruit_MLX90614_B::read16(uint8_t a) {
  uint16_t ret;

  Wire1.beginTransmission(_addr); // start transmission to device
  Wire1.write(a); // sends register address to read from
  Wire1.endTransmission(false); // end transmission

  Wire1.requestFrom(_addr, (uint8_t)3);// send data n-bytes read
  ret = Wire1.read(); // receive DATA
  ret |= Wire1.read() << 8; // receive DATA

  uint8_t pec = Wire1.read();

  return ret;
}
