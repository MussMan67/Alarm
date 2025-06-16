/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>

 ***************************************************
 This example shows the basic function of library for DFPlayer.

 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <string.h>

#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/10, /*tx =*/11);
#define FPSerial softSerial

DFRobotDFPlayerMini myDFPlayer;

const int stopPin = 3;
const int skipPin = 4;
const int holdPin = 5;
const int dialPin = A1;
const int voluPin = A0;
const int rumbPin; // TO-DO
const int LEDPin; // TO-DO

uint32_t wakeUpTime = 15000; //24 * 60 * 60 * 1000;
bool alarmStopped = true;

uint32_t initialSoundTime = 0;
uint32_t initialRumbleTime = 0;
uint32_t currentTime = 0;

bool isTimeToWakeUp;
uint8_t numRepeat = 15;

const uint32_t sleepCycleTime = 120 * 1000;

void setup()
{
  pinMode(stopPin, INPUT);
  pinMode(skipPin, INPUT);
  pinMode(holdPin, INPUT);
  pinMode(dialPin, INPUT);
  pinMode(voluPin, INPUT);
  pinMode(rumbPin, INPUT);
  FPSerial.begin(9600);
  Serial.begin(115200);
  myDFPlayer.begin(FPSerial, true, true);
  changeVolume();
  delay(3); // wait for all serial comms to initialize properly
  myDFPlayer.play(1);
}

void loop() {
  // if (isWakeUpTime() || !alarmStopped) { wakeUp(); }
  // Serial.println(String((uint32_t) millis()) + "        " + String(wakeUpTime) + "    " + String(volumeValue()));
  // // handleAlarmAdjustment();
  // handleSkipCycle();
  // // handleRumble();
}

bool isWakeUpTime() {
  if ((uint32_t) millis() >= wakeUpTime) {
    alarmStopped = false;
    myDFPlayer.play(1);
    wakeUpTime += sleepCycleTime;
    initialSoundTime = millis();
    return true;
  }
  return false;
}

void wakeUp() {
  if (digitalRead(stopPin)) {
    alarmStopped = true;
    myDFPlayer.pause();
    return;
  }
  if (!alarmStopped) {
    currentTime = millis();
    if (currentTime - initialSoundTime > numRepeat * 60 * 1000) {
      initialSoundTime = millis();
      initialRumbleTime = millis();
      myDFPlayer.play(1);
      rumble(numRepeat);
      numRepeat -= 5;
    }
    // stop repeating after 4 times
    if (numRepeat == 0) {
      numRepeat = 15;
      alarmStopped = true;
    }
  }
}

void handleAlarmAdjustment() {
  // when the change alarm-adjustment button is on, store the new desired wake up time and change the volume
  while (digitalRead(holdPin)) {
    changeTimer();
    changeVolume();
  }
}

void handleSkipCycle() {
  while (digitalRead(skipPin)) {
    while (digitalRead(skipPin)) {}
    wakeUpTime += sleepCycleTime;
  }
}

void changeTimer() {
  wakeUpTime = millis() + timerValue();
}

uint32_t timerValue() {
  return ((float) analogRead(dialPin) / 1024) * 12 * 60 * 60 * 1000; // converting the potentiometer value from [0, 1023] to ms, 
                                                                     // where the potentiometer selects from a range of 12 hours
}

void changeVolume() {
  myDFPlayer.volume(volumeValue());
}

uint8_t volumeValue() {
  return (int) (((float) analogRead(voluPin) / 1023) * 30);
}

void handleRumble() {
  if (currentTime - initialRumbleTime >= 3 * 1000) {
     rumble(15);
  }
}

void rumble(int n) {
  analogWrite(rumbPin, (float) ((15 - n) / 15) * 255);
}