#include "mbed.h"
#include "config.h"
#include "xbee.h"

DigitalOut led1(LED1);

Ticker ticker;

	
char data[DATA_BUFFER_SIZE - 1] = {0};

bool ledOn = false;
int panId = 0;
char panIdChar[8];
char macAddressChar[8];
char server[16] = {0};
int readData[128];

void convertPanId(int panId) {
	for (int i = 0; i < 8; i++) {
		panIdChar[i] = (panId >> (56 - 8 * i)) & 0xFF;
	}
}

void getMacAddress(int *readData) {
	for (int i = 4; i < 12; i++) {
		macAddressChar[i - 4] = readData[i];
	}
}

void readConfig() {
	FILE *fp = fopen("/local/config.txt", "r");
	fscanf(fp, "%d %s", &panId, server);
	printf("PanId: %d\n\r", panId);
	printf("Server: %s\n\r\r", server);
	fclose(fp);
	convertPanId(panId);
}

void flashLed() {
	if (!ledOn) {
		sendCommandRequest("D4", macAddressChar, 0x05, 1);
		ledOn = !ledOn;
	} else {
		sendCommandRequest("D4", macAddressChar, 0x04, 1);
		ledOn = !ledOn;
	}
}
	
int main() {
	// Read config from file
	readConfig();

	// Initialize the Xbee
	initXbee(panIdChar, readData);
	
	//Get MAC Address
	readXbee(readData);
	printf("MAC: %x %x %x %x %x %x %x %x\n\r", readData[4], readData[5], readData[6], readData[7], readData[8], readData[9], readData[10], readData[11]);
	getMacAddress(readData);
	
	// Attach ticker to function making LED flash on router
	ticker.attach(flashLed, 1);
	
    while (true) {
        readXbee(readData);
		printf("RECEIVED INPUT FROM: %x CONTAINING: %x\n\r", readData[15], readData[16]);
    }
}
