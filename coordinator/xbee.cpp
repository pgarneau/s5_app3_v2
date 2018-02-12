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

void sendXbee(char *data) {
    pc.printf("Sending characters: \n\r");
    
    for (uint8_t index = 0; index < sizeof(data); index++) {
        xbee.putc(data[index]);
        pc.printf("%02X ", data[index]);
        wait_ms(25);
    }
    pc.printf("\n\r");
}

void setChecksum(char *commandFrame) {
    uint16_t length = ((uint16_t)commandFrame[1] << 8) + commandFrame[2];
    
    char sum = 0;
    int max = length + 3;
    
    for (int index = 3; index < max; index++) {
        sum += commandFrame[index];
    }
    
    commandFrame[max] = 0xff - sum;
}

void sendCommand(char *command, char *data) {
    char commandFrame[128];
    commandFrame[0] = START;
    commandFrame[1] = get_MSB(sizeof(data) + AT_MIN_SIZE);
    commandFrame[2] = get_LSB(sizeof(data) + AT_MIN_SIZE);
    commandFrame[3] = AT_COMMAND_ID;
    commandFrame[4] = FRAME_ID;
    memcpy(&commandFrame[5], command, CMD_SIZE);
    memcpy(&commandFrame[6], data, sizeof(data));
    
    setChecksum(commandFrame);
    
    sendXbee(commandFrame);  
}

void initXbee(char *panId) {
    resetXbee();
    sendCommand("ID", panId);
    sendCommand("WR", 0);
    sendCommand("AC", 0);
}
