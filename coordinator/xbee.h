#ifndef XBEE_H
#define XBEE_H

#define START 0x7E
#define START_INDEX 0

#define LENGTH_MSB_INDEX 1
#define LENGTH_LSB_INDEX 2

#define COMMAND_SIZE 2
#define COMMAND_ID_INDEX 3

#define FRAME_ID 0x01
#define FRAME_ID_INDEX 4

#define MIN_FRAME_DATA_SIZE 4
#define MIN_COMMAND_FRAME_SIZE 8
#define MIN_COMMAND_REQUEST_FRAME_DATA_SIZE 15
#define MIN_COMMAND_REQUEST_FRAME_SIZE 19

#define AT_MIN_SIZE 4
#define TRANSMIT_MIN_SIZE   14
#define FRAME

#define AT_COMMAND_ID 0x09
#define ZIGBEE_TRANSMIT_ID 0x10
#define MAC_SIZE 8

// Frame types
#define AT_QUEUE_COMMAND 0x09
#define TRANSMIT_REQUEST 0x10
#define REMOTE_AT_COMMAND 0x17
#define AT_COMMAND_RESPONSE 0x88
#define MODEM_STATUS 0x8A
#define TRANSMIT_STATUS 0x8B
#define RECEIVE_PACKET 0x90

void initXbee(char *panId, int *readData);
void readXbee(int *readData);
void sendCommandRequest(char *command, char *mac, int parameter, int dataLength);
#endif
