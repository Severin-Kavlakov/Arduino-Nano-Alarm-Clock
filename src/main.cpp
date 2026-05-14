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

uint64_t currentMillis = 0, prevMillis = 0;

bool  setAlarmState      = false,   prev_setAlarmState      = false;
bool  stopAlarmState     = false,   prev_stopAlarmState     = false;
bool  chooseHoursState   = false,   prev_chooseHoursState   = false;
bool  chooseMinutesState = false,   prev_chooseMinutesState = false;

bool cursorAtHours=true;

const uint8_t START_HOUR   = 22;
const uint8_t START_MINUTE = 06;

uint8_t endHour   = 0;
uint8_t endMinute = 0;

uint8_t currentHour   = 0; //need function to calc this
uint8_t currentMinute = 0;




// FUNCTIONS ------------------------------------------------------------------------------


// format uint8_t to be 2 chars wide ALWAYS 
char bufferUint8[2];
char* format_uint8_t(uint8_t i) { // WILL prolly need custom funct to print 0 before 1 sized ints
  sprintf(bufferUint8, "%2d", i);
  return bufferUint8;
}


void switch_alarm(bool on) {
  while(on==true) {
    tone(activeBuzzer, 5000, 1000);
    tone(activeBuzzer, 2000, 1000);
  }
}


void calc_wait_time(uint8_t endHour, uint8_t endMinute, 
              uint8_t START_HOUR, uint8_t START_MINUTE)
{
  // calc wait time
  // wait from calculated wait time ; PUT TO SLEEP
  // SWITCH ALARM
}


void find_current_time( uint64_t currentMillis,
                        uint8_t currentHour, uint8_t currentMinute,    
                        uint8_t START_HOUR, uint8_t START_MINUTE) 
{
  //convert millis passed from start to minutes passed from start

  // curr time = start time + converted into minutes
}








void setup() {
  lcd.begin(16, 2);

  lcd.setCursor(2, 0); lcd.print(":");

  lcd.print(7, 0); lcd.print(":");

  delay(1000);
}


void loop() {

currentMillis = millis();


(digitalRead(chooseHours)) ?   chooseHoursState = true : chooseHoursState = false;
if (chooseHoursState == false && prev_chooseHoursState == true) {
  cursorAtHours = true;
  lcd.setCursor(3,1); lcd.print("  ");
  lcd.setCursor(0,1); lcd.print("^^");
}
prev_chooseHoursState = chooseHoursState;


(digitalRead(chooseMinutes)) ?   chooseMinutesState = true : chooseMinutesState = false;
if (chooseMinutesState == false && prev_chooseMinutesState == true) {
  cursorAtHours = false;
  lcd.setCursor(0,1); lcd.print("  ");
  lcd.setCursor(3,1); lcd.print("^^");
}
prev_chooseMinutesState = chooseMinutesState;




if(cursorAtHours) {
  endHour = map(  analogRead(potentiometerChooseTime),    0, 1023,     0, 13);

  lcd.setCursor(0, 0); lcd.print(format_uint8_t(endHour));
}
else {
  endMinute = map( analogRead(potentiometerChooseTime),   0, 1023,     0, 60);

  lcd.setCursor(3, 0); lcd.print(format_uint8_t(endMinute));
}




(digitalRead(setAlarm)) ?   setAlarmState = true : setAlarmState = false;
if (setAlarmState == false && prev_setAlarmState == true) {
  switch_alarm(true);
}
prev_setAlarmState = setAlarmState;


(digitalRead(stopAlarm)) ?   stopAlarmState = true : stopAlarmState = false;
if (stopAlarmState == false && prev_stopAlarmState == true) {
  switch_alarm(false);
}
prev_stopAlarmState = stopAlarmState;


if(currentMillis - prevMillis >= 1000) {
  lcd.setCursor(11, 0); lcd.print(format_uint8_t()); // current time
  lcd.setCursor(14, 0); lcd.print(format_uint8_t()); // current time

  prevMillis = currentMillis;
}

}
