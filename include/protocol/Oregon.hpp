/*
 * connectingStuff, Oregon Scientific v2.1 Emitter
 * http://connectingstuff.net/blog/encodage-protocoles-oregon-scientific-sur-arduino/
 *
 * Copyright (C) 2013 olivier.lebrun@gmail.com
 *
 * Refactored to class by Aurélien Labrosse <https://github.com/arcadien>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include "protocol/Oregon.h"
#include <config.h>

#include <avr/io.h>
#include <util/delay.h>

template <int MODE>
uint8_t Oregon<MODE>::PREAMBLE[] = {0xFF, 0xFF};

template <int MODE>
uint8_t Oregon<MODE>::POSTAMBLE[] = {0x00};

template <int MODE>
void Oregon<MODE>::txPinLow() { PORTB &= ~_BV(PB0); }

template <int MODE>
void Oregon<MODE>::txPinHigh() { PORTB |= _BV(PB0); }

template <int MODE>
inline void Oregon<MODE>::sendZero(void)
{
  txPinHigh();
  _delay_us(TIME);
  txPinLow();
  _delay_us(TWOTIME);
  txPinHigh();
  _delay_us(TIME);
}

template <int MODE>
inline void Oregon<MODE>::sendOne(void)
{
  txPinLow();
  _delay_us(TIME);
  txPinHigh();
  _delay_us(TWOTIME);
  txPinLow();
  _delay_us(TIME);
}

template <int MODE>
inline void Oregon<MODE>::sendQuarterMSB(const uint8_t data)
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
}

template <int MODE>
inline void Oregon<MODE>::sendQuarterLSB(const uint8_t data)
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}

template <int MODE>
void Oregon<MODE>::sendData(uint8_t *data, uint8_t size)
{
  for (uint8_t i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}

template <int MODE>
void Oregon<MODE>::sendOregon(uint8_t *data, uint8_t size)
{
  sendPreamble();
  // sendSync();
  sendData(data, size);
  sendPostamble();
}

template <int MODE>
inline void Oregon<MODE>::sendPreamble(void) { sendData(PREAMBLE, 2); }

template <int MODE>
inline void Oregon<MODE>::sendPostamble(void)
{

  sendData(POSTAMBLE, 1);
}

template <int MODE>
inline void Oregon<MODE>::sendSync(void) { sendQuarterLSB(0xA); }

template <int MODE>
void Oregon<MODE>::setType(uint8_t *data, const uint8_t *type)
{
  data[0] = type[0];
  data[1] = type[1];
}

template <int MODE>
void Oregon<MODE>::setChannel(uint8_t *data, uint8_t channel) { data[2] = channel; }

template <int MODE>
void Oregon<MODE>::setId(uint8_t *data, uint8_t ID) { data[3] = ID; }

template <int MODE>
void Oregon<MODE>::setBatteryLevel(uint8_t *data, uint8_t level)
{
  if (!level)
    data[4] = 0x0C;
  else
    data[4] = 0x00;
}

template <int MODE>
void Oregon<MODE>::setTemperature(uint8_t *data, float temp)
{
  // Set temperature sign
  if (temp < 0)
  {
    data[6] = 0x08;
    temp *= -1;
  }
  else
  {
    data[6] = 0x00;
  }

  // Determine decimal and float part
  int tempInt = (int)temp;
  int td = (int)(tempInt / 10);
  int tf = (int)round((float)((float)tempInt / 10 - (float)td) * 10);

  int tempFloat = (int)round((float)(temp - (float)tempInt) * 10);

  // Set temperature decimal part
  data[5] = (td << 4);
  data[5] |= tf;

  // Set temperature float part
  data[4] |= (tempFloat << 4);
}

template <int MODE>
void Oregon<MODE>::setHumidity(uint8_t *data, uint8_t hum)
{
  data[7] = (hum / 10);
  data[6] |= (hum - data[7] * 10) << 4;
}

template <int MODE>
void Oregon<MODE>::setPressure(uint8_t *data, float pres)
{
  if ((pres > 850) && (pres < 1100))
  {
    data[8] = (int)round(pres) - 856;
    data[9] = 0xC0;
  }
}

template <int MODE>
int Oregon<MODE>::Sum(uint8_t count, const uint8_t *data)
{
  int s = 0;

  for (uint8_t i = 0; i < count; ++i)
  {
    s += (data[i] & 0xF0) >> 4;
    s += (data[i] & 0xF);
  }

  if (int(count) != count)
    s += (data[count] & 0xF0) >> 4;

  return s;
}

template <int MODE>
void Oregon<MODE>::calculateAndSetChecksum(uint8_t *data)
{
#if OREGON_MODE == MODE_0
  int s = ((Sum(6, data) + (data[6] & 0xF) - 0xa) & 0xff);
  data[6] |= (s & 0x0F) << 4;
  data[7] = (s & 0xF0) >> 4;

#elif OREGON_MODE == MODE_1
  data[8] = ((Sum(8, data) - 0xa) & 0xFF);

#else
  data[10] = ((Sum(10, data) - 0xa) & 0xFF);
#endif
}
