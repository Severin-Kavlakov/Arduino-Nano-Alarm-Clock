#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/sleep.h>

//TODO: System of menus where on start: map rotation of pot. to scroll thru modes - alarm ; timer
// alarm: on choose - choose current time ; enter ; choose alarm time ; enter ; wait
// timer: enter wait time ; wait

typedef const uint8_t pin;

pin 
  RS = 12, 
  E  = 11,
  D4 = 5,
  D5 = 4,
  D6 = 3, 
  D7 = 2;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


pin 
  potentiometerScroll = A0,

  setAlarmOrEnter = A1,
  resetAlarmOrExit = A2,

  stopAlarm = 6,

  chooseHoursOrFirst = 7,
  chooseMinutesOrSecond = 8,

buzzer = 9;






/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
const uint8_t START_HOUR   = 21, 
              START_MINUTE = 23;

const uint16_t msDebugToSerial = 20;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

uint32_t ms = 0;
uint32_t prevMs_SerialDebug = 0;

uint8_t alarmHour   = 0, alarmMinute = 0,
        currentHour = 0, currentMinute = 0,
        waitHours   = 0, waitMinutes = 0;
uint32_t waitMs = 0;

uint32_t minutesSinceBoot = 0;
uint32_t totalMinutes = 0;


bool  setAlarmState      = false,   prev_setAlarmState      = false;
bool  stopAlarmState     = false,   prev_stopAlarmState     = false;
bool  chooseHoursState   = false,   prev_chooseHoursState   = false;
bool  chooseMinutesState = false,   prev_chooseMinutesState = false;

uint8_t cursorAtHours = 255; /* other number than 0 or 1 */





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
void ring_alarm() {
  tone(buzzer, 5000);
  tone(buzzer, 1000);
}


// curr time = start time + time after boot
void find_current_time( uint32_t currentMillis,
                        const uint8_t START_HOUR, const uint8_t START_MINUTE,
                        uint8_t& currentHour,     uint8_t& currentMinute 
                      )
// not passing by pointer but by address , C++ treats every instance of &x as x , this saves unnecessary syntax 
{
  minutesSinceBoot = (currentMillis/1000) / 60;
  
  uint16_t minutesOnDayOfBoot = (START_HOUR * 60) + START_MINUTE;

  totalMinutes = minutesSinceBoot + minutesOnDayOfBoot;


  uint16_t minutesCurrentDay = totalMinutes % (24*60);

  currentHour   = minutesCurrentDay / 60; // divide by 60 = hours, int division excludes minutes
  currentMinute = minutesCurrentDay % 60; // leftover from division by 60 = ONLY minutes
}


// based on the current time and alarm time, calculate the wait time for the Arduino to sleep
void calc_wait_time( uint8_t  currentHour, uint8_t  currentMinute,
                     uint8_t  endHour,     uint8_t  alarmMinute,
                     uint8_t& waitHours,   uint8_t& waitMinutes
                   )
{
  if      (endHour > currentHour) waitHours = endHour - currentHour; 
  else if (endHour < currentHour) {

    uint8_t partHours = 24 - currentHour;
    waitHours = partHours + endHour;  
  }
  else waitHours = 0;
  

  if      (alarmMinute > currentMinute) waitMinutes = alarmMinute - currentMinute;
  else if (alarmMinute < currentMinute) { 

    uint8_t partMinutes = 60 - currentMinute;
    waitMinutes = partMinutes + alarmMinute;

    (waitHours == 0) ?    waitHours = 23 : waitHours -= 1; //IN MOST CASES: if end minutres < currentMinutes => wait time is 1 hour less        
  
  }
  else waitMinutes = 0;
}


// put to sleep until the alarm needs to ring
void set_alarm(uint8_t waitHours, uint8_t waitMinutes) {
  
  waitMs =  1000UL*( (uint32_t)waitHours*60*60 + (uint32_t)waitMinutes*60 );

  Serial.println("                                                                                                              ");
  Serial.print(waitMs); Serial.print("ms wait");


  // sleep for calculated wait time ; wake up if reset alarm button pressed
  delay(waitMs); // TEMPORARY


  ring_alarm();
}






