/**
 * @file CosaLCD10DOF.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * @section Description
 * Cosa demonstration of 10 DOF module (GY-80) with ADXL345, BMP085, 
 * HMC5883L, and L3G4200D; 3-axis acceleratometer, thermometer,
 * barometer, 3-axis compass and 3-axis gyroscope with output to LCD.
 *
 * @section Circuit
 * The GY-80 10DOF module with pull-up resistors (4K7) for TWI signals
 * and 3V3 internal voltage converter. For LCD wiring see HD44780.hh.
 *
 *                           GY-80
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 *                     2-|3V3         |
 * (GND)---------------3-|GND         |
 * (A5/SCL)------------4-|SCL         |
 * (A4/SDA)------------5-|SDA         |
 *                     6-|M-DRDY      |
 *                     7-|A-INT1      |
 *                     8-|T-INT1      |
 *                     9-|P-XCLR      |
 *                    10-|P-EOC       |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/TWI/Driver/ADXL345.hh"
#include "Cosa/TWI/Driver/BMP085.hh"
#include "Cosa/TWI/Driver/HMC5883L.hh"
#include "Cosa/TWI/Driver/L3G4200D.hh"

// LCD and communication port
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::MJKDZ port;
HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::ERM1602_5 port;
// HD44780 lcd(&port, 20, 4);
HD44780 lcd(&port);

// Digital acceleratometer with alternative address
ADXL345 acceleratometer(1);

// Digital temperature and pressure sensor
BMP085 bmp;

// The 3-Axis Digital Compass
HMC5883L compass;

// Digital Gyroscope using alternative address
L3G4200D gyroscope(1);

void setup()
{
  // Start the watchdog ticks
  Watchdog::begin();

  // Start trace output stream on the LCD
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCD10DOF"));
  sleep(2);
  
  // Start the sensors
  acceleratometer.begin();
  bmp.begin(BMP085::ULTRA_LOW_POWER);
  compass.set_output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.set_samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.set_range(HMC5883L::RANGE_4_0_GA);
  compass.set_mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);
  compass.begin();
  gyroscope.begin();
}

void loop()
{
  // Read barometer; pressure and temperature
  bmp.sample();
  trace << clear;
  trace << PSTR("Barometer") << endl;
  trace << bmp.get_pressure() << PSTR(" Pa, ");
  trace << (bmp.get_temperature() + 5) / 10 << PSTR(" C");
  sleep(2);

  // Read compass heading
  compass.read_heading();
  compass.to_milli_gauss();
  HMC5883L::data_t dir;
  compass.get_heading(dir);
  trace << clear;
  trace << PSTR("Compass") << endl;
  trace << dir.x << PSTR(", ") 
	<< dir.y << PSTR(", ") 
	<< dir.z;
  sleep(2);

  // Read acceleration 
  trace << clear;
  ADXL345::sample_t acc;
  acceleratometer.sample(acc);
  trace << PSTR("Accelerometer") << endl;
  trace << acc.x << PSTR(", ")
	<< acc.y << PSTR(", ")
	<< acc.z;
  sleep(2);

  // Read gyroscope
  trace << clear;
  L3G4200D::sample_t rate;
  gyroscope.sample(rate);
  trace << PSTR("Gyroscope") << endl;
  trace << rate.x << PSTR(", ")
	<< rate.y << PSTR(", ")
	<< rate.z;
  sleep(2);

  // Read battery 
  trace << clear;
  trace << PSTR("Battery") << endl;
  trace << AnalogPin::bandgap() << PSTR(" mV");
  sleep(2);
}
