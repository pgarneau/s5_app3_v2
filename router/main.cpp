#include "mbed.h"
#include "config.h"
#include "xbee.h"

DigitalOut led1(LED1);
Semaphore test;
Serial pcc(USBTX, USBRX);

	
char data[DATA_BUFFER_SIZE - 1] = {0};
char PUSSY[] = {0x01,0x90};
int panId = 0;
char panIdChar[8];
float echantillonage =0;

//Accelero et btn

I2C i2c(p28, p27);
const int accAddr = 0x1D << 1;
const int accZRegisterMSB = 0x05;
const double PI = 3.14159265359;
const int uint14Max = 16383;
int intAngle;
DigitalIn btn(p21);
//Declaration des fonctions allant dans le tableau, plusieurs autres peuvent etre ajoutes
void calculateAngleAcc();
void getBtnStatus();

//Declaration du tableau de fonction 
/**** **** **** **** **** **** **** **** 
SECTION A CHANGER POUR AJOUTER DES FONCTIONS
******** **** **** **** **** **** **** **********************************/

void(*myFunctionArray[])(void) = {calculateAngleAcc,getBtnStatus};
const int function_cpt = 2;
/*********************************************************************/

void convertPanId(int panId) {
	for (int i = 0; i < 8; i++) {
		panIdChar[i] = (panId >> (56 - 8 * i)) & 0xFF;
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

float GetAxisAcc(int addr) {
    uint8_t data[2];
    char axisRegister[1] = {(char)addr};
    i2c.write(accAddr, axisRegister, 1, true);
    i2c.read(accAddr, (char *)data, 2);

    int16_t axisAcc = (data[0] << 6) | (data[1] >> 2);
    if (axisAcc > uint14Max/2)
        axisAcc -= uint14Max;
 
    return axisAcc / 4096.0;
}
int CalculateAngle(float z){
    if(z > 1){
        z = 1;
    }
    if(z < -1){
        z = -1;    
    }

    double angle = std::acos(z);

    if(angle > PI/2)
    {
        angle = PI - angle;
    }

    return (int)(angle*180/PI);
}

	int acceleroConfig()  {
		int resetAngle = 0;
		uint8_t initializeAcc[2] = {0x2A, 0x01};
		i2c.write(accAddr, (char *)initializeAcc, 2);
		
		

		
	}
	
	void calculateAngleAcc(){
			float zAxis = GetAxisAcc(accZRegisterMSB);
			intAngle = CalculateAngle(zAxis);
		if(intAngle>20){
		pcc.printf("Angle is %i ::\n\r",intAngle);
			char angle[] = {0x00,(char)intAngle};
			int a = sizeof(angle);
			sendTransmit(angle,a);
			
	}			
		}

	
	void getBtnStatus(){
		if(btn==1){
			pcc.printf("BTN ON ::\n\r");
			char bouton[] = {0x01,0x01};
				int a = sizeof(bouton);
			sendTransmit(bouton,a);
		}
	}
int main() {
	readConfig();
	acceleroConfig();
	
	initXbee(panIdChar);
 pcc.printf("Avant send transmit\n\r");
	//char buffer_send[21] = {0x7E ,0x00 ,0x11 ,0x10 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFE ,0x00 ,0x00 ,0x61 ,0x73 ,0x64,0xB9};
	//sendXbee(buffer_send,21);

	//readXbee();
	int a = sizeof(PUSSY);
	 pcc.printf("Apres send transmit, sizeof = %i\n\r",a);
		
		sendTransmit(PUSSY,a);
		//readXbee();
	
	pcc.printf("\n\r* * * * ** * * * * * * * * * * * * *\n\r DETECTION SECTION \n\r* * * * ** * * * * * * * * * * * * *\n\r");
    while (true) {
			

		for(int i=0; i<function_cpt; ++i){
			myFunctionArray[i]();			
		}
			
        led1 = !led1;
		wait(echantillonage);
    }
}
