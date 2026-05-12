// LIBRARIES ------------------------------------------------------------------------------

#include <Arduino.h>
#include <LiquidCrystal.h>


// PINS -----------------------------------------------------------------------------------
typedef const uint8_t pin;

pin 
  RS = 12, 
  E  = 11,
  D4 = 5,
  D5 = 4,
  D6 = 3, 
  D7 = 2;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

pin potentiometerChooseTime = A0;
pin setAlarm = 6;
pin stopAlarm = 7;
pin cursorToHours = 8;
pin cursorToMinutes = 9;

pin activeBuzzer = 10;





// VARIABLES ------------------------------------------------------------------------------

bool  buttonState=false, prevButtonState=false;




// FUNCTIONS ------------------------------------------------------------------------------
/*
char bufferFloat[5];
char* float_out(float f) {
  dtostrf(f, sizeof(bufferFloat), 2, bufferFloat); //decimals
  return bufferFloat;
}

char bufferUint16[5];
char* uint16_t_out(uint16_t i) {
  sprintf(bufferUint16, "%5d", i);
  return bufferUint16;
}
*/

void outHz(uint8_t freaquencyHz) { //needs work on the i<100 part
  uint8_t delayMs = freaquencyHz/1000;

  for(uint16_t i=0;   i < 100;   i++) {
    digitalWrite(activeBuzzer, HIGH); delay(delayMs);
    digitalWrite(activeBuzzer, LOW ); delay(delayMs);
  }
}




// MAIN -----------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);






  delay(1000); //debigging
}




void loop() {
/*
  (digitalRead(buttonPin)) ?    buttonState = true : buttonState = false; // read button
  if (buttonState == false && prevButtonState == true) {                  // if button pressed




  }
  prevButtonState = buttonState; // 2nd sample of button to check if pressed 
*/
  



  

  //Serial.print(); Serial.print(" ");

  //Serial.print("      ");

  Serial.println("");




  delay(50);
}
