#include <WiFi.h>

#define LED_BUILTIN 2

using uint16 = unsigned short;

NetworkUDP udp;
NetworkClient remote;

void setup () {
  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (9600);

  Serial.println (WiFi.softAP("InfoDrohne-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (8080);

  Serial.println ("Server started");
}

void loop () {
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
  
  return;
}
