#include <QueueList.h>
#include "SoonerColorduinoMaster.h"

//Threshold to determine if there is an obstacle or not
#define OBSTACLE_THRESHOLD 100

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;

//Metal Detector
int metalDetectorPin = 4;

//IR Sensor (Obstacle Detection)
int sharpSensorPin = A0; //This is an analog sensor

void setup() 
{
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
	
	//Metal Detection
	pinMode(metalDetectorPin, INPUT);
	
	Serial.begin(9600);
	
	delay(10000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0;
	bool commandApproved = true;
	int obstacleLocation;

	//VERY IMPORTANT, waits until the RobotManager is no longer actively moving the robot to assert more instructions
	while(digitalRead(moving) == LOW)          
	{ 
		// Do whatever here while we wait 
	}
	
	//Cache Operations
	command = 6;
	
	//Bitshifting is OK here for some reason
	digitalWrite(E, command & 1); 
	digitalWrite(F, command>>1 & 1);
    digitalWrite(G, command>>2 & 1);
	
	//Wait for the lines to actually be asserted with digitalWrite
	delay(10);
	
	//Tell RobotManager that the instructions are ready to execute
	digitalWrite(instruct, 0);
	
	while(digitalRead(moving) == 1) {}   //VERY IMPORTANT, waits until RobotManager realizes that it has incoming data
	
	//Disable the instruct line
	digitalWrite(instruct, 1);
	
	//Wait for the instruct line to switch to the correct value
	delay(10);
	
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the RobotManager
}

