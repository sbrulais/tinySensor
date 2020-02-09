/*
 * This file is part of the TinySensor distribution (https://github.com/arcadien/TinySensor)
 *
 * Copyright (c) 2019 Aurélien Labrosse
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <inttypes.h>

// sleep 32s between each sample and
// emission
#define SLEEP_TIME_IN_SECONDS (8 * 4)

#define LED_PIN PB1
#define PWM_PIN PA7
#define TX_RADIO_PIN PB0
#define RADIO_POWER_PIN PA2
#define BAT_SENSOR_PIN PA1


/* 
* actual value for 1v1 internal ref,
* which have to be checked for each MCU
* \see https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
*
*
* Briefly, the reference can be obtained by 
* `internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)`
*/
#define ACTUAL_1V1_REF_MV 1100.

#ifdef BOARD_USES_3V3_STEPUP
#define ACTUAL_VCC_MV 3280.
#else
#define ACTUAL_VCC_MV readVcc()
#endif

// Sensors
#define USE_BMP280
//#define USE_BME280
//#define USE_DS18B20

#if defined(USE_BMP280) or defined(USE_BME280)
#define USE_I2C
#endif

const uint8_t OREGON_ID = SENSOR_ID;
const uint8_t OREGON_TYPE[] = SENSOR_TYPE;

/*
 * MODE_0 0 // Temperature only [THN132N]
 * MODE_1 1 // Temperature + Humidity [THGR2228N]
 * MODE_2 2 // Temperature + Humidity + Baro() [BTHR918N]
 */
// #define OREGON_MODE MODE_1
