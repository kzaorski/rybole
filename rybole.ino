#include <Time.h>
#include <TimeLib.h>

#include <DS1302RTC.h>

#include <Wire.h>

const int kCePin   = 3;  // Chip Enable
const int kIoPin   = 4;  // Input/Output
const int kSclkPin = 2;  // Serial Clock

const int filtr = 7; //5
const int fpow = 8; //4
const int plant = 9; //3
const int sunny = 6; //2
const int button = 5;

boolean stanFiltr = 0;
boolean stanFP = 0;
boolean stanPlant = 0;
boolean stanSunny = 0;
boolean zarlo = false;
boolean buttonDown = false;

DS1302RTC RTC(kCePin, kIoPin, kSclkPin);

unsigned long currentTime = 0;
unsigned long rememberedTime = 0;
unsigned long rememberedTimeFeed = 0;
unsigned long feedStart = 0;
unsigned long timeButtonDown = 0;
unsigned long timeButtonUp = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Wire.begin();
  setSyncProvider(RTC.get);

  pinMode(filtr, OUTPUT);
  pinMode(fpow, OUTPUT);
  pinMode(plant, OUTPUT);
  pinMode(sunny, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  //---------------------------------------------
  digitalWrite(filtr, HIGH); // 5
  //digitalWrite(fpow, LOW); // 4
  //digitalWrite(plant, HIGH); // 3
  //digitalWrite(sunny, LOW); // 2
  // 1 - zasilanie stałe
  //---------------------------------------------
}
void loop() {
  currentTime = millis();
  if (currentTime - rememberedTime >= 100UL) { //delay 1 sec
    rememberedTime = currentTime;
    // feed pause
    if ((digitalRead(button) == LOW) && (buttonDown == false)) {
      timeButtonDown = currentTime;
      buttonDown = true;
    }

    if ((digitalRead(button) == HIGH) && (buttonDown == true)) {
      timeButtonUp = currentTime;
      buttonDown = false;
      Serial.println(timeButtonUp - timeButtonDown);
    }
    // button down between 5...10 sec
    if (!stanPlant && (timeButtonUp - timeButtonDown >= 5000UL) && (timeButtonUp - timeButtonDown < 10000UL)) {
      plant_on();
      timeButtonUp = 0;
      timeButtonDown = 0;
    }

    // button down between 0,5...3 sec
    if (!zarlo && (timeButtonUp - timeButtonDown >= 500UL) && (timeButtonUp - timeButtonDown < 3000UL)) {
      Serial.println("ZARLO");
      fpow_off();
      filtr_off();
      feedStart = currentTime;
      zarlo = true;
      timeButtonUp = 0;
      timeButtonDown = 0;
    }

    // button down between 10...15 sec
    if ((stanPlant && timeButtonUp - timeButtonDown >= 10000UL) && (timeButtonUp - timeButtonDown < 15000UL)) {
      plant_off();
      timeButtonUp = 0;
      timeButtonDown = 0;
    }

    if ((stanFiltr == 0) && (currentTime - feedStart >= 300000UL)) {
      filtr_on();
    }

    // start testowy

    // czas(19, 0, 0, sunny_on);
    // czas(13, 30, 0, plant_on);

    if ((!stanPlant)) {
      czas(12, 00, 0, plant_on);
      czas(15, 00, 0, plant_on);
      czas(16, 30, 0, plant_on);
      czas(19, 30, 0, plant_on);
    }
    if  (stanPlant) {
      czas(14, 30, 0, plant_off);
      czas(16, 00, 0, plant_off);
      czas(18, 30, 0, plant_off);
      czas(20, 30, 0, plant_off);
    }
    if (!stanSunny) {
      czas(13, 30, 0, sunny_on);
      czas(16, 00, 0, sunny_on);
      czas(18, 30, 0, sunny_on);
      czas(20, 00, 0, sunny_on);
    }
    if (stanSunny) {
      czas(15, 00, 0, sunny_off);
      czas(17, 30, 0, sunny_off);
      czas(19, 30, 0, sunny_off);
    }

    czas(21, 00, 0, dobranoc);

    // filtr powierzchniowy co godzine na 10 min
    ffpow();
  }
}

void ffpow() {
  if (minute() == 0 && second() == 00) {
    fpow_on();
  }
  if (minute() == 10 && second() == 00) {
    fpow_off();
  }
}
void czas (const int h, const int m, const int s, void f() ) {
  if ((hour() == h) && minute() == m) { //&& second() == s) {
    f();
  }
}
void dobranoc() {
  Serial.println("Dobranoc");
  plant_off();
  sunny_off();
}
void sunny_on() {
  Serial.println("sunny on");
  stanSunny = 1;
  digitalWrite(sunny, HIGH);
} void sunny_off() {
  Serial.println("sunny off");
  stanSunny = 0;
  digitalWrite(sunny, LOW);
}
void plant_on() {
  Serial.println("plant on");
  stanPlant = 1;
  digitalWrite(plant, HIGH);
}
void plant_off() {
  Serial.println("plant off");
  stanPlant = 0;
  digitalWrite(plant, LOW);
}
void fpow_on () {
  Serial.println("fpow on");
  stanFP = 1;
  digitalWrite(fpow, HIGH);
}
void fpow_off () {
  Serial.println("fpow off");
  stanFP = 0;
  digitalWrite(fpow, LOW);
}
void filtr_on () {
  Serial.println("filtr on");
  stanFiltr = 1;
  digitalWrite(filtr, HIGH);
} void filtr_off () {
  Serial.println("filtr off");
  stanFiltr = 0;
  digitalWrite(filtr, LOW);
}
