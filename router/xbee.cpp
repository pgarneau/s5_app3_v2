#include "xbee.h"
#include "mbed.h"

Serial xbee(p13, p14);
Serial pc(USBTX, USBRX);
Semaphore flashyflashy(1);
DigitalOut xbeeResetPin(p8);
DigitalOut errorLed(p20);
const char MAC_MASK[]={0,0,0,0,0,0,0,0};
int swag[128];
Thread thread_error;


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


// thread attendant un signal pour faire flasher la led.
void flashLed() {
	while(1){
	Thread::signal_wait(0x1);
	// pour ne pas saturer la led, on met une semaphore pendant le blink de 2 secondes
	flashyflashy.wait();

	errorLed = 1;
	Thread::wait(1000);
	errorLed = 0;
	Thread::wait(1000);		
	flashyflashy.release();
	}
}


void readXbee(int command) {
	pc.printf("\n\r Received caractere \n\r");
	int counter = 0;
	memset(swag, 0, 128);
	
	while (!xbee.readable()) {
	}
	while (xbee.readable()) {
		uint8_t received_char = xbee.getc();
		swag[counter] = received_char;
		pc.printf("%02X ", received_char);
		counter++;
		wait_ms(5);
	}
			
	
	/* frame typesss :
//frame response type
#define AT_COMMAND_RESPONSE 0x88
#define MODEM_STATUS 0x8A
#define TRANSMIT_STATUS 0x8B
#define RECEIVE_PACKET 0x90 */	

// selon la trame recu, on envoie lerreur et fait flasher la LED	
switch(swag[3]) {
 
	
	case AT_COMMAND_RESPONSE : // si command est 0x88
		if(swag[7]!=0x00){
			thread_error.signal_set(0x1);
			 pc.printf("\n\r\n\r********ERROR AT COMMAND RESPONSE\n\r");				
		}
		  break;
		// comme il y a plusieurs type de modem status on ajoute
	case  MODEM_STATUS: // si commande est 0x8A
		if(swag[4]!=0x00 ||swag[4]!=0x02 ||swag[4]!=0x06||swag[4]!=0x07  ){		
			thread_error.signal_set(0x1);
			if(swag[4]==0x03){
				 pc.printf("\n\r\n\r********ERROR MODEM STATUS DISASSOCIATED\n\r");
			}
		}
		  break;
		
	case  TRANSMIT_STATUS: // si command est 0x8B
		if(swag[8]!= 0x00){
			thread_error.signal_set(0x1);
			 pc.printf("\n\r\n\r**ERROR TRANSMIT STATUS \n\r");
			
		}
		  break;

		
	case RECEIVE_PACKET : // si commande est 0x90

		  break;

   default : 
      pc.printf("\n\r\n\r aucune trame de reponse trouvee yolo \n\r");

}
	
}



void sendXbee(char *data, int dataLength) {
    pc.printf("\n\rSending START characters : \n\r");
    
    for (uint8_t index = 0; index < dataLength; index++) {
        xbee.putc(data[index]);
        pc.printf("%02X ", data[index]);
        wait_ms(25);
    }
		pc.printf("\n\rSending FINISH:: \n\r");
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

//envoie remote command selon la doc
void sendCommand(char *command, char *data, uint8_t dataLength) {
    char commandFrame[128];
    commandFrame[START_INDEX] = START;
    commandFrame[LENGTH_MSB_INDEX] = get_MSB(dataLength + MIN_FRAME_DATA_SIZE);
    commandFrame[LENGTH_LSB_INDEX] = get_LSB(dataLength + MIN_FRAME_DATA_SIZE);
    commandFrame[COMMAND_ID_INDEX] = AT_COMMAND_ID;
    commandFrame[FRAME_ID_INDEX] = FRAME_ID;
    memcpy(&commandFrame[5], command, COMMAND_SIZE);
	//copier le data dynamiquement pour pouvoir avoir des trames de grandeurs differentes

    memcpy(&commandFrame[7], data, dataLength);
    
    setChecksum(commandFrame);
    
    sendXbee(commandFrame, dataLength + MIN_COMMAND_FRAME_SIZE);  
}

//envoie transmit command selon la doc
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
	
	
//copier le data dynamiquement pour pouvoir avoir des trames de grandeurs differentes
	memcpy(&commandFrame[17], data, dataLength);
	int commandSize = 17 + dataLength;

	setChecksum(commandFrame);
	commandSize++;
	sendXbee(commandFrame, commandSize);  
	readXbee(0x8B);
    
}


//envoie request command selon la doc
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
	//initilaisation des threads et des commandes
	thread_error.start(flashLed);
    resetXbee();
	readXbee(0x8A);
    sendCommand("ID", panId, 8);
	readXbee(0x88);
    sendCommand("WR", 0, 0);
	readXbee(0x88);
    sendCommand("AC", 0, 0);
	readXbee(0x88);
}
