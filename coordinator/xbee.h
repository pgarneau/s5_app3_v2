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

#define AT_MIN_SIZE 4
#define FRAME

#define AT_COMMAND_ID 0x09

void initXbee(char *panId);
void readXbee();
#endif
