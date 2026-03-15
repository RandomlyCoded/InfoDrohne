#include <WiFi.h>

#include <ESP32Servo.h>

constexpr int numMotors = 4;

Servo escMotors[numMotors];

constexpr int MIN_THROTTLE = 500; // these values are enforced by the esp32Servo library
constexpr int MAX_THROTTLE = 2048; // 2500 is the max supported; 2048 makes conversion easier 
int currentThrottle = MIN_THROTTLE;

int packetCount = 0;

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

  Serial.begin (115200);

  Serial.println (WiFi.softAP("InfoDrohne-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (8080);

  Serial.println ("Server started");

  constexpr int pins[numMotors] = {32, 27, 25, 26};

//  for (int i = 0; i < numMotors; ++i)
//    escMotors[i] = Motor(pins[i], i);
  
  Serial.println ("ready!");
}

void loop () {
  bool buttonPressed = analogRead(PIN_button) > 4000;
  if (buttonPressed && !lastButtonState && false) { // we don't support switching rn
    btMode ^= 1;
  }

  lastButtonState = buttonPressed;
  if (btMode)
    handleBtLE();
  else
    handleUdp();
  
  delay(10);  
}

void handleBtLE() {

}

void handleUdp() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.printf("new packet: %d\n", packetSize);
    // .c_str() to shut the compiler up (%s expects char*, not String)
    Serial.printf("Received packet from %s: %d (%d)\n", udp.remoteIP().toString().c_str(), udp.remotePort(), packetCount++);

    char buffer[255];
    int len = udp.read(buffer, 255);
    uint16 *throttles = (uint16*)buffer;
    for (int i = 0; i < 4; ++i) {
      Serial.printf("%d -> %d\n", i, throttles[i]);
//      escMotors[i].setDuty(throttles[i]);
    }
  }
}
