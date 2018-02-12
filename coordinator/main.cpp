#include "mbed.h"
#include "config.h"
#include "xbee.h"

DigitalOut led1(LED1);
Semaphore test;

char panId[PANID_BUFFER_SIZE - 1] = {0};
char data[DATA_BUFFER_SIZE - 1] = {0};

int main() {
    getConfig(panId, data);
    
    for (int i = 0; i < sizeof(panId); i++) {
        printf("%d\n\r", panId[i]);
    }
    
    printf("%d\n\r", sizeof(panId));
    printf("%d\n\r", sizeof(data));
	
	
    while (true) {
        led1 = !led1;
        wait(0.5);
    }
}
