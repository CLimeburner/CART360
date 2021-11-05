
const int pulsePin = A0; //set pin A0 to take input from the pulse oxymeter
const int breathPin = A1; //set pin A1 to take input from the respiratory sensor
const int fiveVPin = 8; //pin 8 will serve as a secondary 5V source

long pulseSignal = 0; //for storing pulse sensor input
long breathSignal = 0; //for storing breath sensor input

const int numberOfSamples = 32; //number of instantaneous samples taken from sensor

const int pulseThreshold = 1000; //experimentally determined cutoff to detect heart beats

long priorPulseSignal = 0; //previous pulse signal sample

unsigned long clockTracker = 0; //current time derived from millis()
unsigned long prevSpike = 0; //time the previous heart beat occured
unsigned long currentSpike = 0; //time the most recent heart beat occured
int pulseInterval = 0; //time between most recent and prior heart beat
float heartRate = 0; //pulseInterval converted to BPM

const int numberOfPulseIntervalSamples = 32; //number of sequential samples of pulse intervals
unsigned long pulseIntervalRecord[numberOfPulseIntervalSamples+1]; //keep track of historical heart rates

const int timeSeriesFrames = 12; //calculate the number of frames for our time series window
int timeSeries[timeSeriesFrames+1]; //an array to store a time series of tidal volume values

int subsampleCounter = 0; //for counting the loop frames for subsampling breathing data

//helper variable for determining the shape of the breath signal
int inhaleCounter = 0;
int inhaleLength = 0;
int exhaleCounter = 0;
int exhaleLength = 0;
int breathMin = 0;
int breathMax = 0;

//metrics of the breath signal
float breathPeriod = 0.00;
float breathRate = 0.00;
float breathDepth = 0.00;

