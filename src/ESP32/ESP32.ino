#include "motor.h"
#include "networking.h"

constexpr int numMotors = 4;
auto escMotors = std::array<Motor, numMotors> {};

          int packetCount = 0;
constexpr int pwmFreq     = 50;
constexpr int pwmRange    = 1023; // 10 bit

bool doPrint = false;

#ifndef LED_BUILTIN
#define LED_BUILTIN 8
#endif // LED_BUILTIN

void setup () {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin (115200);

  initUDP();

  Serial.println ("Server started. Starting BLE...");

  initBLE();

  Serial.println("attaching Motors...");

#if defined(ESP32)
  constexpr int pins[numMotors] = {A0, A1, A2, A3};
#elif defined(ESP8266)
  constexpr int pins[numMotors] = {D8, D7, D6, D5};
#endif // defined ESP32

  for (int i = 0; i < numMotors; ++i)
    escMotors[i].attach(pins[i]);

  Serial.println ("ready!");
  Serial.print ("MODE: ");
  Serial.println(btMode ? "BtLE" : "UDP");

  ledcAttach(LED_BUILTIN, pwmFreq, ceil(log2(pwmRange)));
}

void loop () {
  const auto now = millis() % 1000;

  ledcWrite(LED_BUILTIN, now < 250 ? pwmRange : 0);

  if (!btMode) // btle is handled via callbacks
    handleUdp();
  
  for (auto &motor: escMotors) {
    motor.update();
/*
    if (doPrint)
      Serial.printf("%f (target %f)\n", motor.currentSpeed(), motor.targetSpeed());*/
  }

/*  if (doPrint)
    Serial.println(); */
  // TODO: debug level

  delay(1); // avoid overheating
}
