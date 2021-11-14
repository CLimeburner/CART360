#define PIN_LED_BUTTON_1  3   
#define PIN_LED_BUTTON_2  5   
#define PIN_LED_BUTTON_3  9 
#define PIN_LED_BUTTON_4  10  
#define PIN_PIEZO         11  

#define LED_BUTTON_COUNT  4   // Number of LEDs & buttons

static int buttonLookup[LED_BUTTON_COUNT] = { PIN_LED_BUTTON_1, PIN_LED_BUTTON_2, PIN_LED_BUTTON_3, PIN_LED_BUTTON_4 }; //array of button IDs
static long gameSeed;





// Returns the period in microseconds of the half-cycle tone
// for each button.
int ledButtonHalfPeriod(int button) {  //generates game pitches
  switch (button) {
    case PIN_LED_BUTTON_1:
      return 1911;
    case PIN_LED_BUTTON_2:
      return 1703;
    case PIN_LED_BUTTON_3:
      return 1517;
    case PIN_LED_BUTTON_4:
      return 1432;
  }
  return 3822;
}





bool checkButtonPush(int button) {
  bool result = false; //create a bool that's false by default
  pinMode(button, INPUT);   //set pin to input
  digitalWrite(button, HIGH); //since pin is set to input, this enables the internal pullup resistor
  
  int halfPeriod = ledButtonHalfPeriod(button); //get the corresponding pitch for the button
  
  while (digitalRead(button) == LOW) { //if the pin is low, then the intenral pullup is being pulled low to ground, so button is beign pushed
    // Button is being pushed
    result = true; //set bool to true
    digitalWrite(PIN_PIEZO, HIGH); //flicker sound manually
    delayMicroseconds(halfPeriod);
    digitalWrite(PIN_PIEZO, LOW);
    delayMicroseconds(halfPeriod);
  }
  return result; //return the result
}





//function checks each button pin to see if the button's being pushed, if so it returns the pin's index, otherwise returns -1
int getButtonPush(void) {
  for (int i = 0; i < LED_BUTTON_COUNT; ++i) {
    if (checkButtonPush(buttonLookup[i])) {
      return i;
    }
  }
  return -1;
}





void displayLightAndSound(int led, int duration) {
  if (led >= 0) {
    pinMode(led, OUTPUT);   // Set led cathode pin as output
    digitalWrite(led, LOW); //write the pin low, effectively making it a ground
  }
  
  int halfPeriod = ledButtonHalfPeriod(led); //get corresponding pitch
  long cycleCount = (long)duration * 500;
  cycleCount /= halfPeriod;
  for (; cycleCount > 0; --cycleCount) {
    digitalWrite(PIN_PIEZO, HIGH); //flicker sound manually
    delayMicroseconds(halfPeriod);
    digitalWrite(PIN_PIEZO, LOW);
    delayMicroseconds(halfPeriod);
  }
  
  if (led >= 0) { 
    pinMode(led, INPUT);      // Set led cathode pin as input
    digitalWrite(led, HIGH);  //since the pin is in input mode, this enables the internal pullup resistor
  }
}





void startUpLightsAndSound(void) {
  // Power-up lights and sound
  displayLightAndSound(PIN_LED_BUTTON_1, 100); //sweep through the LEDs
  displayLightAndSound(PIN_LED_BUTTON_2, 100);
  displayLightAndSound(PIN_LED_BUTTON_3, 100);
  displayLightAndSound(PIN_LED_BUTTON_4, 100);

  // Wait for button push to continue
  while (getButtonPush() < 0) { 
    delay(100);
  }
  
  // Delay a bit before starting
  delay(500);
}






// the setup routine runs:
void setup() {
  // Set button pins as input
  pinMode(PIN_LED_BUTTON_1, INPUT);
  pinMode(PIN_LED_BUTTON_2, INPUT);
  pinMode(PIN_LED_BUTTON_3, INPUT);
  pinMode(PIN_LED_BUTTON_4, INPUT);
  
  // Set the piezo pin as output
  pinMode(PIN_PIEZO, OUTPUT);

  // Power-up lights and sound
  startUpLightsAndSound();

  // Randomize the first game
  gameSeed = micros();
}





// the loop routine runs over and over again forever:
void loop() {
  static int sequenceLength = 1; //start off with a sequence of length 1, static, so only intialized once, hence does not overwrite on each loop
  int i;
  
  // Display current sequence
  randomSeed(gameSeed); //initialize the pseudo-random sequence
  for (i = sequenceLength; i > 0; --i) { //count down, but the counting doesn't matter much because the psuedo-random process sequences itself
    displayLightAndSound(buttonLookup[random(LED_BUTTON_COUNT)], 250);
    delay(250);
  }
  
  randomSeed(gameSeed); //initialize the pseudo-random sequence again, so the sequence matches the above
  for (int matches = 0; matches < sequenceLength; ++matches) { //notably matches increments on it's own, so it's more about not getting it wrong, than strictly getting it right
    int button = -1;
    int timeout;
    // Wait for the player to press a button (with 3 second timeout)
    for (button = -1, timeout = 30; timeout && button < 0; --timeout) { //loops until you get a button or timeout
      button = getButtonPush();
      delay(100);
    }
  
    // Check if the correct button was pushed
    if (button != random(LED_BUTTON_COUNT)) { //checks aginst the pseudorandom sequence
      // Play sad sound and start a new game
      displayLightAndSound(-1, 1000);
      gameSeed = micros();
      delay(1000);
      sequenceLength = 0;
      startUpLightsAndSound();
    }
  }
  ++sequenceLength;
  delay(1000);
}
