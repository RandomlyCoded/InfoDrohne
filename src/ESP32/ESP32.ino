#if defined(ESP32)
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#define ENABLE_BLE 1

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

using NetworkUDP    = WiFiUDP;
using NetworkClient = WiFiClient;

#define ENABLE_BLE 0

#else

#error "Unsupported platform. Use an ESP32."

#endif // defined(ESP32)

#include "motor.h"

constexpr int numMotors = 4;
Motor escMotors[numMotors];

          int packetCount = 0;
constexpr int pwmFreq     = 50;
constexpr int pwmRange    = 1023; // 10 bit

constexpr int LED_btMode     = 2;
constexpr int LED_connStatus = 5;
constexpr int PIN_button     = 33;

constexpr auto serviceUUID        = "69fc8256-d3ca-9112-bce0-388de24860d3";
constexpr auto characteristicUUID = "ebc49f11-9ffc-96b2-2a4e-6994a99175b5";

constexpr bool doPrint = true;

bool btMode          = true && ENABLE_BLE;
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
        Serial.printf("%d -> %d\n", i, throttles[i]);
      
      escMotors[i].setSpeed(throttles[i] / 255.);
    }
  }
}

#if ENABLE_BLE
class BLECallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) {
    if (doPrint)
      Serial.println("new BtLE packet!");

    String value = pCharacteristic->getValue();

    if (!btMode)
      return;

    handlePacket(value.c_str(), value.length());
  }
};
#endif // ENABLE_BLE

bool expectedLength(char flags)
{
  if (flags & 128) // debug
    return 5; // flag + 4x throttle

  return 7; // flag + 3x dir + 3x rotation
}

#ifndef LED_BUILTIN
#define LED_BUILTIN 8
#endif // LED_BUILTIN

void setup () {
  pinMode (LED_btMode, OUTPUT);
  pinMode (LED_connStatus, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin (115200);
  Serial.println (WiFi.softAP("InfoDrone-WiFi", "sekuriti")); // security, but stonks
  
  IPAddress myIP = WiFi.softAPIP ();
  Serial.print ("AP IP address: ");
  Serial.println (myIP);
  udp.begin (26122); // Abi 26, Q 12/2

  Serial.println ("Server started. Starting BLE...");

#if ENABLE_BLE
  BLEDevice::init("InfoDrone-BLE");
  auto bleServer = BLEDevice::createServer();

  auto bleService = bleServer->createService(serviceUUID);
  auto throttleCharacteristic = bleService->createCharacteristic(characteristicUUID, BLECharacteristic::PROPERTY_WRITE);

  throttleCharacteristic->setCallbacks(new BLECallbacks());

  bleService->start();
  bleServer->getAdvertising()->start();
#else
  Serial.println("skipping!");
#endif // ENABLE_BLE

  Serial.println("attaching Motors...");

#if defined(ESP32)
  constexpr int pins[numMotors] = {A0, A1, A2, A3};
#elif defined(ESP8266)
  constexpr int pins[numMotors] = {D8, D7, D6, D5};
#endif // defined ESP32

  for (int i = 0; i < numMotors; ++i)
    escMotors[i].attach(pins[i]);
  
  Serial.println("waiting for arm...");

  Serial.println ("ready!");
  Serial.print ("MODE: ");
  Serial.println(btMode ? "BtLE" : "UDP");

  ledcAttach(LED_BUILTIN, pwmFreq, ceil(log2(pwmRange)));
}

void loop () {
  const auto now = millis() % 1000;

  ledcWrite(LED_BUILTIN, now < 250 ? pwmRange : 0);

/*  bool buttonPressed = analogRead(PIN_button) > 4000;
  if (buttonPressed && !lastButtonState && false) { // we don't support switching rn
    btMode ^= 1;
  }

  lastButtonState = buttonPressed;*/

  if (!btMode) // btle is handled via callbacks
    handleUdp();
  
  delay(1);
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
