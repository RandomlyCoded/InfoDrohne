enum class Command {
  SetMotionVectors = 0x20,

  OptionSelectUDP = 0x40,
  OptionSelectBLE = 0x41,
  OptionSelectBtC = 0x42, // Bluetooth Classic, duh

  DebugSetThrottle   = 0x80,
  DebugEnableLogging = 0x81
};

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
