#include <avr/io.h>
#include <util/delay.h>
#include <config.h>

/*
* \see https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
*
*/
long readVcc()
{
    uint8_t ADMUX_backup = ADMUX;

// Read 1.1V reference against AVcc
// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84a__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
#else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

    _delay_ms(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC))
    {
    }
    long result = (ACTUAL_1V1_REF_MV * 1023) / ADC;

    ADMUX = ADMUX_backup;
    return result;
}