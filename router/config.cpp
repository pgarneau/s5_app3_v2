#include "config.h"
#include "mbed.h"

LocalFileSystem local("local");

void getConfig(char *panId, char *data) {
    FILE *fp = fopen("/local/config.txt", "r");
    if (fp == NULL) {
        printf("Can't open this file \n");
        exit(1);
    }
    
    for (uint8_t index = 0; index < 2; index++) {
        if (index == 0) {
            if (fgets(panId, PANID_BUFFER_SIZE, fp) == NULL)
                break;
                    
            printf("Found PanId: %s\n\r", panId);
        }
        else if (index == 1) {
            if (fgets(data, DATA_BUFFER_SIZE, fp) == NULL)
                break;
            
            printf("Found data: %s\n\r", data);
        }
    }
    fclose(fp);
}
