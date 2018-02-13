#include "mbed.h"
#include "config.h"
#include "xbee.h"
#include "EthernetInterface.h"
#include "Websocket.h"

DigitalOut led1(LED1);
Serial pcc(USBTX, USBRX);

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
	pcc.printf("TRYING TO OPEN FILE \n\r");
	FILE *fp = fopen("/local/config.txt", "r");
	pcc.printf("OPENED THE FILE \n\r");
	fscanf(fp, "%d %s", &panId, server);
	pcc.printf("PanId: %d\n\r", panId);
	pcc.printf("Server: %s\n\r\r", server);
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
	// Websockets qui fonctionnent pas
//	EthernetInterface eth;
//	eth.set_network("10.43.138.170", "255.255.255.0", "10.43.138.1");
//	eth.connect();
//	printf("IP Address is %s\n\r", eth.get_ip_address());
//	Websocket ws("ws://10.43.157.181:8000/", &eth);
//	bool resulting = ws.connect();
//	char recv[128];

//	int allo = ws.send("ALLO CEST PHIL");
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
		// readData[15] -> 0 = angle, 1 = button
		// readData[16] -> result
		printf("RECEIVED INPUT FROM: %x CONTAINING: %x\n\r", readData[15], readData[16]);
    }
}
