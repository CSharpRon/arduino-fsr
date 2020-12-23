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

#include <Arduino_KNN.h>

// Constants needed to use kNN.
const int CLASSES = 4;
const int K = 5;
const int SAMPLE_SIZE = 5;

bool sample = true;                                           // When true, program starts by taking samples
float fsrData[SAMPLE_SIZE];                                   // Holds data captured by the FSR
String label[CLASSES] = {"Off", "Low", "Medium", "High"};     // Labels for each of the vibration states

KNNClassifier forceClassifier(SAMPLE_SIZE);

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
  if (sample) {
      // Get samples of data in each class
      for (int c = 0; c < CLASSES; c++) {
        sampleData(c);
      }
      sample = false; // Our model is now trained!
  }

  // As long as our model is trained, we can classify data!
  getFSRReading(fsrData);
  int classification = forceClassifier.classify(fsrData, 5);

  Serial.print(label[classification]);
  Serial.println(" - Force Detected");

  sendMotorVibration(classification);

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

  // Begin taking sample
  Serial.println(label[classNumber]);

  // At each step (1 second), 5 samples are taken and used as an example of the force class.
  for (int i = 0; i < 10; i++) {

    getFSRReading(fsrData);

    forceClassifier.addExample(fsrData, classNumber);
  }
}

// Populates the passed in array with data taken at each 200ms step defined in SAMPLE_SIZE.
void getFSRReading(float data[]) {

  for (int j = 0; j < SAMPLE_SIZE; j++) {
    int fsrReading = analogRead(FSR);  
    data[j] = fsrReading;
    Serial.println(fsrReading);
    delay(200);
  }
  
}

// Send a vibration to the motor based on one of the four states (off, low, medium, high).
void sendMotorVibration(int mode) {

   switch (mode) {
      case 0:
        analogWrite(VIB, OFF);
        break;
      case 1:
        analogWrite(VIB, LO);
        break;
      case 2:
        analogWrite(VIB, MEDIUM);
        break;
      case 3:
        analogWrite(VIB, HI);
        break;
      default:
        break;
   }
}
