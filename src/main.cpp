#include <Arduino.h>
#include <LiquidCrystal.h>
#include "ArduinoLowPower.h"




typedef const uint8_t pin;

pin 
  RS = 12, 
  E  = 11,
  D4 = 5,
  D5 = 4,
  D6 = 3, 
  D7 = 2;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

pin potentiometerChooseTime = A1;

pin stopAlarm = 6;
pin setAlarm = 7;
pin chooseHours = 8;
pin chooseMinutes = 9;

pin activeBuzzer = 10;

pin wakeUpToResetAlarm = A0;




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
const uint8_t START_HOUR   = 17, 
              START_MINUTE = 42;

const uint8_t  msCheckButtons     = 25; 
const uint16_t msFindCurrentTime  = 5000; 
const uint16_t msDebugToSerial    = 500;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


uint32_t ms = 0; //
uint32_t prevMs_FindCurrentTime = 0, 
         prevMs_CheckButtons = 0, 
         prevMs_debugToSerial = 0;

uint8_t endHour = 0, endMinute = 0;//
uint8_t currentHour = 0, currentMinute = 0;//
uint8_t waitHours = 0, waitMinutes = 0;//

uint32_t minutesSinceBoot = 0;//
uint32_t totalMinutes = 0;//


bool  setAlarmState      = false,   prev_setAlarmState      = false;
bool  stopAlarmState     = false,   prev_stopAlarmState     = false;
bool  chooseHoursState   = false,   prev_chooseHoursState   = false;
bool  chooseMinutesState = false,   prev_chooseMinutesState = false;

uint8_t cursorAtHours = 255; // other number than 0 or 1 //





// if start hour between 0 and 24 ; if start minute between 0 and 60  
bool is_start_hour_and_minute_correct(uint8_t START_HOUR, uint8_t START_MINUTE) {
  return (START_HOUR   > 0  &&  START_HOUR   < 24)
      &&
         (START_MINUTE > 0  &&  START_MINUTE < 60);
}


// turn ints to time eg 7 -> 07
char buffer[3];
char* format_uint8_t(uint8_t i) {
  sprintf(buffer, "%02d", i); // 0 added at front
  return buffer;
}


// turn alarm on or off
void switch_alarm(bool on) {
  while(on==true) {
    tone(activeBuzzer, 5000, 1000);
    tone(activeBuzzer, 2000, 1000);
  }
}



// curr time = start time + time after boot
void find_current_time( uint32_t currentMillis,
                        const uint8_t START_HOUR, const uint8_t START_MINUTE,
                        uint8_t& currentHour,     uint8_t& currentMinute 
                      )
// NOT passing by pointer BUT by address, C++ treats every instance of &x as x - saves unnecessary syntax 
{
  minutesSinceBoot = (currentMillis/1000) / 60;
  
  uint16_t minutesOnDayOfBoot = (START_HOUR * 60) + START_MINUTE;

  totalMinutes = minutesSinceBoot + minutesOnDayOfBoot;


  uint16_t minutesCurrentDay = totalMinutes % (24*60);

  currentHour   = minutesCurrentDay / 60; // divide by 60 = hours, int division excludes minutes
  currentMinute = minutesCurrentDay % 60; // leftover from division by 60 = ONLY minutes
}


// calc wait time for Arduino to sleep, based on current time and alarm time
void calc_wait_time( uint8_t  currentHour, uint8_t  currentMinute,
                     uint8_t  endHour,     uint8_t  endMinute,
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
void set_alarm(uint8_t waitHours, uint8_t waitMinutes) {
  // SLEEP for calculated wait time
  uint32_t waitMs =  1000*(  (waitHours*60)*60 
                            +
                              waitMinutes*60    );

  
  //temp
  delay(waitMs);

  // SWITCH ALARM ON
  switch_alarm(true);

}






void setup() {
  delay(3000);

  pinMode(potentiometerChooseTime, INPUT);
  pinMode(stopAlarm,               INPUT);
  pinMode(setAlarm,                INPUT);
  pinMode(chooseHours,             INPUT);
  pinMode(chooseMinutes,           INPUT);
  pinMode(activeBuzzer, OUTPUT);


  lcd.begin(16, 2);

  lcd.setCursor(2, 0); lcd.print(":");
  lcd.setCursor(13, 0); lcd.print(":");

  Serial.begin(9600);


  if(! is_start_hour_and_minute_correct(START_HOUR, START_MINUTE)) {
    lcd.setCursor(0, 0); lcd.print("hours arent 0-24");
    lcd.setCursor(0, 1); lcd.print("mins arent 0-24");
    Serial.print("ERROR: start HOURS must be 0-24 , start MINTUES must be 0-60 !!!");
    while(1);
  }
}




void loop() {

  ms = millis();

  //check buttons for presses every 50ms
  if(ms - prevMs_CheckButtons >= msCheckButtons) {

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

      endHour  = map(analogRead(potentiometerChooseTime),    0, 1023,     0, 24);

      lcd.setCursor(0, 0); lcd.print(format_uint8_t(endHour));
    }
    if(cursorAtHours == 0) {

      endMinute = map(analogRead(potentiometerChooseTime),   0, 1023,     0, 60);

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


    prevMs_CheckButtons = ms;
  }





  // update current time on display every N seconds
  if(ms - prevMs_FindCurrentTime >= msFindCurrentTime) { 
    
    find_current_time(ms, START_HOUR, START_MINUTE, currentHour, currentMinute);

    lcd.setCursor(11, 0); lcd.print(format_uint8_t(currentHour)); // current time
    lcd.setCursor(14, 0); lcd.print(format_uint8_t(currentMinute)); // current time

    prevMs_FindCurrentTime = ms;
  }




  //print debug info to serial every 500ms
  if(ms - prevMs_debugToSerial >= msDebugToSerial) {
    
    Serial.print(ms); Serial.print(" ms from boot");  
    
    Serial.print("    ");

    Serial.print(endHour);   Serial.print(" alarm hour"); Serial.print("  ");
    Serial.print(endMinute); Serial.print(" alarm minute");
    
    Serial.print("    ");

    Serial.print(currentHour);   Serial.print(" current hour"); Serial.print("  ");
    Serial.print(currentMinute); Serial.print(" current minute");
    
    Serial.print("    ");
    
    Serial.print(waitHours);   Serial.print(" wait hours");  Serial.print("  ");
    Serial.print(waitMinutes); Serial.print(" wait minutes"); 

    Serial.print("    ");

    Serial.print(minutesSinceBoot); Serial.print(" minutes since boot"); Serial.print("  ");
    Serial.print(totalMinutes);     Serial.print(" minutes from 00:00am on day of boot TO this moment"); 
    
    Serial.print("    ");

    Serial.print(cursorAtHours);


    Serial.println(" ");
  }

}
