  #include <APA102.h>

// Define which pins to use.
const uint8_t dataPin = 9;
const uint8_t clockPin = 6;

// Create an object for writing to the LED strip.
APA102<dataPin, clockPin> ledStrip;

// Set the brightness to use (the maximum is 31).
const uint8_t brightness = 1;

const uint16_t ledCount = 5;

//potentiometer
const int potPin = A4;
int potValue = 0;
int val;

//button
const int buttonPin = 13; 
int buttonState = LOW;

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

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin (9600);
}

void loop() {
  currentTime = millis();
  
  if (timerStart == 0) {
    buttonState = LOW;
    potValue = analogRead(potPin);
    val = map(potValue, 0, 1023, 0, ledCount+1);

    Serial.println(val);

  ledStrip.startFrame();
  for(uint16_t i = 0; i < ledCount; i++)
  {
    if (i < val) {
        ledStrip.sendColor(255, 255, 255, 1);
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

  if (timerStart > 0 && millis()-timerStart >= 1000) {
    timerStart += 1000;
    startNumber--;

    ledStrip.startFrame();
    for(uint16_t i = 0; i < ledCount; i++)
    {
      if (i < startNumber) {
        ledStrip.sendColor(255, 255, 255, 1);
      }
      else {
        ledStrip.sendColor(0, 0, 0, 0);
      }
    }
    ledStrip.endFrame(ledCount);

    if (startNumber < endNumber) {
      startNumber--;
      while (buttonState == LOW) {
        buttonState = digitalRead(buttonPin);
      }
      while (buttonState == HIGH) {
        buttonState = digitalRead(buttonPin);
      }
      timerStart = 0;
      buttonState = LOW;

    }
  }

  
}
