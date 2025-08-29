// Wellness Lamp Code Complete

int mode = 0; // mode of the wellness lamp 0=off, 1=low, 2=med, 3=high, 4=manual
unsigned long startMillis; // Stores the start time of the current mode
unsigned long duration = 0; // Stores how long the light should be on for the current mode
const unsigned long minuteMS = 60*1000; // 1 minute = 60 seconds * 1000 ms

// tell the Arduino which pins are doing what
void setup()
{
  Serial.begin(9600); // Initialize Serial communication at 9600 BPS
  pinMode(A1, INPUT); // Potentiometer input
  pinMode(12, INPUT_PULLUP); // button to change mode
  pinMode(7, OUTPUT); // indicator light 1
  pinMode(4, OUTPUT); // indicator light 2
  pinMode(2, OUTPUT); // indicator light 3
  pinMode(5, OUTPUT); // wellness LED 1 NMOS signal
  pinMode(6, OUTPUT); // wellness LED 2 NMOS signal
  pinMode(9, OUTPUT); // wellness LED 3 NMOS signal
  pinMode(10, OUTPUT); // wellness LED 4 NMOS signal
  pinMode(11, OUTPUT);// wellness LED 5 NMOS signal
  pinMode(13, OUTPUT); // indicator light 4 (other colour)
   
  startMillis = millis(); // Initialize Start Time
}

// Main loop
void loop()
{
  	// check if the user has hit the button to change the mode
	int buttonState = digitalRead(12); // read if the button is pressed
    if(buttonState==0) // button is pressed
	{
      	//Iterates through the available modes
    	mode=mode+1; // increment mode
    	mode%=5; // limit it to range 0-4
    	Serial.print("Mode is: "); Serial.println(mode); // debug output
        // Reset the timer given the mode change
        startMillis = millis(); // Initialize Start Time for current mode
      
		// mode change so update light settings
		switch(mode) {
          case 0: // mode 0 - no lights, indicators off
            setLights(LOW, LOW, LOW, LOW, 0, 0, 0, 0, 0);
            duration = 0; // no limits if lights off
            break;
          case 1: // mode 1 - low intensity wellness brightness, 1 indicator, long duration (60 min)
            setLights(HIGH, LOW, LOW, LOW, 100, 100, 100, 100, 100); 
            duration = 60*minuteMS; // long duration at low light level
            break;
          case 2: // mode 2 - medium, 2 indicators, med duration (45 min)
          	setLights(HIGH, HIGH, LOW, LOW, 170, 170, 170, 170, 170);
          	duration = 45*minuteMS; // medium duration at med light level
          	break;
          case 3: // mode 3 - high, 3 indicators, short duration (30min)
          	setLights(HIGH, HIGH, HIGH, LOW, 255, 255, 255, 255, 255);
          	duration = 30*minuteMS; // low duration at high light level
          	break;
          // case 4: handled outside the button pressed if statement
          default: // no other known cases so do nothing
          break;
        }
    }
  
  	// button pressed or not (e.g. buttonState==0 or 1)
  	// need to check every cycle if in mode 4 to allow continuous adjustment of brightness
    if (mode==4) { // set manual indicator and manual brightness setting
      // get power setting from potentiometer 
      int powerSet = analogRead(A1); 
      Serial.print("powerSet value: "); Serial.println(powerSet);
      //map potentiometer data range to MOSFET gate range (0-1023 to 0-255)
      int mappedVal = map(powerSet, 0, 1023, 0, 255); 
      Serial.print("mappedvalue is: "); Serial.println(mappedVal); 
      //set lights with corrected level of dimming
      setLights(LOW, LOW, LOW, HIGH, mappedVal, mappedVal, mappedVal, mappedVal, mappedVal);
  	}

  // check if time is up for any of the modes 1 through 3
  unsigned long currentMillis = millis(); //Get the current time
  // Calculate the elapsed time since last mode setting
  unsigned long elapsedMillis = currentMillis - startMillis;
  // turn off if passes duration and not in mode 0 or 4
  if (elapsedMillis >= duration && mode>0 && mode<4)  {
    setLights(LOW, LOW, LOW, LOW, 0, 0, 0, 0, 0);
    duration = 0; // reset duration
    mode = 0; // reset mode to 0 (or off)
  }

  // if the button was just pressed delay 250 milliseconds to allow user to lift finger
  // before reading button again
  if (buttonState==0){ 
    delay(250);
  }
}

// Function to simplify setting the lights (wellness and indicators)
// Currently implemented with individual control of the wellness leds. Could be changed
// to pass one wellness brightness setting to all wellness LEDs
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
               
