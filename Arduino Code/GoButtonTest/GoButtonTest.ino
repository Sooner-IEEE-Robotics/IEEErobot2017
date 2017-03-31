#include "SoonerColorduinoMaster.h"

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;

//Go Button
int goButtonPin = 2;

//Colorduino
SoonerColorduinoMaster scm;

void setup() 
{	
	//Go Button
	pinMode(goButtonPin, INPUT);
	//Pull down resistor to be safe
	digitalWrite(goButtonPin, LOW);
	
	//Comms pins
	pinMode(E, OUTPUT);//bit 0        //output pins for the states
	pinMode(F, OUTPUT);//bit 1
	pinMode(G, OUTPUT);//bit 2
	pinMode(moving, INPUT);  //low when moving, high when not moving
	pinMode(instruct, OUTPUT);  //low when info is valid, high when info is not valid
	pinMode(indicator, OUTPUT); //indicator led
	digitalWrite(instruct, HIGH);  //tells the other board that the info is not ready
	digitalWrite(indicator, LOW);  //pin 13 LED indicator
	digitalWrite(E, 0);            //initializes the state as state 0
	digitalWrite(F, 0);
	digitalWrite(G, 0);

	//Try multiple times to set the A7 LED
	scm.setPixelYellow(6,0);	
	delay(10000);
	scm.setPixelYellow(6,0);
	
	while(digitalRead(goButtonPin) == LOW)
	{
		scm.setPixelYellow(6,0);
		delay(50);
	}
	delay(1000);
}

void loop() 
{
	if(digitalRead(goButtonPin) == HIGH)
	{
		scm.setPixelRed(6,0);
	}
	else
	{
		scm.setPixelYellow(6,0);
	}
	
	delay(100);
}
