#define FCPU 9600000UL
#include <avr/io.h>

uint16_t adc_read(uint8_t mux) {
  ADMUX = (0<<REFS0) // vcc reference
    | (0<<ADLAR) // left-adjusted values
    | (mux<<MUX0);
  ADCSRB = 0;
  ADCSRA = (1<<ADEN) // adc enable
    | (1<<ADSC)  // start conversion
    | (0<<ADATE) // auto trigger
    | (0<<ADIF)  // interrupt flag
    | (1<<ADIE)  // interrupt enable
    | (1<<ADPS2) // prescale division by 32
    | (0<<ADPS1)
    | (1<<ADPS0);
  while( ADCSRA & (1<<ADSC) ) { }
  return ADC;
}

int main(void) {

  DIDR0 = ( 1 << ADC3D ) | ( 1 << ADC2D );
  DDRB = 0b00000100;
  PORTB = 0b00000000;

  uint8_t out_pin = 0b00000100;
  uint8_t sync_pin = 0b00000001;
  uint32_t val = 0;
  uint8_t sync = PINB & sync_pin;
  for( ;; ) {
    uint32_t rateVal = adc_read(3);
    rateVal = (rateVal + 8) << 12;
    uint32_t dutyVal = adc_read(2);
    dutyVal = dutyVal << 22;
    val = val + rateVal;
    uint8_t syncVal = PINB & sync_pin;
    if( syncVal == sync_pin && sync != sync_pin ) {
      val = 0;
    }
    sync = syncVal;
    if( val < dutyVal ) {
      PORTB |= out_pin;
    } else {
      PORTB &= ~out_pin;
    }
  }
}
