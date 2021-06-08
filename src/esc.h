#pragma once
#include <CAN.h>
#include "config.h"

typedef enum
{
  CAN_PACKET_SET_DUTY = 0,
  CAN_PACKET_SET_CURRENT,
  CAN_PACKET_SET_CURRENT_BRAKE,
  CAN_PACKET_SET_RPM,
  CAN_PACKET_SET_POS,
  CAN_PACKET_FILL_RX_BUFFER,
  CAN_PACKET_FILL_RX_BUFFER_LONG,
  CAN_PACKET_PROCESS_RX_BUFFER,
  CAN_PACKET_PROCESS_SHORT_BUFFER,
  CAN_PACKET_STATUS,
  CAN_PACKET_SET_CURRENT_REL,
  CAN_PACKET_SET_CURRENT_BRAKE_REL,
  CAN_PACKET_SET_CURRENT_HANDBRAKE,
  CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
  CAN_PACKET_STATUS_2,
  CAN_PACKET_STATUS_3,
  CAN_PACKET_STATUS_4,
  CAN_PACKET_PING,
  CAN_PACKET_PONG,
  CAN_PACKET_DETECT_APPLY_ALL_FOC,
  CAN_PACKET_DETECT_APPLY_ALL_FOC_RES,
  CAN_PACKET_CONF_CURRENT_LIMITS,
  CAN_PACKET_CONF_STORE_CURRENT_LIMITS,
  CAN_PACKET_CONF_CURRENT_LIMITS_IN,
  CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN,
  CAN_PACKET_CONF_FOC_ERPMS,
  CAN_PACKET_CONF_STORE_FOC_ERPMS,
  CAN_PACKET_STATUS_5,
  CAN_PACKET_POLL_TS5700N8501_STATUS,
  CAN_PACKET_CONF_BATTERY_CUT,
  CAN_PACKET_CONF_STORE_BATTERY_CUT,
  CAN_PACKET_SHUTDOWN,
  CAN_PACKET_IO_BOARD_ADC_1_TO_4,
  CAN_PACKET_IO_BOARD_ADC_5_TO_8,
  CAN_PACKET_IO_BOARD_ADC_9_TO_12,
  CAN_PACKET_IO_BOARD_DIGITAL_IN,
  CAN_PACKET_IO_BOARD_SET_OUTPUT_DIGITAL,
  CAN_PACKET_IO_BOARD_SET_OUTPUT_PWM,
  CAN_PACKET_BMS_V_TOT,
  CAN_PACKET_BMS_I,
  CAN_PACKET_BMS_AH_WH,
  CAN_PACKET_BMS_V_CELL,
  CAN_PACKET_BMS_BAL,
  CAN_PACKET_BMS_TEMPS,
  CAN_PACKET_BMS_HUM,
  CAN_PACKET_BMS_SOC_SOH_TEMP_STAT
} CAN_PACKET_ID;

class ESC
{
private:
  struct can_frame
  {
    unsigned long can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    unsigned char can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
    unsigned char data[8] __attribute__((aligned(8)));
  };

  struct can_frame currentPacket;

  uint8_t readBuffer[50];
  uint8_t readBufferLength = 0;
  uint8_t readBufferInfo[8];
  uint8_t readBufferInfoLength = 0;

  bool requestSent = false;
  uint8_t step = 0;
  long lastRequestMillis = 0;
  unsigned char requestID;
  uint16_t expectedLength;
  uint16_t supposedLength = 0;

  void sendNextRequest();
  void getRealtimeData();
  void getBalance();

  void parseRequest();
  void parseRealtimeData();
  void parseBalance();
  void convert_erpm();

#ifdef BMS_CAN_ID
  void getBMS();
  void parseBMS();
#endif

  void startOver();
  void readCAN();
  void printFrame(struct can_frame *frame);

  uint8_t read_uint8(uint8_t pos);
  uint16_t read_uint16(uint8_t pos);
  uint32_t read_uint32(uint8_t pos);
  double read_double_2(uint8_t pos, double scale);
  double read_double_4(uint8_t pos, double scale);

public:
  // RT Data vars
  double tempMosfet;
  double tempMotor;
  double dutyCycle;
  double erpm;
  double voltage;
  uint8_t fault;
  double rpm;
  double speed_ms;

  // Balance vars
  double pidOutput;
  double pitch;
  double roll;
  uint32_t loopTime;
  double motorCurrent;
  double motorPosition;
  uint16_t balanceState;
  uint16_t switchState;
  double adc1;
  double adc2;

  // BMS vars
  double packVoltage;
  double packCurrent;
  uint8_t packSoC;
  double cellVoltageHigh;
  double cellVoltageAverage;
  double cellVoltageLow;
  double cellVoltageMisMatch;
  // there's more that I don't feel like adding now

  void setup();

  void loop();
};
