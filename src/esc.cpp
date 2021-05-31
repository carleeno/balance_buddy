#include "esc.h"

void ESC::setup()
{
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  // start the CAN bus at 500 kbps
  CAN.begin(500E3);

  // For testing
  getBalance();
}

void ESC::loop()
{
  getRealtimeData();
  getBalance();
#ifdef BMS_CAN_ID
  getBMS();
#endif
}

void ESC::getRealtimeData()
{
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + ESC_CAN_ID;
  canMsg1.can_dlc = 0x07;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x32;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = B10000001;
  canMsg1.data[6] = B11000011;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();

  batchRead();
  parseRealtimeData();
}

void ESC::parseRealtimeData()
{
  if (readBufferLength != 0x12 || readBuffer[0] != 0x32)
  {
    return;
  }
  tempMosfet = read_double_2(5, 10);
  tempMotor = read_double_2(7, 10);
  dutyCycle = read_double_2(9, 1000);
  erpm = read_double_4(11, 1);
  voltage = read_double_2(15, 10);
  fault = read_uint8(17);
  convert_erpm();

  //  Serial.print("Mosfet Temp: ");
  //  Serial.print(tempMosfet);
  //  Serial.println();
  //  Serial.print("voltage: ");
  //  Serial.print(voltage);
  //  Serial.println();
  //  Serial.print("faultoll: ");
  //  Serial.print(fault);
  //  Serial.println();
}

void ESC::getBalance()
{
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + ESC_CAN_ID;
  canMsg1.can_dlc = 0x03;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x4F;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();

  batchRead();
  parseBalance();
}

void ESC::parseBalance()
{
  if (readBufferLength != 0x25 || readBuffer[0] != 0x4F)
  {
    return;
  }
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
  //  Serial.print("PID Output: ");
  //  Serial.print(pidOutput);
  //  Serial.println();
  //  Serial.print("Pitch: ");
  //  Serial.print(pitch);
  //  Serial.println();
  //  Serial.print("Roll: ");
  //  Serial.print(roll);
  //  Serial.println();
}

#ifdef BMS_CAN_ID
void ESC::getBMS()
{
  struct can_frame canMsg1;
  canMsg1.can_id = (uint16_t(CAN_PACKET_PROCESS_SHORT_BUFFER) << 8) + BMS_CAN_ID;
  canMsg1.can_dlc = 0x03;
  canMsg1.data[0] = BALANCE_BUDDY_CAN_ID;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x04;

  CAN.beginExtendedPacket(canMsg1.can_id, canMsg1.can_dlc);
  CAN.write(canMsg1.data, canMsg1.can_dlc);
  CAN.endPacket();

  batchRead();
  parseBMS();
}

void ESC::parseBMS()
{
  if (readBufferLength != 0x32 || readBuffer[0] != 0x04)
  {
    return;
  }
  packVoltage = read_double_4(1, 1000);
  packCurrent = read_double_4(5, 1000);
  packSoC = read_uint8(9);
  cellVoltageHigh = read_double_4(10, 1000);
  cellVoltageAverage = read_double_4(14, 1000);
  cellVoltageLow = read_double_4(18, 1000);
  cellVoltageMisMatch = read_double_4(22, 1000);
  // there's more that I don't feel like adding now

  //  Serial.println("BMS Data:");
  //  Serial.println(packVoltage);
  //  Serial.println(packCurrent);
  //  Serial.println(packSoC);
  //  Serial.println(cellVoltageHigh);
  //  Serial.println(cellVoltageAverage);
  //  Serial.println(cellVoltageLow);
  //  Serial.println(cellVoltageMisMatch);
}
#endif

uint8_t ESC::read_uint8(uint8_t pos)
{
  return readBuffer[pos];
}

uint16_t ESC::read_uint16(uint8_t pos)
{
  return ((uint16_t)readBuffer[pos] << 8) + ((uint16_t)readBuffer[pos+1]);
}

uint32_t ESC::read_uint32(uint8_t pos)
{
  return ((uint32_t)readBuffer[pos] << 24) + ((uint32_t)readBuffer[pos+1] << 16) + ((uint32_t)readBuffer[pos+2] << 8) + ((uint32_t)readBuffer[pos+3]);
}

double ESC::read_double_2(uint8_t pos, double scale)
{
  if (scale == 0.0)
    scale = 1.0;
  return int16_t(((int16_t)readBuffer[pos] << 8) + ((int16_t)readBuffer[pos+1])) / scale;
}

double ESC::read_double_4(uint8_t pos, double scale)
{
  if (scale == 0.0)
    scale = 1.0;
  return (((int32_t)readBuffer[pos] << 24) + ((int32_t)readBuffer[pos+1] << 16) + ((int32_t)readBuffer[pos+2] << 8) + ((int32_t)readBuffer[pos+3])) / scale;
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

void ESC::batchRead()
{

  responsesLength = 0;
  for (int j = 0; j < 1000 && responsesLength < 10; j++)
  {
    int packetSize = CAN.parsePacket();
    if (packetSize)
    {
      responses[responsesLength].can_id = CAN.packetId();
      responses[responsesLength].can_dlc = packetSize;
      for (int i = 0; i < packetSize; i++)
      {
        responses[responsesLength].data[i] = CAN.read();
      }
      responsesLength++;
    }
    delayMicroseconds(10);
  }

  //  for(int i = 0; i < responsesLength; i++){
  //    printFrame(&responses[i]);
  //  }

  // Clear buffer
  readBufferLength = 0;
  for (int i = 0; i < 50; i++)
  {
    readBuffer[i] = 0; //(Not really necessary ?)
  }
  readBufferInfoLength = 0;
  for (int i = 0; i < 8; i++)
  {
    readBufferInfo[i] = 0; //(Not really necessary ?)
  }
  // Convert can frames to full buffer
  for (int i = 0; i < responsesLength; i++)
  {
    if (responses[i].can_id == ((uint16_t)CAN_PACKET_FILL_RX_BUFFER << 8) + BALANCE_BUDDY_CAN_ID)
    {
      for (int j = 1; j < responses[i].can_dlc; j++)
      {
        readBuffer[responses[i].data[0] + j - 1] = responses[i].data[j];
        readBufferLength++;
      }
    }
    else if (responses[i].can_id == ((uint16_t)CAN_PACKET_PROCESS_RX_BUFFER << 8) + BALANCE_BUDDY_CAN_ID)
    {
      for (int j = 0; j < responses[i].can_dlc; j++)
      {
        readBufferInfo[j] = responses[i].data[j];
        readBufferInfoLength++;
      }
    }
  }

  //  for(int i = 0; i < readBufferLength; i++){
  //    Serial.print(readBuffer[i],HEX);
  //    Serial.print(" ");
  //  }
  //  Serial.println();
  //
  //  for(int i = 0; i < readBufferInfoLength; i++){
  //    Serial.print(readBufferInfo[i],HEX);
  //    Serial.print(" ");
  //  }
  //  Serial.println();

  // TODO: Proper read validation with checksum, for now we just compare lengths, can improove if data is error prone.
  uint16_t supposedLength = ((uint16_t)readBufferInfo[2] << 8) + readBufferInfo[3];
  if (readBufferLength != supposedLength)
  {
    readBufferLength = 0;
    readBufferInfoLength = 0;
  }
}

void ESC::convert_erpm()
{
  rpm = erpm / (MOTOR_POLES / 2);
  speed_ms = rpm * WHEEL_DIAMETER_MM * PI / 1000 / 60;
}
