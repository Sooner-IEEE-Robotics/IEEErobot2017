#include "LED.h"
#include "Comms.h"

LED led(13);

//State Machine
enum State{OFF, ON, BLINK, IDLE_STATE}; //List the states here
State stateMachine = IDLE_STATE;

//Communications
Comms comm;

//Get message from interrupt
void getMessage()
{
	int command = comm.receiveBitsAndEncode();
	
	Serial.print("CMD: ");
	Serial.println(command);
	
	if(command == 0)
	{
		stateMachine = OFF;
	}
	else if(command == 1)
	{
		stateMachine = ON;
	}
	else if(command == 2)
	{
		stateMachine = BLINK;
	}
	else
	{
		stateMachine = IDLE_STATE;
	}	
}

void setup()
{
	//Setup for LED
	led.setupPin();
	
	/*
	//Set this board to receive messages
	comm.setup3PinReceive(5, 6, 7);
	comm.setupReadyOut(8);
	
	//Be notified when a new command is here
	attachInterrupt(0, getMessage, CHANGE);
	*/
	
	Serial.begin(9600);
}

void loop()
{
	if(stateMachine == OFF)
	{
		led.off();
		delay(3000);
		
		stateMachine = IDLE_STATE;
	}
	else if(stateMachine == ON)
	{
		led.on();
		delay(3000);
		
		stateMachine = IDLE_STATE;
	}
	else if(stateMachine == BLINK)
	{
		unsigned long start = millis();
		
		while(millis() - start < 3000)
		{
			led.blink();
		}
		
		stateMachine = IDLE_STATE;
	}
	else
	{
		//Ready for next command
		Serial.write(1);
		if(Serial.available() >= 3)
		{
			getMessage();
		}
	}
	
	Serial.println(stateMachine);
	delay(100);
}