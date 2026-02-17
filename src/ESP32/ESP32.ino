#include <ESP32Servo.h>
#include <WiFi.h>

constexpr int numMotors = 4;
constexpr int escPins[numMotors] = {32, 25, 26, 27};

Servo escMotors[numMotors];

constexpr int MIN_THROTTLE = 500; // these values are enforced by the esp32Servo library
constexpr int MAX_THROTTLE = 2048; // 2500 is the max supported; 2048 makes conversion easier 
int currentThrottle = MIN_THROTTLE;

using uint16 = unsigned short;

constexpr int LED_btMode = 2;
constexpr int LED_connStatus = 5;
constexpr int PIN_button = 33;


bool btMode = false;
bool lastButtonState = false;

NetworkUDP udp;
NetworkClient remote;

void setup () {
  pinMode (LED_btMode, OUTPUT);
  pinMode (LED_connStatus, OUTPUT);
  
  Serial.begin (9600);

  Serial.println (WiFi.softAP("InfoDrohne-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (8080);

  Serial.println ("Server started");
  resetMotors ();
  Serial.println ("ready!");
}

void loop () {
  bool buttonPressed = analogRead(PIN_button) > 4000;
  if (buttonPressed && !lastButtonState) {
    btMode ^= 1;
  }

  lastButtonState = buttonPressed;
  if (btMode)
    handleBtLE();
  else
    handleUdp();
  
  delay(1);
}

void handleBtLE() {

}

void handleUdp() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet from: ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.println(udp.remotePort());

    char buffer[255];
    int len = udp.read(buffer, 255);
    uint16 *throttles = (uint16*)buffer;
    for (int i = 0; i < 4; ++i) {
      Serial.printf("%d -> %d\n", i, throttles[i]);
      escMotors[i].writeMicroseconds(throttles[i] / 32); // 16 bit in, clamp to 11 bits
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
      delay(50);
    }
  }

  for (int i = 0; i < numMotors; i++) {
    Serial.print("Attaching motor on pin ");
    Serial.print(escPins[i]);
    Serial.print("... ");
    
    // Try to attach the ESC
    escMotors[i].attach(escPins[i], MIN_THROTTLE, MAX_THROTTLE);
    
    // Set to minimum throttle
    escMotors[i].writeMicroseconds(MIN_THROTTLE);
    delay(1000); // Short pause between attachments
  }
}

void resetMotors() {
  Serial.println("Resetting all motors...");
  
  attachAllESCs(false);
  
  // Re-arm ESCs
  armESCs();
}

void armESCs() {
  Serial.println("Arming ESCs...");
  
  // Ensure all are at minimum throttle
  for (int i = 0; i < numMotors; i++) {
    escMotors[i].writeMicroseconds(MIN_THROTTLE);
  }
  
  // Wait for ESCs to recognize the signal
  delay(3000);
  
  Serial.println("ESCs armed. Ready for input.");
  Serial.print("Current throttle: ");
  Serial.println(currentThrottle);
}