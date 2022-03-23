#include <TimerFreeTone.h>
#include <Servo.h> //Timer 1
#include "NewPing.h" //Timer 2
#include <APA102.h>

/* PINS!!
 *  LED clock (yellow): 6
 *  Ultrasonic echo (yellow): 7
 *  Ultrasonic trig (green): 8
 *  LED data (green):9
 *  Button: 13
 *  
 *  Servo: A0 
 *  Buzzer: A3
 *  Potentiometer: A4
 */

// LED strip
const uint8_t dataPin = 9;
const uint8_t clockPin = 6;
APA102<dataPin, clockPin> ledStrip;

// Set the number of LEDs to control.
const uint16_t ledCount = 75; //ADJUST ACCORDINGLY
// Create a buffer for holding the colors (3 bytes per color).
rgb_color colors[ledCount];
// Set the brightness to use (the maximum is 31).
const uint8_t brightness = 5; //ADJUST ACCORDINGLY!!

Servo myservo;

//buzzer
#define BUZ_PIN A3

//potentiometer
const int potPin = A4;
int potValue = 0;
int val;

//button
const int buttonPin = 13; 
int buttonState = LOW;

// ultrasonic sensor
const int TRIG_PIN = 8; // Arduino pin connected to Ultrasonic Sensor's TRIG pin
const int ECHO_PIN = 7; // Arduino pin connected to Ultrasonic Sensor's ECHO pin
#define MAX_DISTANCE 400 
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

//defining other variables
float displayTimeSecs = 5; //how long do you want each number on display to show (in secs)
float displayTime = (displayTimeSecs * 5000);
float pausedTime = 0;
long startNumber = 0; //countdown starts with this number
long endNumber = 0; //countdown ends with this number
float duration_1, distance_1, duration_2, distance_2;
static unsigned long currentTime;
long time1, time2 = 0;
static long timerStart = 0;

const int elev_position = 90;
const int low_position = 0;
const int volume = 10; // max is 10

void setup(){
    pinMode(buttonPin, INPUT);
    myservo.attach(A0);
    myservo.write(elev_position);
    Serial.begin (9600);
}

void loop() {
  
  currentTime = millis();

  duration_1 = sonar.ping();
  distance_1 = (duration_1 / 2) * 0.0343;

  if (5 < distance_1 and distance_1 < 15) { //senses hand above
    time1 = currentTime;
  }
  
  else{
    time1 = 0;
  }

  while (time1 > 0) { //once hand is detected
    if (millis()-time1 >= 100) { //(CHANGE FOR DIFF. TIME BTWN DISTANCE MEASUREMENTS)
      duration_2 = sonar.ping();
      distance_2 = (duration_2 / 2) * 0.0343;

      Serial.println("DIST DIFF: ");
      Serial.print(distance_1-distance_2);
      Serial.println("DIST 2: ");
      Serial.print(distance_2);
      Serial.println("__________________________");

      if ((distance_1 - distance_2) > 5 && abs(distance_1 - distance_2) < 50 && distance_2 != 0 && distance_2 < 40) {
        timerStart = -1;
        pausedTime = startNumber;
        time1 = 0;
        goto alarm;
      }
      time1 = 0;
    }
  }

  if (timerStart == 0) {
    buttonState = LOW;
//    currentTime = millis();
    potValue = analogRead(potPin);
    val = map(potValue, 0, 1023, 0, ledCount+1);
    
    ledStrip.startFrame();
    for(uint16_t i = 0; i < ledCount; i++)
    {
      if (i < val) {
          ledStrip.sendColor(255, 255, 255, brightness);
        }
        else {
          ledStrip.sendColor(0, 0, 0, 0);
        }
    }
    ledStrip.endFrame(ledCount);

    
    buttonState = digitalRead(buttonPin);

    if (buttonState == HIGH) {
        startNumber = val;
        timerStart = currentTime;
        buttonState = LOW;
      }
  }

  
//  for(uint16_t i = 0; i < ledCount; i++)
//  {
//    //white (honeydew)
//    colors[i].red = 240;
//    colors[i].green = 255;
//    colors[i].blue = 240;
//  }
//  ledStrip.write(colors, ledCount, 1);
  

  if (timerStart > 0 && millis()-timerStart >= 1000) {
    timerStart += 1000;
    startNumber--;

    ledStrip.startFrame();
    for(uint16_t i = 0; i < ledCount; i++)
    {
      if (i < startNumber) {
        ledStrip.sendColor(255, 255, 255, brightness);
      }
      else {
        ledStrip.sendColor(0, 0, 0, 0);
      }
    }
    ledStrip.endFrame(ledCount);

    if (startNumber < endNumber && timerStart != 0) { //end of timer
      
      startNumber--;
      ledStrip.startFrame();
      for(uint16_t i = 0; i < ledCount; i++)
      {
        ledStrip.sendColor(255, 255, 0, brightness);
      }
      ledStrip.endFrame(ledCount);
      
      while (buttonState == LOW) {

        currentTime = millis();
        duration_1 = sonar.ping();
        distance_1 = (duration_1 / 2) * 0.0343;
      
        if (5 < distance_1 and distance_1 < 15) { //senses hand above
          time1 = currentTime;
        }
        
        else{
          time1 = 0;
        }
        TimerFreeTone(BUZ_PIN,1000,80,volume);
        while (time1 > 0) { //once hand is detected
          if (millis()-time1 >= 1) { //(CHANGE FOR DIFF. TIME BTWN DISTANCE MEASUREMENTS)
            duration_2 = sonar.ping();
            distance_2 = (duration_2 / 2) * 0.0343;
      
            if ((distance_1 - distance_2) > 5 && abs(distance_1 - distance_2) < 50 && distance_2 != 0 && distance_2 < 40) {
              Serial.println("TEST WORKING?");
              timerStart = -1;
              pausedTime = 0;
              time1 = 0;
              goto alarm;
//              break;
            }
            time1 = 0;
          }
        }
        
        buttonState = digitalRead(buttonPin);
      }
      
      while (buttonState == HIGH) {
        buttonState = digitalRead(buttonPin);
        timerStart = 0;
      }
      buttonState = LOW;

    }
  }

  alarm:
    if (timerStart < 0) {
      buttonState = digitalRead(buttonPin);
      myservo.write(low_position);
  
      while (buttonState == LOW) {
        TimerFreeTone(BUZ_PIN,5000,50,volume);
        ledStrip.startFrame();
        for(uint16_t i = 0; i < ledCount; i++)
        {
          ledStrip.sendColor(255, 0, 0, brightness);
        }
        ledStrip.endFrame(ledCount);
        
        buttonState = digitalRead(buttonPin);
      }
      while (buttonState == HIGH) {
        buttonState = digitalRead(buttonPin);
      }
      if (pausedTime != 0) {
        startNumber = pausedTime;
        timerStart = currentTime;
      }
      else {
        timerStart = 0;
      }
  
      myservo.write(elev_position);
      time1 = 0;
      buttonState = LOW;
    }
}
