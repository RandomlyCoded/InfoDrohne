#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Servo.h>
#include <WiFi.h>

constexpr int numMotors = 4;
ESP32PWM escMotors[numMotors];

int packetCount                       = 0;
constexpr int    freq                 = 1000;
constexpr double DEBUG_THROTTLE_SCALE = 1. / 256;

constexpr int LED_btMode     = 2;
constexpr int LED_connStatus = 5;
constexpr int PIN_button     = 33;

constexpr auto serviceUUID        = "69fc8256-d3ca-9112-bce0-388de24860d3";
constexpr auto characteristicUUID = "ebc49f11-9ffc-96b2-2a4e-6994a99175b5";

constexpr bool doPrint = false;

bool btMode = true;
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
  udp.begin (26122); // Abi 26, Q 12/2

  Serial.println ("Server started. Starting BLE...");

  BLEDevice::init("InfoDrone-BLE");
  auto bleServer = BLEDevice::createServer();

  auto bleService = bleServer->createService(serviceUUID);
  auto throttleCharacteristic = bleService->createCharacteristic(characteristicUUID, BLECharacteristic::PROPERTY_WRITE);

  throttleCharacteristic->setCallbacks(new BLECallbacks());

  bleService->start();
  bleServer->getAdvertising()->start();

  Serial.println("attaching Motors...");
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
