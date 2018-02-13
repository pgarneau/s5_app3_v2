#include "xbee.h"
#include "mbed.h"

Serial xbee(p13, p14);
Serial pc(USBTX, USBRX);
DigitalOut xbeeResetPin(p8);
const char MAC_MASK[]={0,0,0,0,0,0,0,0};
int swag[128];

// Extrait le most significant byte
uint8_t get_MSB(uint16_t data) {
    uint8_t MSB = (data >> 8) & 0x00FF;
    
    return MSB;
}

// Extrait le least significant byte de la longueur
uint8_t get_LSB(uint16_t data) {
    uint8_t LSB = data & 0x00FF;
    
    return LSB;
}

// Reset du Xbee pour qui'il s'active
void resetXbee() {
    xbeeResetPin = 0;
    wait_ms(410);
    xbeeResetPin = 1;
}

// Lecture du port serie provenant du xbee
void readXbee(int *readData) {
	int counter = 0;
	memset(readData, 0, 128);
	
	while (!xbee.readable()) {
	}
	while (xbee.readable()) {
		uint8_t received_char = xbee.getc();
		readData[counter] = received_char;
		if (received_char == 0x7e) {
			pc.printf("\n\rReceived: \n\r");
		}
		pc.printf("%02X ", received_char);
		counter++;
		wait_ms(10);
	}
	pc.printf("\n\rDone receiving\n\r");
}

// Lecture du port serie provenant du xbee et qui confirme que la commande envoyee a bien ete envoyee
void confirmCommand(int command) {
	int counter = 0;
	memset(swag, 0, 128);
	
	while (!xbee.readable()) {
	}
	while (xbee.readable()) {
		uint8_t received_char = xbee.getc();
		swag[counter] = received_char;
		//pc.printf("%02X ", received_char);
		counter++;
		wait_ms(5);
	}
	if (swag[3] == command) {
		pc.printf("VALID\n\r");
	} else {
		pc.printf("INVALID\n\r");
	}
}


// Envoie du data vers xbee par port serie
void sendXbee(char *data, int dataLength) {
    pc.printf("Sending characters: ");
    
    for (uint8_t index = 0; index < dataLength; index++) {
        xbee.putc(data[index]);
        pc.printf("%02X ", data[index]);
        wait_ms(1);
    }
	pc.printf("\n\r");
   
}

// Calcul du checksum pour mettre dans la trame
void setChecksum(char *commandFrame) {
    uint16_t length = ((uint16_t)commandFrame[LENGTH_MSB_INDEX] << 8) + commandFrame[LENGTH_LSB_INDEX];
    
    char sum = 0;
    int max = length + 3;
    
    for (int index = 3; index < max; index++) {
        sum += commandFrame[index];
    }
    
    commandFrame[max] = 0xff - sum;
}

// Envoie d'une commande normale
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
	confirmCommand(0x88);
}

// Envoie d'un remote AT command request
void sendCommandRequest(char *command, char *mac, int parameter, int dataLength) {
	char commandFrame[128];
	commandFrame[START_INDEX] = START;
	commandFrame[LENGTH_MSB_INDEX] = get_MSB(dataLength + MIN_COMMAND_REQUEST_FRAME_DATA_SIZE);
	commandFrame[LENGTH_LSB_INDEX] = get_LSB(dataLength + MIN_COMMAND_REQUEST_FRAME_DATA_SIZE);
	commandFrame[COMMAND_ID_INDEX] = REMOTE_AT_COMMAND;
	commandFrame[FRAME_ID_INDEX] = 0x00;
	memcpy(&commandFrame[5], mac, 8);
	commandFrame[13] = 0xFF;
	commandFrame[14] = 0xFE;
	commandFrame[15] = 0x02;
	memcpy(&commandFrame[16], command, 2);
	commandFrame[18] = parameter;
	
	setChecksum(commandFrame);
	
	sendXbee(commandFrame, dataLength + MIN_COMMAND_REQUEST_FRAME_SIZE);
	wait_ms(25);
	//confirmCommand(0x97);
}

// Resultat d'un merge conflict
void sendRemoteCommand() {
	char commandFrame[128];
	commandFrame[START_INDEX] = START;
}

// initialisation du xbee avec le bon PANID
void initXbee(char *panId, int *readData) {
    resetXbee();
	readXbee(readData);
    sendCommand("ID", panId, 8);
    sendCommand("WR", 0, 0);
    sendCommand("AC", 0, 0);
}
