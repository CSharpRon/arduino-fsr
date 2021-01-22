/* This package takes input from a Force Sensitive Resistor (fsr) and generates an appropriate vibrational response via PWM. 
 * Vibration is returned after training a SVM model from Python.
 * 
 * Author: Ron Marrero github.com/CSharpRon
 * Sources:
 *  https://www.instructables.com/Interfacing-Force-Sensitive-Resistor-to-Arduino/
 *  https://www.precisionmicrodrives.com/content/how-to-drive-a-vibration-motor-with-arduino-and-genuino/
 */
#define VIB 11 // vibrational motor connected to d11
#define FSR 0 // fsr connected to a0

int pin_number, value_to_write;
char mode, operation;
/* To make this work, you need one FSR and one Vibration Motor.
 * Connect one end of FSR to power, the other end to Analog 0.
 * Then connect one end of a 10K resistor from Analog 0 to ground 
*/
void setup() {
  pinMode(VIB, OUTPUT);
  Serial.begin(9600);   
}

// Keep printing the FSR values at each tenth of a second
void loop() { 
  
  // Handle any operations coming in from Serial
  if (Serial.available() > 0) {
    operation = Serial.read();
    delay(5);
    mode = Serial.read();
    pin_number = Serial.parseInt();
    if (Serial.read() == ':'){
      value_to_write = Serial.parseInt();
    }
    switch (operation){
      case 'W': // Write operation, e.g., WD3:1
        if (mode == 'A') {
          analogWrite(pin_number, value_to_write);
        } else if (mode == 'D') {
          digitalWrite(pin_number, value_to_write);
        } else {
          break;
        }
      default:
        break;
    }
  }

  // Always get fsrReading and print to Serial  
  int fsrReading = analogRead(FSR);  
  Serial.println(fsrReading);
}
