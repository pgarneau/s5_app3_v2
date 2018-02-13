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
	fscanf(fp, "%d %f", &panId, &echantillonage);
	printf("PanId: %d\n\r", panId);
	printf("Server: %f\n\r\r", echantillonage);
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
	
	void getBtnStatus(){
		if(btn==1){
			pcc.printf("BTN ON ::\n\r");
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
	
	initXbee(panIdChar);
 pcc.printf("Avant send transmit\n\r");
	//char buffer_send[21] = {0x7E ,0x00 ,0x11 ,0x10 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFE ,0x00 ,0x00 ,0x61 ,0x73 ,0x64,0xB9};
	//sendXbee(buffer_send,21);

	//readXbee();
	int a = sizeof(PUSSY);
	 pcc.printf("Apres send transmit, sizeof = %i\n\r",a);
		
		sendTransmit(PUSSY,a);
		readXbee();
    while (true) {
			

		for(int i=0; i<function_cpt; ++i){
			myFunctionArray[i]();			
		}
			
        led1 = !led1;
		wait(echantillonage);
    }
}
