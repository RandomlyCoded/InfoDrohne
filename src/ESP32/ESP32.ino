#include <WiFi.h>

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
    for (int i = 0; i < 4; ++i)
      Serial.printf("%d -> %d\n", i, throttles[i]);
  }
}