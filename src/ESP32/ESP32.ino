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

enum class Command {
  SetMotionVectors = 0x20,

  OptionSelectUDP = 0x40,
  OptionSelectBLE = 0x41,
  OptionSelectBtC = 0x42, // Bluetooth Classic, duh

  DebugSetThrottle   = 0x80,
  DebugEnableLogging = 0x81
};

constexpr int numMotors = 4;
auto escMotors = std::array<Motor, numMotors> {};

          int packetCount = 0;
constexpr int pwmFreq     = 50;
constexpr int pwmRange    = 1023; // 10 bit

constexpr int LED_btMode     = 2;
constexpr int LED_connStatus = 5;
constexpr int PIN_button     = 33;

constexpr auto serviceUUID        = "69fc8256-d3ca-9112-bce0-388de24860d3";
constexpr auto characteristicUUID = "ebc49f11-9ffc-96b2-2a4e-6994a99175b5";

bool doPrint = false;

bool btMode          = true && ENABLE_BLE;
bool lastButtonState = false;

NetworkUDP udp;
NetworkClient remote;

void setMotionVectors(const char *buffer)
{
  // TODO: implement motion vectors
}

void selectUDP()
{
  btMode = false;
}

void selectBLE() {
  btMode = true;
}

void selectBtC() {
  Serial.println("ERROR: BtC is not supported. Forcing BLE...");

  btMode = true;
}

void enableLogging(const char *buffer)
{
  doPrint = buffer[1];
}

void setDebugThrottle(const char *buffer) {
    const auto throttles = reinterpret_cast<const uint8_t*>(buffer + 1);

    for (int i = 0; i < 4; ++i) {
      if (doPrint)
        Serial.printf("%d -> %d\n", i, throttles[i]);
      
      escMotors[i].setSpeed(throttles[i] / 255.);
    }

}

void handlePacket(const char *buffer, int len)
{
  if (len < 1) // 0 bytes or error
    return;

  if (len < expectedLength(buffer[0])) // incomplete packet
    return;

  switch (static_cast<Command>(buffer[0])) {
    case Command::SetMotionVectors: setMotionVectors(buffer); break;

    case Command::OptionSelectUDP: selectUDP(); break;
    case Command::OptionSelectBLE: selectBLE(); break;
    case Command::OptionSelectBtC: selectBtC(); break;

    case Command::DebugSetThrottle: setDebugThrottle(buffer); break;
    case Command::DebugEnableLogging: enableLogging(buffer); break;
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

size_t expectedLength(const uint8_t flags)
{
  switch (static_cast<Command>(flags)) {
    case Command::SetMotionVectors: return 7;

    case Command::OptionSelectUDP: return 1;
    case Command::OptionSelectBLE: return 1;
    case Command::OptionSelectBtC: return 1;

    case Command::DebugSetThrottle: return 5;
    case Command::DebugEnableLogging: return 2;
  }

  return 0;
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
  
  for (auto motor: escMotors)
    motor.update();

  delay(1); // avoid overheating
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
