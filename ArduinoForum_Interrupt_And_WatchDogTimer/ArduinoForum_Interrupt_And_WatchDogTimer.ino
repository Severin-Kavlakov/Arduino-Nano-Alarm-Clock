
// Using the watchdog timer, 
//  sleep for the given number of intervals,
//  then print a message. 
// Prints a dot at each WDT wakeup.
// A button is also connected to provide an external interrupt, 
//  print a message when the button is pressed.
// Finally, blink an LED after each wake-up regardless of source.

// Using an ATmega328P at 16MHz and 5V, draws ~6.3µA while sleeping,
//  which is consistent with only the WDT running.
//
//Jack Christensen 19Nov2013     CC BY-SA, see http://creativecommons.org/licenses/by-sa/3.0/

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/


const int BTN_PIN = 2;            // wire a button from pin 2 to ground
const int LED_PIN = 13;           // wire an LED from pin 13 to ground through a proper current-limiting resistor
const int WDT_INTERVALS = 50;     // number of WDT timeouts before printing message
const unsigned long LED_ON_TIME = 200;  // turn on LED for this many ms after each wake-up
const long BAUD_RATE = 9600;

volatile boolean extInterrupt;    // external interrupt flag (button)
volatile boolean wdtInterrupt;    // watchdog timer interrupt flag




void gotoSleep(void)
{
  byte adcsra = ADCSRA;          // save the ADC Control and Status Register A
  ADCSRA = 0;                    // disable the ADC
  EICRA = _BV(ISC01);            // configure INT0 to trigger on falling edge
  EIFR = _BV(INTF0);             // ensure interrupt flag cleared
  EIMSK = _BV(INT0);             // enable INT0
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    wdtInterrupt = false;
    extInterrupt = false;
    sleep_enable();
    sleep_bod_disable();       // disable brown-out detection (saves 20-25µA)
  }
  sleep_cpu();                   // go to sleep
  sleep_disable();               // wake up here
  ADCSRA = adcsra;               // restore ADCSRA
}


//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
  EIMSK = 0;  // disable external interrupts (only need one to wake up)
  extInterrupt = true;
}


// handles the Watchdog Time-out Interrupt
ISR(WDT_vect)
{
  wdtInterrupt = true;
}




void setup(void)
{
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wdt_reset();
    MCUSR &= ~_BV(WDRF);                            // clear WDRF
    WDTCSR |= _BV(WDCE) | _BV(WDE);                 // enable WDTCSR change
    WDTCSR =  _BV(WDIE) | _BV(WDP3) | _BV(WDP0);    // ~8 sec
  }

  Serial << endl << F("Setup complete");
}

void loop(void)
{
  static int wdtCount;
  static int btnCount;

  if (wdtCount == 0 || extInterrupt) Serial << endl << F("Sleep");
  Serial.flush();
  Serial.end();
  gotoSleep();

  digitalWrite(LED_PIN, HIGH); // blink LED on each wakeup
  delay(LED_ON_TIME);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(BAUD_RATE);

  if (wdtInterrupt) {
    Serial << '.';

    if (++wdtCount >= WDT_INTERVALS) {
      Serial << endl << F("WDT: ") << wdtCount;
      wdtCount = 0;
    }
  }
  if (extInterrupt) {
    Serial << endl << F("Button: ") << ++btnCount;
  }
}
