/* This package takes input from a Force Sensitive Resistor (fsr) and generates an appropriate vibrational response via PWM. 
 * Vibration is returned after training a kNN model.
 * 
 * Author: Ron Marrero github.com/CSharpRon
 * Sources:
 *  https://www.instructables.com/Interfacing-Force-Sensitive-Resistor-to-Arduino/
 *  https://www.precisionmicrodrives.com/content/how-to-drive-a-vibration-motor-with-arduino-and-genuino/
 *  https://blog.arduino.cc/2020/06/18/simple-machine-learning-with-arduino-knn/
 */
#define VIB 11 // vibrational motor connected to d11
#define FSR 0 // fsr connected to a0
#define OFF 0
#define LO 45
#define MEDIUM 120
#define HI 153

// Constants needed to use kNN.
const int CLASSES = 4;
const int SAMPLE_SIZE = 5;

bool sample = true;                                           // When true, program starts by taking samples
String label[CLASSES] = {"Off", "Low", "Medium", "High"};     // Labels for each of the vibration states

/* To make this work, you need one FSR and one Vibration Motor.
 * Connect one end of FSR to power, the other end to Analog 0.
 * Then connect one end of a 10K resistor from Analog 0 to ground 
*/
void setup() {
  pinMode(VIB, OUTPUT);
  Serial.begin(9600);   
}

void loop() { 
  
  /* On initialization, sample will be equal to true. This will let you take toggle it off once you trained your model.
     Output is saved to serial monitor and global forceCLassifier variable..
  */  

  Serial.println("Class,Sample1,Sample2,Sample3,Sample4,Sample5,Mean,TightMean");

  // Get samples of data in each class
  for (int c = 0; c < CLASSES; c++) {
    sampleData(c);
  }

  // 1 second delay
  delay(1000);
}

// Runs 5 samples a second for 10 seconds total. Data will be used to train kNN.
void sampleData(int classNumber) {
  
  char sampleMessage[50];
  int fsrReading; // the analog reading from the FSR resistor divider

  Serial.print("Sampling ");
  Serial.print(label[classNumber]);
  Serial.println(" in 5 seconds...");
  delay(5000);

  // Take samples at each step
  for (int i = 0; i < 25; i++) { 
    // Begin taking sample
    Serial.print(classNumber);
    Serial.print(",");
    getFSRReading(); 
  }
}

// Populates the passed in array with data taken at each ms step defined by SAMPLE_SIZE.
void getFSRReading() {
  
  float mean = 0;
  float tightMean = 0;
  float values[100];
  int min = 10000;
  int max = 0;

  // Get the values
  for (int j = 0; j < SAMPLE_SIZE; j++) {
    int fsrReading = analogRead(FSR);  
    values[j] = fsrReading;
    if (fsrReading < min) {
      min = fsrReading;
    }
    if (fsrReading > max) {
      max = fsrReading;
    }
    delay(1000/SAMPLE_SIZE);
  }

  // Do the calculations after values have been collected
  for (int v = 0; v < SAMPLE_SIZE; v++) {
    Serial.print(values[v]);
    Serial.print(",");
    
    // Adjust mean values
    mean = mean + values[v]; 
    
  }

  // If mean is 0, return to avoid divide by 0 error
  if (mean == 0) {
    Serial.println("0,0");
    return;
  }

  tightMean = mean;
  mean = mean / SAMPLE_SIZE;
  if (max != min) {
    tightMean = tightMean - max - min;
    tightMean = tightMean / (SAMPLE_SIZE - 2);
  } else {
    tightMean = mean;
  }

  Serial.print(mean);
  Serial.print(",");
  Serial.println(tightMean);
} 
