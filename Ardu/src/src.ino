#include <Servo.h>

// ESC pins
constexpr int numMotors = 4;
constexpr int escPins[numMotors] = {3, 5, 6, 9};

// Create servo objects for each ESC
Servo escMotors[numMotors];

// ESC values
constexpr int MIN_THROTTLE = 1200; // Minimum throttle value (motors off)
constexpr int MAX_THROTTLE = 20000; // Maximum throttle value
constexpr int THROTTLE_STEP = 100;  // How much to change throttle by with each keypress
int currentThrottle = MIN_THROTTLE;

// Motor status tracking
bool motorAttached[numMotors] = {false, false, false, false};

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Drone ESC Debug - Press W to increase throttle, S to decrease");
  Serial.println("Other commands: '1','2','3','4' = test individual motors, 'R' = reset/reattach all");

  // Attempt to attach ESCs to pins one at a time
  attachAllESCs(false);
  // Arm ESCs
  armESCs();
  
  Serial.println("\n------ COMMANDS ------");
  Serial.println("W: Increase all throttles");
  Serial.println("S: Decrease all throttles");
  Serial.println("1-4: Test individual motors");
  Serial.println("R: Reset/reattach all motors");
  Serial.println("---------------------\n");
}

void loop() {
  // Check if data is available from the serial port
  if (Serial.available() > 0) {
    // Read the incoming byte
    char inputChar = Serial.read();
    
    // Process the command
    if (inputChar == 'W' || inputChar == 'w') {
      // Increase throttle for all motors
      increaseThrottle();
    } 
    else if (inputChar == 'S' || inputChar == 's') {
      // Decrease throttle for all motors
      decreaseThrottle();
    }
    else if (inputChar == 'R' || inputChar == 'r') {
      // Reset/reattach all motors
      resetMotors();
    }
    else if (inputChar >= '1' && inputChar <= '4') {
      // Test individual motor
      int motorIndex = inputChar - '1';
      testSingleMotor(motorIndex);
    }
  }
  
  // Keep sending the current throttle value to all attached motors
  updateAllESCs();
  
  // Short delay to stabilize
  delay(5);
}

// Function to test a single motor
void testSingleMotor(int motorIndex) {
  if (motorIndex >= 0 && motorIndex < numMotors) {
    Serial.print("Testing motor ");
    Serial.print(motorIndex + 1);
    Serial.print(" on pin ");
    Serial.println(escPins[motorIndex]);
    
    if (motorAttached[motorIndex]) {
      // Pulse the motor to a higher value for half a second
      
      // Set just this motor to a higher value
      escMotors[motorIndex].writeMicroseconds(4000);
      Serial.println("Motor should be spinning...");
      delay(1000);
      
      // Return to original value
      escMotors[motorIndex].writeMicroseconds(currentThrottle);
      Serial.println("Test complete");
    } else {
      Serial.println("ERROR: Motor not attached!");
    }
  }
}

void attachAllESCs(bool detach) {
  if (detach) {
    for (int i = 0; i < numMotors; i++) {
      Serial.print("detaching ");
      Serial.print(i);
      Serial.print("/");
      Serial.println(escPins[i]);

      escMotors[i].detach();
      motorAttached[i] = false;
      delay(50);
    }
  }

  for (int i = 0; i < numMotors; i++) {
    Serial.print("Attaching motor on pin ");
    Serial.print(escPins[i]);
    Serial.print("... ");
    
    // Try to attach the ESC
    escMotors[i].attach(escPins[i], MIN_THROTTLE, MAX_THROTTLE);
    
    // Check if attachment was successful
    if (escMotors[i].attached()) {
      motorAttached[i] = true;
      testSingleMotor(i);
      Serial.println("SUCCESS");
    } else {
      Serial.println("FAILED");
    }
    
    // Set to minimum throttle
    escMotors[i].writeMicroseconds(MIN_THROTTLE);
    delay(1000); // Short pause between attachments
  }
}

// Reset and reattach all motors
void resetMotors() {
  Serial.println("Resetting all motors...");
  
  attachAllESCs(false);
  
  // Re-arm ESCs
  armESCs();
}

// Function to consistently update all ESCs
void updateAllESCs() {
  for (int i = 0; i < numMotors; i++) {
    if (motorAttached[i]) {
      // For motors 1 and 2 (pins 3 and 4), reverse the direction
      if (i == 1 || i == 2) {
        escMotors[i].writeMicroseconds(MAX_THROTTLE + MIN_THROTTLE - currentThrottle);
      } else {
        escMotors[i].writeMicroseconds(currentThrottle);
      }
    }
  }
}

// Function to increase throttle
void increaseThrottle() {
  if (currentThrottle < MAX_THROTTLE) {
    currentThrottle += THROTTLE_STEP;
    if (currentThrottle > MAX_THROTTLE) {
      currentThrottle = MAX_THROTTLE;
    }
    Serial.print("Throttle increased to: ");
    Serial.println(currentThrottle);
  }
}

// Function to decrease throttle
void decreaseThrottle() {
  if (currentThrottle > MIN_THROTTLE) {
    currentThrottle -= THROTTLE_STEP;
    if (currentThrottle < MIN_THROTTLE) {
      currentThrottle = MIN_THROTTLE;
    }
    Serial.print("Throttle decreased to: ");
    Serial.println(currentThrottle);
  }
}

// Function to arm ESCs
void armESCs() {
  Serial.println("Arming ESCs...");
  
  // Ensure all are at minimum throttle
  for (int i = 0; i < numMotors; i++) {
    if (motorAttached[i]) {
      escMotors[i].writeMicroseconds(MIN_THROTTLE);
    }
  }
  
  // Wait for ESCs to recognize the signal
  delay(3000);
  
  Serial.println("ESCs armed. Ready for input.");
  Serial.print("Current throttle: ");
  Serial.println(currentThrottle);
}
