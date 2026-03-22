NetworkUDP udp;
NetworkClient remote;

void initUDP()
{
  Serial.println (WiFi.softAP("InfoDrone-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (26122); // Abi 26, Q 12/2
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
