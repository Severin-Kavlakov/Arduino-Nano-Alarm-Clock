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

uint64_t currentMillis = 0;

uint64_t prevMillis_PrintCurrentTime = 0, prevMillis_FindCurrentTime = 0, prevMillis_CheckButtons = 0;

bool  setAlarmState      = false,   prev_setAlarmState      = false;
bool  stopAlarmState     = false,   prev_stopAlarmState     = false;
bool  chooseHoursState   = false,   prev_chooseHoursState   = false;
bool  chooseMinutesState = false,   prev_chooseMinutesState = false;

uint8_t cursorAtHours = 2; // other number than 0 or 1 

const uint8_t START_HOUR   = 19;
const uint8_t START_MINUTE = 26;

uint8_t endHour   = 0;
uint8_t endMinute = 0;

uint8_t currentHour   = 0;
uint8_t currentMinute = 0;

uint8_t waitHours = 0;
uint8_t waitMinutes = 0;






// format uint8_t to be 2 chars wide ALWAYS 
char bufferUint8[3];
char* format_uint8_t(uint8_t i) { // WILL prolly need custom funct to print 0 before 1 sized ints
  sprintf(bufferUint8, "%02d", i); // 0 - added
  return bufferUint8;
}


// turn alarm on or off
void switch_alarm(bool on) {
  while(on==true) {
    tone(activeBuzzer, 5000, 1000);
    tone(activeBuzzer, 2000, 1000);
  }
}


// curr time = start time + time after boot
void find_current_time( uint32_t currentMillis, //temp 32
                        const uint8_t START_HOUR, const uint8_t START_MINUTE,
                        uint8_t& currentHour,     uint8_t& currentMinute 
                      )
// NOT passing by pointer BUT by address, C++ treats every instance of &x as x - saves unnecessary syntax 
{
  uint32_t minutesFromStart = (currentMillis/1000) / 60;
  
  uint16_t minutesFromDayOfBoot = (START_HOUR * 60) + START_MINUTE;

  uint32_t totalMinutes = minutesFromStart + minutesFromDayOfBoot;


  uint16_t minutesFromCurrentDay = totalMinutes % (24*60);

  currentHour   = minutesFromCurrentDay / 60; // divide by 60 to get hours, int division excludes minutes
  currentMinute = minutesFromCurrentDay % 60; // leftover from division by 60 = ONLY minutes

  /*
  Serial.print(minutesFromCurrentDay); Serial.print("  "); 
  Serial.print(minutesFromStart); Serial.print("  ");
  Serial.print(currentMillis); Serial.print("  ");
  Serial.print(totalMinutes);
  
  
  Serial.println(""); */
}


// calc wait time for Arduino to sleep, based on current time and alarm time
void calc_wait_time( uint8_t currentHour, uint8_t currentMinute,
                     uint8_t endHour,     uint8_t endMinute,
                     uint8_t& waitHours,   uint8_t& waitMinutes
                   )
{
  if (endHour > currentHour) { 

    waitHours = endHour - currentHour; 
  }
  else if (endHour < currentHour) {

    uint8_t partHours = 24 - currentHour;
    waitHours = partHours + endHour;  
  }
  else waitHours = 0;
  

  if (endMinute > currentMinute) {

    waitMinutes = endMinute - currentMinute;
  }
  else if (endMinute < currentMinute) { 

    uint8_t partMinutes = 60 - currentMinute;
    waitMinutes = partMinutes + endMinute;

    if (waitHours == 0) {
      waitHours = 23;
    }
    else {
      waitHours -= 1; //IN MOST CASES: if end minutres < currentMinutes  => wait time 1 hour less
    }   

  }
  else waitMinutes = 0;
}


// put to sleep until alarm has to ring
void set_alarm() {
  // wait PUT TO SLEEP for calculated wait time


  // SWITCH ALARM ON

}






void setup() {
  lcd.begin(16, 2);

  lcd.setCursor(2, 0); lcd.print(":");

  lcd.setCursor(13, 0); lcd.print(":");

  delay(2000);

  Serial.begin(9600);
}



void loop() {

currentMillis = millis();


(digitalRead(chooseHours)) ?   chooseHoursState = true : chooseHoursState = false;
if (chooseHoursState == false && prev_chooseHoursState == true) {
  cursorAtHours = 1;
  lcd.setCursor(3,1); lcd.print("  ");
  lcd.setCursor(0,1); lcd.print("^^");
}
prev_chooseHoursState = chooseHoursState;

(digitalRead(chooseMinutes)) ?   chooseMinutesState = true : chooseMinutesState = false;
if (chooseMinutesState == false && prev_chooseMinutesState == true) {
  cursorAtHours = 0;
  lcd.setCursor(0,1); lcd.print("  ");
  lcd.setCursor(3,1); lcd.print("^^");
}
prev_chooseMinutesState = chooseMinutesState;




if(cursorAtHours == 1) {
  endHour = map(  analogRead(potentiometerChooseTime),    0, 1023,     0, 24);

  lcd.setCursor(0, 0); lcd.print(format_uint8_t(endHour));
}
if(cursorAtHours == 0){
  endMinute = map( analogRead(potentiometerChooseTime),   0, 1023,     0, 60);

  lcd.setCursor(3, 0); lcd.print(format_uint8_t(endMinute));
}




(digitalRead(setAlarm)) ?   setAlarmState = true : setAlarmState = false;
if (setAlarmState == false && prev_setAlarmState == true) {
  // set alarm
}
prev_setAlarmState = setAlarmState;

(digitalRead(stopAlarm)) ?   stopAlarmState = true : stopAlarmState = false;
if (stopAlarmState == false && prev_stopAlarmState == true) {
  switch_alarm(false);
}
prev_stopAlarmState = stopAlarmState;




// every 30 seconds update cuиrent time on display
if(currentMillis - prevMillis_PrintCurrentTime >= 30000) { 
  
  find_current_time(currentMillis, START_HOUR, START_MINUTE, currentHour, currentMinute); //temporary here

  lcd.setCursor(11, 0); lcd.print(format_uint8_t(currentHour)); // current time
  lcd.setCursor(14, 0); lcd.print(format_uint8_t(currentMinute)); // current time


  prevMillis_PrintCurrentTime = currentMillis;
}



}
