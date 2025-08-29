#include <Adafruit_LiquidCrystal.h>
Adafruit_LiquidCrystal lcd(0); // MCP23008 at address 0x20

int mode = 0; // mode of the wellness lamp 0=off, 1=low, 2=med, 3=high, 4=manual
unsigned long startMillis; // Stores the start time of the current mode
unsigned long duration = 0; // Duration for each mode
const unsigned int minuteMS = 60 * 1000;
bool timerRunning = false; // Flag to indicate if the timer is running
unsigned long previousMillisLCD = 0; // Tracks time for LCD updates
const long interval = 1000; // 1-second interval for timer updates
int remainingSeconds = 0; // Remaining time in seconds

void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT); // Potentiometer input
  pinMode(12, INPUT); // button to change mode
  pinMode(7, OUTPUT); 
  pinMode(4, OUTPUT); 
  pinMode(2, OUTPUT); 
  pinMode(5, OUTPUT); // wellness LED 1 NMOS signal
  pinMode(6, OUTPUT); // wellness LED 2 NMOS signal
  pinMode(9, OUTPUT); // wellness LED 3 NMOS signal
  pinMode(10, OUTPUT); // wellness LED 4 NMOS signal
  pinMode(11, OUTPUT); // wellness LED 5 NMOS signal
  pinMode(13, OUTPUT);

  startMillis = millis(); // Initialize Start Time

  // Initialize the LCD (16x2)
  lcd.begin(16, 2); 
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.print("Push To Start");
}

void loop() {
  int buttonState = digitalRead(12); // Read if the button is pressed

  if (buttonState == 1) {
    // Increment the mode and handle the wrap-around
    mode = (mode + 1) % 5;
    Serial.print("Mode is: ");
    Serial.println(mode);

    // Reset the timer
    startMillis = millis();
    timerRunning = false; // Stop the timer if mode changes

    unsigned long currentMillis = millis();

    switch (mode) {
      case 0: // Mode 0 - no lights, indicators off
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Push To Start");
        setLights(LOW, LOW, LOW, LOW, 0, 0, 0, 0, 0);
        duration = 0;

        delay(2000); // Show message for 2 seconds
        break;

      case 1: // Mode 1 - low intensity, long duration (60 min)
        lcd.clear();
        setLights(HIGH, LOW, LOW, LOW, 85, 85, 85, 85, 85);
        lcd.print("Low Intensity");
      	lcd.setCursor(0,1);
      	lcd.print("Lux : 5000");
        delay(1000);
        duration = 1 * minuteMS;
        startTimer(1);
        break;

      case 2: // Mode 2 - medium intensity, medium duration (45 min)
        lcd.clear();
        setLights(HIGH, HIGH, LOW, LOW, 170, 170, 170, 170, 170);
        lcd.print("Mid Intensity");
      	lcd.setCursor(0,1);
      	lcd.print("Lux : 7500");
        delay(1000);
        duration = 45 * minuteMS; // 45 minutes
        startTimer(45);
        break;

      case 3: // Mode 3 - high intensity, short duration (30 min)
        lcd.clear();
        setLights(HIGH, HIGH, HIGH, LOW, 255, 255, 255, 255, 255);
        lcd.print("High Intensity");
      	lcd.setCursor(0,1);
      	lcd.print("Lux : 10000");
        delay(1000);
        duration = 30 * minuteMS; // 30 minutes
        startTimer(30);
        break;

      case 4: // Mode 4 - manual mode
        lcd.clear();
        lcd.print("Manual Mode");
        delay(1500);
        break;
    }

    delay(250); // Debounce delay
  }

  // Handle manual mode brightness adjustment
  if (mode == 4) {
    int powerSet = analogRead(A1); 
    int mappedVal = map(powerSet, 0, 1023, 0, 255);
    setLights(HIGH, HIGH, HIGH, HIGH, mappedVal, mappedVal, mappedVal, mappedVal, mappedVal);
  }

  // Check if the timer is running and update the LCD
  if (timerRunning) {
    if (millis() - previousMillisLCD >= interval) {
      previousMillisLCD = millis();

      if (remainingSeconds > 0) {
        remainingSeconds--; // Decrement remaining time

        // Display the timer on the LCD
        int minutes = remainingSeconds / 60;
        int seconds = remainingSeconds % 60;
        lcd.setCursor(0, 1);
        lcd.print((minutes < 10 ? "0" : ""));
        lcd.print(minutes);
        lcd.print(":");
        lcd.print((seconds < 10 ? "0" : ""));
        lcd.print(seconds);
      } else {
        // Timer has ended, execute case 0 logic
        timerRunning = false;

        // Return to "Push To Start" mode and reset lights
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time's Up!");
        setLights(LOW, LOW, LOW, LOW, 0, 0, 0, 0, 0); // Turn off lights
        delay(2000); // Show message for 2 seconds before returning to Case 0
        mode = 0; // Return to case 0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Push To Start");
      }
    }
  }
}

void startTimer(int minutes) {
  remainingSeconds = minutes * 60; // Convert minutes to seconds
  timerRunning = true; // Start the timer
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer Running");
}

void setLights(bool indicator1, bool indicator2, bool indicator3, bool indicator4, 
               int wellness1, int wellness2, int wellness3, int wellness4, int wellness5) {
  digitalWrite(7, indicator1);
  digitalWrite(4, indicator2);
  digitalWrite(2, indicator3);
  digitalWrite(13, indicator4);
  analogWrite(5, wellness1);
  analogWrite(6, wellness2);
  analogWrite(9, wellness3);
  analogWrite(10, wellness4);
  analogWrite(11, wellness5);
}
