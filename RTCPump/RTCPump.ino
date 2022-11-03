//Programmer: Irak Mayer
//Original Date: 11/02/2022
//License: MIT.

#include "RTClib.h"

//Real time object instance
RTC_DS3231 rtc;

const int relayPin = 11;

//Timer settings.
//Start 10 secs after
TimeSpan delayCycle = TimeSpan(0,0,0,10);
//Start 15 mins and 10 secs after
TimeSpan runCycle = TimeSpan(0,0,15,10);

//Function to initialize the timer sensor, and clear alarms
void initRTC(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.disable32K();

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

}

//Initialize peripherials and start timers
void setup () {
  Serial.begin(57600);

  initRTC();

  pinMode(relayPin, OUTPUT);

  if(!rtc.setAlarm1(
          rtc.now() + delayCycle,
          DS3231_A1_Minute // this mode triggers the alarm when the minute match, once every hour
  )) {
      Serial.println("Error, alarm wasn't set!");
  }else {
      Serial.println("Alarm will happen in 10 seconds!");
  }

  if(!rtc.setAlarm2(
          rtc.now() + runCycle,
          DS3231_A2_Minute //this mode triggers the alarm when the minute match, once every hour.
  )) {
      Serial.println("Error, alarm wasn't set!");
  }else {
      Serial.println("Alarm will happen in 10 seconds!");
  }

}

//main loop
void loop () {
    // if the first alarm fires, then trigger the relay.
    if (rtc.alarmFired(1)) {
        rtc.clearAlarm(1);
        Serial.print(" - Alarm cleared");
        digitalWrite(relayPin, HIGH);
    }

    // 15 mins later trigger the relay off, with the second alarm
    if (rtc.alarmFired(2)) {
        rtc.clearAlarm(2);
        Serial.print(" - Alarm cleared");
        digitalWrite(relayPin, LOW);
    }

    delay(3000);
}
