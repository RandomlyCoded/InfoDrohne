#include "networking.h"

#if ENABLE_BLE
constexpr auto serviceUUID        = "69fc8256-d3ca-9112-bce0-388de24860d3";
constexpr auto characteristicUUID = "ebc49f11-9ffc-96b2-2a4e-6994a99175b5";

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

void initBLE()
{

  BLEDevice::init("InfoDrone-BLE");
  auto bleServer = BLEDevice::createServer();

  auto bleService = bleServer->createService(serviceUUID);
  auto throttleCharacteristic = bleService->createCharacteristic(characteristicUUID, BLECharacteristic::PROPERTY_WRITE);

  throttleCharacteristic->setCallbacks(new BLECallbacks());

  bleService->start();
  bleServer->getAdvertising()->start();
}

#else // no BLE
void initBLE() {
  Serial.println("Skipping BLE (unsupported on platform)");
}

#endif // ENABLE_BLE
