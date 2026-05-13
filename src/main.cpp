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

pin stopAlarm = 6;
pin setAlarm = 7;
pin chooseHours = 8;
pin chooseMinutes = 9;

pin activeBuzzer = 10;





// VARIABLES ------------------------------------------------------------------------------

bool cursorAtHours=true;

uint64_t currentMillis = 0;
uint64_t prevMillis = 0;

bool  setAlarmState=false,      prev_setAlarmState=false;
bool  stopAlarmState=false,     prev_stopAlarmState=false;
bool  chooseHoursState=false,   prev_chooseHoursState=false;
bool  chooseMinutesState=false, prev_chooseMinutesState=false;




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

void startAlarm() {
  tone(activeBuzzer, 5000, 1000);
}





void setup() {
  lcd.begin(16, 2);

  lcd.setCursor(3, 0); lcd.print(":");

  delay(1000);

}


void loop() {

(digitalRead(chooseHours)) ?   chooseHoursState = true : chooseHoursState = false;
if (chooseHoursState == false && prev_chooseHoursState == true) {
  cursorAtHours = true;
  lcd.setCursor(4,1); lcd.print("  ");
  lcd.setCursor(1,1); lcd.print("^^");
} 
prev_chooseHoursState = chooseHoursState;

(digitalRead(chooseMinutes)) ?   chooseMinutesState = true : chooseMinutesState = false;
if (chooseMinutesState == false && prev_chooseMinutesState == true) {
  cursorAtHours = false;
  lcd.setCursor(1,1); lcd.print("  ");
  lcd.setCursor(4,1); lcd.print("^^");
} 
prev_chooseMinutesState = chooseMinutesState;






(digitalRead(setAlarm)) ?   setAlarmState = true : setAlarmState = false;
if (setAlarmState == false && prev_setAlarmState == true) {
  startAlarm();
}
prev_setAlarmState = setAlarmState;



 
}
