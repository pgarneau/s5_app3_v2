#include "xbee.h"
#include "mbed.h"

Serial xbee(p13, p14);
Serial pc(USBTX, USBRX);
DigitalOut xbeeResetPin(p8);
const char MAC_MASK[]={0,0,0,0,0,0,0,0};

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
	pc.printf("\n\rDone receiving\n\r");
}



void sendXbee(char *data, int dataLength) {
    pc.printf("Sending characters: ");
    
    for (uint8_t index = 0; index < dataLength; index++) {
        xbee.putc(data[index]);
        pc.printf("%02X ", data[index]);
        wait_ms(25);
    }
   
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
void sendTransmit(char *data, int dataLength) {
	 char commandFrame[128];
    commandFrame[START_INDEX] = START;
    commandFrame[LENGTH_MSB_INDEX] = get_MSB(dataLength +TRANSMIT_MIN_SIZE);
    commandFrame[LENGTH_LSB_INDEX] = get_LSB(dataLength +TRANSMIT_MIN_SIZE);
    commandFrame[COMMAND_ID_INDEX] = ZIGBEE_TRANSMIT_ID;
    commandFrame[FRAME_ID_INDEX] = FRAME_ID;
	  memcpy(&commandFrame[5], MAC_MASK, MAC_SIZE);
		commandFrame[13]=0xFF;
		commandFrame[14]= 0xFE;
		commandFrame[15]= 0x00;
		commandFrame[16]= 0x00;
	
	
		for(int i = 0; i<dataLength; ++i){
		pc.printf("meme copy index %i = %c \n\r",i,data[i]);
	}
		memcpy(&commandFrame[17], data, dataLength);
	int commandSize = 17 + dataLength;
	for(int i = 0; i<commandSize; ++i){
 pc.printf("index[ %i ] = %02x \n\r",i,commandFrame[i]);
	}
	  pc.printf("le data length envoyer is %i + %i\n\r",dataLength , MIN_COMMAND_FRAME_SIZE);
		setChecksum(commandFrame);
		commandSize++;
    sendXbee(commandFrame, commandSize);  
    
}

void sendCommandRequest(char *command, char *mac, int parameter, int dataLength) {
	char commandFrame[128];
	commandFrame[START_INDEX] = START;
	commandFrame[LENGTH_MSB_INDEX] = get_MSB(dataLength + MIN_COMMAND_REQUEST_FRAME_DATA_SIZE);
	commandFrame[LENGTH_LSB_INDEX] = get_LSB(dataLength + MIN_COMMAND_REQUEST_FRAME_DATA_SIZE);
	commandFrame[COMMAND_ID_INDEX] = REMOTE_AT_COMMAND;
	commandFrame[FRAME_ID_INDEX] = FRAME_ID;
	memcpy(&commandFrame[5], mac, 8);
	commandFrame[13] = 0xFF;
	commandFrame[14] = 0xFE;
	commandFrame[15] = 0x02;
	memcpy(&commandFrame[16], command, 2);
	commandFrame[18] = parameter;
	
	setChecksum(commandFrame);
	
	sendXbee(commandFrame, dataLength + MIN_COMMAND_REQUEST_FRAME_SIZE);
}

void sendRemoteCommand() {
	char commandFrame[128];
	commandFrame[START_INDEX] = START;
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