void setup() {
delay(3000);

Serial.begin(115200);

for(uint8_t freePin=A2; freePin <= A7; freePin++) {
  pinMode(freePin, INPUT);
  //Serial.println(freePin);//debug
}

pinMode(potentiometerScroll,   INPUT_PULLUP);

pinMode(setAlarmOrEnter,       INPUT_PULLUP);
pinMode(resetAlarmOrExit,      INPUT_PULLUP);


pinMode(stopAlarm,             INPUT_PULLUP);

pinMode(chooseHoursOrFirst,    INPUT_PULLUP);
pinMode(chooseMinutesOrSecond, INPUT_PULLUP);

pinMode(buzzer, OUTPUT);

lcd.begin(16, 2);

if (! is_start_hour_and_minute_correct(START_HOUR, START_MINUTE)) {
  lcd.setCursor(0, 0); lcd.print("HOURS ARENT 0-23");
  lcd.setCursor(0, 1); lcd.print("MINS ARENT 0-59");
  Serial.print("ERROR: start HOURS must be 0-23 , start MINTUES must be 0-59 !!!");
  while(1);
}



}




void loop() {

ms = millis();

chooseHoursState = !digitalRead(chooseHoursOrFirst);
if (chooseHoursState == false && prev_chooseHoursState == true) cursorAtHours = 1;
prev_chooseHoursState = chooseHoursState;


chooseMinutesState = !digitalRead(chooseMinutesOrSecond);
if (chooseMinutesState == false && prev_chooseMinutesState == true) cursorAtHours = 0;
prev_chooseMinutesState = chooseMinutesState;


if (cursorAtHours == 1) {
  lcd.setCursor(0,1); lcd.print("^^");
  lcd.setCursor(3,1); lcd.print("  ");

  alarmHour   = map(analogRead(potentiometerScroll),   0, 1023,     0, 24);
  lcd.setCursor(0, 0); lcd.print(format_uint8_t(alarmHour)); lcd.print(":");
}

if (cursorAtHours == 0) {
  lcd.setCursor(0,1); lcd.print("  ");
  lcd.setCursor(3,1); lcd.print("^^");

  alarmMinute = map(analogRead(potentiometerScroll),   0, 1023,     0, 60);
  lcd.setCursor(3, 0); lcd.print(format_uint8_t(alarmMinute));
}




find_current_time(ms, START_HOUR, START_MINUTE, currentHour, currentMinute);

calc_wait_time(currentHour, currentMinute, alarmHour, alarmMinute, waitHours, waitMinutes);

lcd.setCursor(6, 0); lcd.print("Time ");
lcd.print(format_uint8_t(currentHour)); lcd.print(":"); lcd.print(format_uint8_t(currentMinute));




setAlarmState = !digitalRead(setAlarmOrEnter);
if (setAlarmState == false && prev_setAlarmState == true) {

  lcd.setCursor(6, 1); lcd.print("Alarm");
  lcd.print(format_uint8_t(waitHours)); lcd.print(":"); lcd.print(format_uint8_t(waitMinutes));

  set_alarm(waitHours, waitMinutes);
}
prev_setAlarmState = setAlarmState;


stopAlarmState = !digitalRead(stopAlarm);
if (stopAlarmState == false && prev_stopAlarmState == true) {
  noTone(buzzer);
}
prev_stopAlarmState = stopAlarmState;








//print debug info to serial every N ms
if(ms - prevMs_SerialDebug >= msDebugToSerial) {
  
  Serial.print(ms); Serial.print(" ms from boot");  
  
  Serial.print("  ");

  Serial.print(alarmHour);   Serial.print("alarm h"); Serial.print("  ");
  Serial.print(alarmMinute); Serial.print("alarm m");
  
  Serial.print("  ");

  Serial.print(currentHour);   Serial.print("h cur"); Serial.print("  ");
  Serial.print(currentMinute); Serial.print("m cur");
  
  Serial.print("  ");
  
  Serial.print(waitHours);   Serial.print("h wait");  Serial.print("  ");
  Serial.print(waitMinutes); Serial.print("m wait"); 

  Serial.print("   ");

  Serial.print(minutesSinceBoot); Serial.print("m total"); Serial.print("  ");
  Serial.print(totalMinutes);     Serial.print(" m 0am boot day-now"); 
  
  Serial.print("  ");

  Serial.print(cursorAtHours); Serial.print(" h chose");

  Serial.print("  ");

  



  Serial.println(" ");
}

}
