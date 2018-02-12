#include "xbee.h"
#include "mbed.h"

Serial xbee(p13, p14);
Serial pc(USBTX, USBRX);
DigitalOut xbeeResetPin(p8);

uint8_t get_MSB(uint16_t data) {
    uint8_t MSB = (data >> 8) & 0x00FF;
    
    return MSB;
}

uint8_t get_LSB(uint16_t data) {
    uint8_t LSB = data & 0x00FF;
    
    return LSB;
}

void resetXbee() {
    xbeeResetPin = 0;
    wait_ms(400);
    xbeeResetPin = 1;
}

void readXbee() {
	while (!xbee.readable()) {
	}
	while (xbee.readable()) {
		uint8_t received_char = xbee.getc();
		if (received_char == 0x7e) {
			pc.printf("\n\rReceived: \n\r");
		}
		pc.printf("%02X ", received_char);
		wait_ms(50);
	}
	pc.printf("\n\r\n\r");
}

void sendXbee(char *data, int dataLength) {
    pc.printf("Sending characters: ");
    
    for (uint8_t index = 0; index < dataLength; index++) {
        xbee.putc(data[index]);
        pc.printf("%02X ", data[index]);
        wait_ms(25);
    }
    pc.printf("\n\r");
}

void setChecksum(char *commandFrame) {
    uint16_t length = ((uint16_t)commandFrame[LENGTH_MSB_INDEX] << 8) + commandFrame[LENGTH_LSB_INDEX];
    
    char sum = 0;
    int max = length + 3;
    
    for (int index = 3; index < max; index++) {
        sum += commandFrame[index];
    }
    
    commandFrame[max] = 0xff - sum;
}

void sendCommand(char *command, char *data, uint8_t dataLength) {
    char commandFrame[128];
    commandFrame[START_INDEX] = START;
    commandFrame[LENGTH_MSB_INDEX] = get_MSB(dataLength + MIN_FRAME_DATA_SIZE);
    commandFrame[LENGTH_LSB_INDEX] = get_LSB(dataLength + MIN_FRAME_DATA_SIZE);
    commandFrame[COMMAND_ID_INDEX] = AT_COMMAND_ID;
    commandFrame[FRAME_ID_INDEX] = FRAME_ID;
    memcpy(&commandFrame[5], command, COMMAND_SIZE);
    memcpy(&commandFrame[7], data, dataLength);
    
    setChecksum(commandFrame);
    
    sendXbee(commandFrame, dataLength + MIN_COMMAND_FRAME_SIZE);  
}

void initXbee(char *panId) {
    resetXbee();
	readXbee();
    sendCommand("ID", panId, 8);
	readXbee();
    sendCommand("WR", 0, 0);
	readXbee();
    sendCommand("AC", 0, 0);
	readXbee();
}
