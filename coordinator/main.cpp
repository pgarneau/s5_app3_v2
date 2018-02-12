#include "mbed.h"
#include "config.h"
#include "xbee.h"

DigitalOut led1(LED1);
Semaphore test;

char data[DATA_BUFFER_SIZE - 1] = {0};

int panId = 0;
char panIdChar[8];
char server[16] = {0};

void convertPanId(int panId) {
	for (int i = 0; i < 8; i++) {
		panIdChar[i] = (panId >> (56 - 8 * i)) & 0xFF;
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
	
int main() {
	readConfig();

	initXbee(panIdChar);
	
    while (true) {
        led1 = !led1;
        wait(0.5);
    }
}
