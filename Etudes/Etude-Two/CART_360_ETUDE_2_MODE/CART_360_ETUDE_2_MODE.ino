/**********************ETUDE 3 CART 360 2020*******************************
 * WELCOME! 
 * THE PURPOSE OF THIS EXERCISE IS TO DESIGN A VERY SIMPLE KEYBOARD (4 KEYS)
 * WHICH ALLOWS YOU TO PLAY LIVE, RECORD, AND PLAYBACK SINGLE NOTES (NO CHORDS OR TIMING). 
 * THERE WILL BE A BUTTON WHICH WHEN PRESSED WILL TAKE THE USER TO THE NEXT MODE:
 * THERE ARE 5 POSSIBLE MODES
 * 0 ==  reset    ==   led off == also resets
 * 1 ==  live     ==   led BLUE
 * 2 ==  record   ==   led RED
 * 3 ==  play     ==   led GREEN
 * 4 ==  looper   ==   led PURPLE
 * 
 * ADDITIONALLY - WHEN THE USER CHANGES MODE, 
 * THE RGB LED WILL CHANGE COLOR (ACCORDING TO THE SPECIFICATIONS)

 * PLEASE FOLLOW THE INSTRUCTIONS IN THE COMMENTS:
 * DO NOT ADD ANY MORE FUNCTION DEFINITIONS 
 * ONLY IMPLEMENT THE FUNCTION DEFINITIONS SUPPLIED
 * THERE IS NO NEED TO ADD ANY NEW VARIABLES OR CONSTANTS
 * PROVIDE COMMENTS FOR ANY OF THE CODE IMPLEMENTED
 * GOOD LUCK!
 */
/**** CONSTANTS ********************************************************/

#define BUTTON_MODE_PIN 2 // Button to change the mode

// constants for RGB LED
#define LED_PIN_R 9  // B PIN
#define LED_PIN_G 10 // G PIN
#define LED_PIN_B 11 // R PIN

// constant for note in (button-resistor ladder on breadboard)
# define NOTE_IN_PIN A0

//constant for max number of notes in array
#define MAX_NOTES 16

// duration for a single played note
const int duration = 200;

// constant for pin to output for buzzer 
#define BUZZER_PIN 3 // PWM

/******** VARIABLES *****************************************************/
// counter for how many notes we have
int countNotes = 0;
int mode = 0; // start at off
// array to hold the notes played (for record/play mode)
int notes [MAX_NOTES]; 

/*************************************************************************/


/**********************SETUP() DO NOT CHANGE*******************************/
// Declare pin mode for the single digital input
void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_MODE_PIN, INPUT);
}

/**********************LOOP() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change the loop - it establishes the flow of the program
 * We call here 3 functions repeatedly:
 * 1: chooseMode(): this function will determine the mode that your program is in 
 * based on if the button (linked to the BUTTON_MODE_PIN) was pressed
 * 2: setRGB(): will set the color of the RGB LED based on the value of the mode variable
 * 3: selectMode(): will determine which function to call based on the value of the mode variable

**************************************************************************/
void loop()
{
  chooseMode();
  setRGB();
  selectMode();
}
/******************CHOOSEMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Read the value from the Button (linked to the BUTTON_MODE_PIN) and 
 * if is being pressed then change the mode variable.
 * REMEMBER:
 * mode == 0 is reset 
 * mode == 1 is live
 * mode == 2 is record
 * mode == 3 is play
 * mode == 4 is loopMode
 * Every time the user presses the button, the program will go to the next mode,
 * once it reaches 4, it should go back to mode == 0. 
 * (i.e. if mode ==2 and we press, then mode ==3) ...
**************************************************************************/
void chooseMode(){
  int modeVal = digitalRead(BUTTON_MODE_PIN); //read if the button on pin 2 is being pushed
  if(modeVal == HIGH) { //if the button on pin 2 is being pushed...
    mode = (mode + 1) % 5; //...increment mode and if it overflows, rollover back to 0 using the modulo operator
    while(digitalRead(BUTTON_MODE_PIN) == HIGH){} //this while loop executes indefinitely as long as the button is held. This forced the code to see each discrete button push as one, and only one, press.
  }
}

