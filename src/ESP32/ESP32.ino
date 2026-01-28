#include <WiFi.h>

#define LED_BUILTIN 2

using uint16 = unsigned short;

NetworkServer server (80);

void setup () {
  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin (9600);

  Serial.println (WiFi.softAP("InfoDrohne-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  server.begin ();

  Serial.println ("Server started");
}

NetworkClient client;

String readLine () {
  String line = "";
  while (client.available ()) {
    char c = client.read ();
    if (c == '\n')
      break;
    
    if (c != '\r') // who cares about \r?
      line += c;
  }

  return line;
}

void discardHeaders () {
  // the body starts after the first 0-length line
  while (readLine ().length () > 0)
    ;
}

void acceptThrottle () {
  discardHeaders ();

  uint16 throttles[4] = {0, 0, 0, 0};

  for (int i = 0; i < 4; ++i) {
    char lower  = client.read ();
    char higher = client.read ();

    throttles[i] = uint16(lower) | (uint16(higher) << 8);
    Serial.print (i);
    Serial.print (" -> ");
    Serial.println (throttles[i]);
  }
}

void loop () {
  if (!client.connected () || !client)
    client = server.accept ();

  if (client && client.connected() && client.available ()) {
    String command = readLine();

    if (command.startsWith("POST /throttle")) {
      acceptThrottle();
      Serial.println("throttle accepted");
      client.print("throttle received");
    } else {
      client.print("unknown command: ");
      client.print(command);
    }
  }

  delay(50);
}
