#include <WiFi.h>

#include <ESP32Servo.h>

constexpr int numMotors = 4;
ESP32PWM escMotors[numMotors];

int packetCount                       = 0;
constexpr int    freq                 = 1000;
constexpr double DEBUG_THROTTLE_SCALE = 1. / 256;

using uint16 = unsigned short;

constexpr int LED_btMode = 2;
constexpr int LED_connStatus = 5;
constexpr int PIN_button = 33;

constexpr bool doPrint = false;

bool btMode = false;
bool lastButtonState = false;

NetworkUDP udp;
NetworkClient remote;

void handlePacket(const char *buffer, int len)
{
  if (len < 1) // 0 bytes or error
    return;

  if (len < expectedLength(buffer[0])) // incomplete packet
    return;

  if (buffer[0] & 128) { // debug
    uint8_t *throttles = (uint8_t*)(buffer + 1);
    for (int i = 0; i < 4; ++i) {
      if (doPrint)
        Serial.printf("%d -> %d (%f)\n", i, throttles[i], throttles[i] * DEBUG_THROTTLE_SCALE);
      
      escMotors[i].writeScaled(throttles[i] * DEBUG_THROTTLE_SCALE);
    }
  }
}

bool expectedLength(char flags)
{
  if (flags & 128) // debug
    return 5; // flag + 4x throttle

  return 7; // flag + 3x dir + 3x rotation
}

void setup () {
  pinMode (LED_btMode, OUTPUT);
  pinMode (LED_connStatus, OUTPUT);

  Serial.begin (115200);
  Serial.println (WiFi.softAP("InfoDrone-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (8080);

  Serial.println ("Server started");

  constexpr int pins[numMotors] = {32, 27, 25, 26};

  Serial.println("attaching ESPs...");
  constexpr int pins[numMotors] = {12, 13, 15, 14};
  for (int i = 0; i < numMotors; ++i) {
	  ESP32PWM::allocateTimer(i);
    escMotors[i].attachPin(pins[i], freq, 10);
  }
  
  Serial.println("waiting for arm...");
  delay(3000 * 0);
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
    if (doPrint) {
      Serial.printf("new packet: %d\n", packetSize);
      // .c_str() to shut the compiler up (%s expects char*, not String)
      Serial.printf("Received packet from %s: %d (%d)\n", udp.remoteIP().toString().c_str(), udp.remotePort(), packetCount++);
    }

    char buffer[255];
    int len = udp.read(buffer, 255);
    handlePacket(buffer, len);
  }
}