/******************SETRGB(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Depending on the value of the mode variable:
 * if the mode is 0 - RGB LED IS OFF
 * if the mode is 1 - RGB LED IS BLUE
 * if mode is 2 - RGB LED IS RED
 * if mode is 3 - RGB LED IS GREEN
 * if mode is 4 - RGB LED iS PURPLE
 * YOU MUST USE A SWITCH CASE CONSTRUCT (NOT A SERIES OF IF / ELSE STATEMENTS
**************************************************************************/
void setRGB()
{
  switch(mode) {
    case 1: //if in mode 1, set the LED to blue
      digitalWrite(LED_PIN_R, LOW);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, HIGH);
      break;
    case 2: //if in mode 2, set the LED to red
      digitalWrite(LED_PIN_R, HIGH);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, LOW);
      break;
    case 3: //if in mode 3, set the LED to green
      digitalWrite(LED_PIN_R, LOW);
      digitalWrite(LED_PIN_G, HIGH);
      digitalWrite(LED_PIN_B, LOW);
      break;
    case 4: //if in mode 4, set the LED to purple
      digitalWrite(LED_PIN_R, HIGH);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, HIGH);
      break;
    default: //if none of the above cases are true, turn the LED off
      digitalWrite(LED_PIN_R, LOW);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, LOW);
  }
}
/**********************SELECTMODE() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change this function - it selects WHICH function should run based on the mode variable
 * There are 4 possibilities
 * 1: reset(): this function will reset any arrays etc, and will do nothing else
 * 2: live(): this function will play the corresponding notes 
 * to the user pressing the respective buttons. 
 * NOTE:: the notes played are NOT stored
 * 3: record(): this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * 4: play(): this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * 5: loopMode(): this function will playback any notes stored in the array that were recorded, 
 * BUT unlike the previous mode, you can choose in which sequence the notes are played.
 * REQUIRED: only play notes from the array (no live stuff)

******************************************************************************/
void selectMode()
{
  if(mode == 0) { 
    reset();
  }
  else if(mode == 1) {
    live();
  }
  else if(mode == 2) {
    record();
  }
  
  else if(mode == 3) {
    play();
  }
   
   else if(mode == 4) {
    looper();
  }
}
/******************RESET(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function should ensure that any notes recorded are no longer available
**************************************************************************/
void reset()
{
  for(int i = 0; i < MAX_NOTES; i++) { //for index up to the maximum number of notes...
    notes[i] = 0; //...replace that index in notes[] with a 0
  }
  countNotes = 0; //reset countNotes
}
/******************LIVE(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * NOTE:: the notes played are NOT stored
 * SO: you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * THEN - output the note to the buzzer using the tone() function
**************************************************************************/
void live()
{
    int ladderVal = analogRead(NOTE_IN_PIN); //read the voltage coming from the resistor ladder
    if (ladderVal > 0) { //if the ladder value is non-zero (i.e. some button is being pushed)...
      tone(BUZZER_PIN, ladderVal); //...output a tone of frequency equal to the measured voltage.
      while(analogRead(NOTE_IN_PIN) >0){} //hold in an indefinite loop until the button changes or is released
    } else {
      noTone(BUZZER_PIN); //otherwise stop the current tone. 
    }
}
/******************RECORD(): IMPLEMENT **********************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * SO:you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * AND - output the note to the buzzer using the tone() function
 * THEN store that note in the array  - BE CAREFUL - you can only allow for up to MAX_NOTES to be stored
**************************************************************************/
void record()
{
  int ladderVal = analogRead(NOTE_IN_PIN); //read the voltage coming from the resistor ladder
    if (ladderVal > 0 && countNotes < MAX_NOTES) { //if the ladder value is non-zero (i.e. some button is being pushed) and we haven't hit our maximum number of notes yet...
      tone(BUZZER_PIN, ladderVal); //...output a tone of frequency equal to the measured voltage.
      notes[countNotes] = ladderVal; //put the measured ladderVal note into our array of notes
      countNotes++; //increment the notes counter
      while(analogRead(NOTE_IN_PIN) >0){} //hold in an indefinite loop until the button changes or is released
    } else {
      noTone(BUZZER_PIN); //otherwise stop the current tone. 
    }
}
/******************PLAY(): IMPLEMENT ************************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ each value IN ORDER
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void play()
{
  for(int i = 0; i < countNotes; i++) { //for each note in the notes array...
    chooseMode(); //...first call chooseMode() to give users a chance to exit
    if(mode != 3) { //if the mode is no longer appropriate...
      return; //...exit from the the for loop, which will then exit play, which will then move along because mode has been incremented
    }
    tone(BUZZER_PIN, notes[i], 500); //if the for loop gets this far, takes the next note in the notes array and play it for 500ms
    delay(500); //wait for 500ms while the note plays before moving to the next note
  }
  delay(500); //pause for half a second between loops
}
/******************LOOPMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ values IN ANY ORDERING (You MUST use the array - but you can determine your own sequence)
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void looper() //this method will loop through the recorded notes, but in the reverse order they were recorded
{
  for(int i = (countNotes - 1); i > -1; i--) { //for each note in the notes array (but backwards)...
    chooseMode(); //...first call chooseMode() to give users a chance to exit
    if(mode != 4) { //if the mode is no longer appropriate...
      return; //...exit from the the for loop, which will then exit play, which will then move along because mode has been incremented
    }
    tone(BUZZER_PIN, notes[i], 500); //if the for loop gets this far, takes the next note in the notes array and play it for 500ms
    delay(500); //wait for 500ms while the note plays before moving to the next note
  }
  delay(500); //pause for half a second between loops
}

/**************************************************************************/



