
//define I/O pins
const int motorOnePin = 9; 
const int motorTwoPin = 10;
const int arousalPin = A0;
const int valencePin = A1;

//variables to store analog input
float arousal;
float valence;

//variables for later calculating how long air pumps should be on or off
int onTime = 555;
float varTime = 0;
int offTime = 555;

//variable for calculating the phase shift of the second pump
float phaseShift = 0;

//counters for the number of loops that have taken place, for creating cyclic behaviour
int frameCounter = 0;
int adjustedCounter = 0;


void setup()
{
  pinMode(motorOnePin, OUTPUT); 
  pinMode(motorTwoPin, OUTPUT);
  Serial.begin(9600); 
}


void loop()
{ 
  frameCounter = (frameCounter + 7) % 5555; //calculate the frame of the cycle

  //get analog input
  arousal = analogRead(arousalPin);
  valence = analogRead(valencePin);

  //calculate length of ON state and phase shift of the second pump
  varTime = 4444.0 * (arousal / 1023.0);
  phaseShift = 2777.0 * (valence / 1023.0);

  //calculate the full on time with the associated base on length
  onTime = 555 + varTime;

  //if the current frame is during the ON portion of the cycle, output HIGH to the pump, otherwise output LOW
  if (frameCounter < onTime) {
    analogWrite(motorOnePin, 255);
  } else {
    analogWrite(motorOnePin, 0);
  }

  //calculate the phase shifted frame value for the second pump
  adjustedCounter = (frameCounter + (int)phaseShift) % 5555;

  //if the current frame is during the ON portion of the cycle, output HIGH to the pump, otherwise output LOW
  if (adjustedCounter < onTime) {
    analogWrite(motorTwoPin, 255);
  } else {
    analogWrite(motorTwoPin, 0);
  }
}