//variable and arrays for smoothing respiratory feature data
int breathFeatureCounter = 0;
float breathRateSeries[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float breathDepthSeries[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float avgBreathRate = 0.0;
float avgBreathDepth = 0.0;

//variables for calculating instantaneous rate of change in biometrics
float prevBreathRate = 0.0;
float prevBreathDepth = 0.0;
float prevHeartRate = 0.0;
float deltaBreathRate = 0.0;
float deltaBreathDepth = 0.0;
float deltaHeartRate = 0.0;

const int delayAmount = 10; //the ms length of sampling frames 


// setup()
//
// initialize the program
//
void setup() {
    Serial.begin(9600); //start serial comms

    pinMode(fiveVPin, OUTPUT); //set pin to output
    digitalWrite(fiveVPin, HIGH); //set pin to act as a constant 5V source

   //initialize pulseIntervalRecord with 0's
   for (int i = 0; i < numberOfPulseIntervalSamples; i++) {
     pulseIntervalRecord[i] = 0;
   }

   //initalize timeSeries with 0's
  for(int i = 0; i < timeSeriesFrames; i++) {
    timeSeries[i] = 0;
  }
}


// loop()
//
// loop throughout the runtime
//
void loop() {

    prevBreathRate = avgBreathRate;
    prevBreathDepth = avgBreathDepth;
    prevHeartRate = heartRate;
    
    priorPulseSignal = pulseSignal; //store the previous value for comparison
    
    breathSignal = 0; //reset breath value
    pulseSignal = 0; //reset pulse value
  
    calculateInstantaneousSignals(); //calculate instantaneous values for tidal volume and blood oxygenation

    //every 25th frame, sample the breathing signal
    if (subsampleCounter == 0) { 
      updateTimeSeries(); //shift the time series by one frame to accomodate the new data point
    }

    calculatePulse(); //compute the instantaneous pulse in BPM

    trackBreath(); //carry out signal analysis to determine the shape of the breath signal
    
    breathPeriod = (inhaleLength + exhaleLength)/(1000.0/delayAmount); //calculate the length of the most recent breathing period
    breathRate = 60.0/breathPeriod; //calculate breaths/min from the most recent breath period
    
    updateBreathFeatures();

    deltaBreathRate = avgBreathRate - prevBreathRate;
    deltaBreathDepth = avgBreathDepth - prevBreathDepth;
    deltaHeartRate = heartRate - prevHeartRate;

    subsampleCounter = (subsampleCounter + 1) % 25; //increment subsampleCounter and rollover every 25 frames
    clockTracker = millis(); //update clockTracker

    //Serial.println(timeSeries[timeSeriesFrames]);
    //Serial.print(",");
    //Serial.println(pulseSignal);
    //Serial.print(",");
    //Serial.println(heartRate);
    //Serial.print(",");
    Serial.println(heartRate);
    Serial.print(",");
    Serial.println(deltaHeartRate + 90);
    delay(delayAmount);
}




// calculateInstantaneousSignals()
//
// takes a given numebr of samples and computes an instantaneous average
//
void calculateInstantaneousSignals() {
    //sample tidal volume
    for(int i = 0; i < numberOfSamples; i++) {
      breathSignal = breathSignal + analogRead(breathPin);
    }
    breathSignal = 1.05 * breathSignal / numberOfSamples; 
    
    //sample blood oxygenation
    for (int i = 0; i < numberOfSamples; i++) { 
      pulseSignal = pulseSignal + analogRead(pulsePin);
    }
    pulseSignal = 2 * pulseSignal / numberOfSamples;
}


// calculatePulse()
//
// calculates instantaneous pulse based on the time interval between the latest spike and the prior spike
//
void calculatePulse() {
    if (pulseSignal > pulseThreshold && priorPulseSignal <= pulseThreshold) { //if the pulse signal is crossing the threshold in an upward trend...
     currentSpike = clockTracker; //...store the current time
     pulseInterval = currentSpike - prevSpike; //calculate the time interval between the current spike and the prior spike
     updatePulseIntervals(); //shift historic pulse interval values by one frame to make room for the new one
     heartRate = calculateBPM(); //calculate the BPM using the historic interval values stored in pulseIntervalRecord
     prevSpike = currentSpike; //current spike becomes the previous spike for the next calculation
    } 
}


//  updatePulseIntervals()
//
//  shifts each pulse interval value by one frame and adds a new pulse interval value
//
void updatePulseIntervals() {
  //shifting old values by one frame to make room for a new one
  for (int i = 0; i < (numberOfPulseIntervalSamples); i++) {
    pulseIntervalRecord[i] = pulseIntervalRecord[i+1];
  }
  pulseIntervalRecord[numberOfPulseIntervalSamples] = pulseInterval; //adding new interval value
  return;
}



// updateTimeSeries()
//
// shifts each respiratory signal value by one frame and adds a new signal value
//
void updateTimeSeries() {
  //shift all the values of time series backwards one frame
  for(int i = 0; i < timeSeriesFrames; i++) {
    timeSeries[i] = timeSeries[i+1];
  }
  timeSeries[timeSeriesFrames] = breathSignal; //insert our new sensor value into the last frame
  return;
}


// calculateBPM()
//
// calculates heart rate in BPM based on an aggregate of recent pulse interval samples
//
int calculateBPM() {
  int aggregateInterval = 0; //temporary variable to calculate the interval averaged over our sampling window

  //add up all our intervals
  for (int i = 0; i < numberOfPulseIntervalSamples; i++) {
    aggregateInterval = aggregateInterval + pulseIntervalRecord[i];
  }
  aggregateInterval = aggregateInterval / numberOfPulseIntervalSamples; //divide our aggregate by the number of samples
  aggregateInterval = 60000.0/aggregateInterval; //calculate BPM from the average interval

  return aggregateInterval; //return heart rate in BPM
}


// detectIncrease()
//
// recognizes increases in the respiratory signal by identifying values more than a certain threshold above the local average of signal values
//
int detectIncrease () {
  long localAvg = 0;

  //calculate a local average of time series values from the 5 most recent frames
  for(int i = 0; i < 5; i++) {
    localAvg = localAvg + timeSeries[timeSeriesFrames-(i+1)];
  }
  localAvg = localAvg/5;

  //if the latest frame is greater than the local average by more than 2, return a 1 to indicate that the signal is increasing, else return a 0
  if (timeSeries[timeSeriesFrames] > (localAvg + 2)) {
    return 1;
  } else {
    return 0;
  }
}


// trackBreath()
//
// carries out various computation on length and depth of breath based on the detection of increasing sections of the respiratory signal
//
void trackBreath() {
  //if currently inhaling...
  if(detectIncrease() == 1) {
    if(inhaleCounter == 0) { //...and this is the first frame of the inhale...
      exhaleLength = exhaleCounter; //record the length of the prior exhale
      exhaleCounter = 0; //reset the exhale counter
      breathMax = (timeSeries[timeSeriesFrames] + 1.0);
      breathMin = timeSeries[timeSeriesFrames];
    } else {
      if (timeSeries[timeSeriesFrames] > breathMax) {
        breathMax = timeSeries[timeSeriesFrames];
      }
    }
    inhaleCounter++;
  }
  //if not currently inhaling...
  if(detectIncrease() == 0) {
    if(exhaleCounter == 0) { //...and this is the first frame of the exhale...
      inhaleLength = inhaleCounter; //record the length of the prior inahle
      inhaleCounter = 0; //reset the inhale counter
      breathDepth = ((2.0*breathMax) - (2.0*breathMin)); //calculate the depth of the most recent breath
    } 
    exhaleCounter++;
  }
}


// updateBreathFeatures()
//
// updates and calculates values for average breathing features
//
void updateBreathFeatures() {
  breathRateSeries[breathFeatureCounter] = breathRate;
  breathDepthSeries[breathFeatureCounter] = breathDepth;
  breathFeatureCounter = (breathFeatureCounter + 1) % 10;

  //reset averages
  avgBreathRate = 0.0;
  avgBreathDepth = 0;

  for(int i = 0; i < 10; i++) {
    avgBreathRate = avgBreathRate + breathRateSeries[i];
    avgBreathDepth = avgBreathDepth + breathDepthSeries[i];
  }
  avgBreathRate = avgBreathRate/10.0;
  avgBreathDepth = avgBreathDepth/10.0;
}