/******************ANSWER TO PART TWO OF ETUDE TWO *********************************
 * In the resistor ladder provided, the buttons allow for a closed circuit to be formed at different “levels” of the ladder, 
 * passing the current through a variable number of resistors. For instance, pushing the button closest to wire leading to 
 * the Arduino circumvents all 10KΩ resistors of the ladder, while pushing the next button over passes the current through 
 * one resistor, then the next through two resistors, and so on. When pushing two buttons, the one passing the current through 
 * the fewest resistors prevails as this effectively creates a parallel circuit where one branch of the parallel loop contains 
 * no resistors. Since the rule for calculating the total resistance of two resistors in parallel is:
 * 
 *  (1)     1/R_Total = 1/R_1 + 1/R_2
 *  
 * Or, written another way:
 * 
 *  (2)     (R_1*R_2)/(R_1 + R_2) = R_Total
 * 
 * Then if one of the branches has effectively zero resistance, say R2, then the entire left side of the equation goes to zero, 
 * and thus the effective resistance of two buttons in our ladder being pushed is only as resistive as those resistors that 
 * come after the parallel branching (i.e. after the button closest to the wire running to the Arduino). 
 * 
 * Bearing this all in mind, since the wire running to the analog pin of the Arduino has a pull-down resistor, voltage at the 
 * pin is clamped to zero until a a circuit is closed to 5V via pushing one of the buttons. The voltage supplied to the pin 
 * then becomes proportional to how many resistors the current must pass through, in accordance with Ohm’s law and the discussion 
 * above. The voltage on the analog pin is then converted to a digital signal with integer value between 0 and 1023, which within 
 * the code is passed to the tone() function. The tone() function interprets the number as a frequency and sends a pulsed digital 
 * signal from pin 3 with a duty cycle corresponding to the given frequency. This signal then reaches the buzzer, where the width 
 * of the pulses causes the diaphragm of the buzzer to vibrate at the specified frequency, creating an audible tone. 
 * 
 * The mode button behaves quite simply as a digital pin with a pull-down resistor clamping it to zero. When the mode button is 
 * pushed, a circuit is closed to 5V and the digital pin reads HIGH, allowing us to detect a signal. 
 */
