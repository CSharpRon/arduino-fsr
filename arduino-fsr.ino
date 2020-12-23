/* This package takes input from a Force Sensitive Resistor (fsr) and generates an appropriate vibrational response via PWM. 
 * 
 * Sources:
 *  https://www.instructables.com/Interfacing-Force-Sensitive-Resistor-to-Arduino/
 *  https://www.precisionmicrodrives.com/content/how-to-drive-a-vibration-motor-with-arduino-and-genuino/
 */
#define VIB 11 // vibrational motor connected to d11
#define FSR 0 // fsr connected to a0
#define LO 45
#define MEDIUM 120
#define HI 153
#define OFF 0

#include <Arduino_KNN.h>
const int INPUTS = 1;
const int CLASSES = 3;
const int EXAMPLES_PER_CLASS = 30;

const int K = 5;
KNNClassifier myKNN(INPUTS);

String label[CLASSES] = {"Low", "Medium", "High"};
float color[INPUTS];
const int THRESHOLD = 5;

/* FSR simple testing sketch.  <br>Connect one end of FSR to power, the other end to Analog 0.
Then connect one end of a 10K resistor from Analog 0 to ground 
*/
void setup() {
  pinMode(VIB, OUTPUT);
  Serial.begin(9600);   
}

void loop() {

  
  int fsrReading; // the analog reading from the FSR resistor divider
  fsrReading = analogRead(FSR);  

  //Serial.print("Analog reading = ");
  //Serial.print(fsrReading);     // the raw analog reading

  Serial.print("Off")
  
  if (fsrReading == 0) {
    analogWrite(VIB, OFF);
    Serial.println(" - No pressure");
  } else if (fsrReading < 200) {
    analogWrite(VIB, LO);
    Serial.println(" - Light squeeze");
  } else if (fsrReading < 700) {
    analogWrite(VIB, MEDIUM);
    Serial.println(" - Medium squeeze");
  } else {
    analogWrite(VIB, HI);
    Serial.println(" - Big squeeze");
  }

  // Sample at 200ms
  delay(200);

}
