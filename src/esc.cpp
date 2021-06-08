#include "esc.h"

void ESC::setup()
{
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  // start the CAN bus at 500 kbps
  CAN.begin(500E3);
}

void ESC::loop()
{
  if (!requestSent)
    sendNextRequest();
  else
    readCAN();
}

void ESC::sendNextRequest()
{
  if (lastRequestMillis + CAN_INTERVAL > millis())
    return; // don't do anything until CAN_INTERVAL elapsed

  step++;
  switch (step)
  {
  case 1:
    getRealtimeData();
    break;
  case 2:
    getBalance();
    break;
#ifdef BMS_CAN_ID
  case 3:
    getBMS();
    break;
#endif
  default:
    step = 0;
    return;
  }
  requestSent = true;
  lastRequestMillis = millis();
}

void ESC::getRealtimeData()
{
  requestID = 0x32;
  expectedLength = 18;
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + ESC_CAN_ID;
  canMsg1.can_dlc = 0x07;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = requestID;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = B10000001;
  canMsg1.data[6] = B11000011;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();
}

void ESC::getBalance()
{
  requestID = 0x4F;
  expectedLength = 37;
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + ESC_CAN_ID;
  canMsg1.can_dlc = 0x03;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = requestID;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();
}

#ifdef BMS_CAN_ID
void ESC::getBMS()
{
  requestID = 0x04;
  expectedLength = 50;
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + BMS_CAN_ID;
  canMsg1.can_dlc = 0x03;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = requestID;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();
}
#endif

void ESC::startOver()
{
  readBufferLength = 0;
  std::fill(readBuffer, readBuffer + 50, 0);
  readBufferInfoLength = 0;
  std::fill(readBufferInfo, readBufferInfo + 8, 0);
  supposedLength = 0;
  requestSent = false;
}

void ESC::readCAN()
{
  int packetSize = CAN.parsePacket();
  while (packetSize)
  {
    currentPacket.can_id = CAN.packetId();
    currentPacket.can_dlc = packetSize;
    for (int i = 0; i < packetSize; i++)
      currentPacket.data[i] = CAN.read();

    // printFrame(&currentPacket); // for debugging
    if (currentPacket.can_id == ((uint16_t)CAN_PACKET_FILL_RX_BUFFER << 8) + BALANCE_BUDDY_CAN_ID)
    {
      for (int j = 1; j < currentPacket.can_dlc; j++)
      {
        readBuffer[currentPacket.data[0] + j - 1] = currentPacket.data[j];
        readBufferLength++;
      }
    }
    else if (currentPacket.can_id == ((uint16_t)CAN_PACKET_PROCESS_RX_BUFFER << 8) + BALANCE_BUDDY_CAN_ID)
    {
      for (int j = 0; j < currentPacket.can_dlc; j++)
      {
        readBufferInfo[j] = currentPacket.data[j];
        readBufferInfoLength++;
      }
    }
    packetSize = CAN.parsePacket();
  }

  if (readBufferInfoLength && !supposedLength)
    supposedLength = ((uint16_t)readBufferInfo[2] << 8) + readBufferInfo[3];

  // TODO: Proper read validation with checksum, for now we just compare lengths, can improve if data is error prone.
  if (supposedLength && readBufferLength == supposedLength)
  {
    if (readBufferLength == expectedLength && readBuffer[0] == requestID)
      parseRequest();
    startOver();
  }
  else if (supposedLength && readBufferLength > supposedLength) // this means something is wrong and we got too much data
    startOver();
  else if (lastRequestMillis + CAN_TIMEOUT < millis()) // give up and start over
    startOver();
}

void ESC::parseRequest()
{
  switch (step)
  {
  case 1:
    parseRealtimeData();
    break;
  case 2:
    parseBalance();
    break;
#ifdef BMS_CAN_ID
  case 3:
    parseBMS();
    break;
#endif
  }
}

void ESC::parseRealtimeData()
{
  tempMosfet = read_double_2(5, 10);
  tempMotor = read_double_2(7, 10);
  dutyCycle = read_double_2(9, 1000);
  erpm = read_double_4(11, 1);
  voltage = read_double_2(15, 10);
  fault = read_uint8(17);
  convert_erpm();
}

void ESC::convert_erpm()
{
  rpm = erpm / (MOTOR_POLES / 2);
  speed_ms = rpm * WHEEL_DIAMETER_MM * PI / 1000 / 60;
}

void ESC::parseBalance()
{
  pidOutput = read_double_4(1, 1000000);
  pitch = read_double_4(5, 1000000);
  roll = read_double_4(9, 1000000);
  loopTime = read_uint32(13);
  motorCurrent = read_double_4(17, 1000000);
  motorPosition = read_double_4(21, 1000000);
  balanceState = read_uint16(25);
  switchState = read_uint16(27);
  adc1 = read_double_4(29, 1000000);
  adc2 = read_double_4(33, 1000000);
}

#ifdef BMS_CAN_ID
void ESC::parseBMS()
{
  packVoltage = read_double_4(1, 1000);
  packCurrent = read_double_4(5, 1000);
  packSoC = read_uint8(9);
  cellVoltageHigh = read_double_4(10, 1000);
  cellVoltageAverage = read_double_4(14, 1000);
  cellVoltageLow = read_double_4(18, 1000);
  cellVoltageMisMatch = read_double_4(22, 1000);
  // there's more that I don't feel like adding now
}
#endif

uint8_t ESC::read_uint8(uint8_t pos)
{
  return readBuffer[pos];
}

uint16_t ESC::read_uint16(uint8_t pos)
{
  return ((uint16_t)readBuffer[pos] << 8) + ((uint16_t)readBuffer[pos + 1]);
}

uint32_t ESC::read_uint32(uint8_t pos)
{
  return ((uint32_t)readBuffer[pos] << 24) + ((uint32_t)readBuffer[pos + 1] << 16) + ((uint32_t)readBuffer[pos + 2] << 8) + ((uint32_t)readBuffer[pos + 3]);
}

double ESC::read_double_2(uint8_t pos, double scale)
{
  if (scale == 0.0)
    scale = 1.0;
  return int16_t(((int16_t)readBuffer[pos] << 8) + ((int16_t)readBuffer[pos + 1])) / scale;
}

double ESC::read_double_4(uint8_t pos, double scale)
{
  if (scale == 0.0)
    scale = 1.0;
  return (((int32_t)readBuffer[pos] << 24) + ((int32_t)readBuffer[pos + 1] << 16) + ((int32_t)readBuffer[pos + 2] << 8) + ((int32_t)readBuffer[pos + 3])) / scale;
}

void ESC::printFrame(struct can_frame *frame)
{
  Serial.print(frame->can_id, HEX); // print ID
  Serial.print(" ");
  Serial.print(frame->can_dlc, HEX); // print DLC
  Serial.print(" ");
  for (int i = 0; i < frame->can_dlc; i++)
  { // print the data
    Serial.print(frame->data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
