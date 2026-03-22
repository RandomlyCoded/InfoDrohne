#ifndef DRONE_NETWORKING_H
#define DRONE_NETWORKING_H

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

static void initBLE();
static void initUDP();

static bool btMode          = true && ENABLE_BLE;

#endif // DRONE_NETWORKING_H