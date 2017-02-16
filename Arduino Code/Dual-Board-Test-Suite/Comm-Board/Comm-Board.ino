#include "Comms.h"

Comms comm;

int index = 0;
int messageArray[4] = {0,1,2,1};

int lastNotify = LOW;

void serialEvent()
{
	if(index < (sizeof(messageArray)/sizeof(messageArray[0])))
	{
		Serial.print(messageArray[index]);
		Serial.println(" is being sent.");
		
		comm.sendNumber(messageArray[index]);
		
		index++;
	}
	else
	{
		index = 0;
	}
	
	Serial.flush();
}

void setup()
{
	//Setup board to send 3 bit messageArray
	//comm.setup3PinSend(12, 11, 10);
	//comm.setupNotifyOut(9);
	//comm.setupReadyIn(7);
	
	Serial.begin(9600);
}

void loop()
{
	delay(250);
}