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

//Colorduino
SoonerColorduinoMaster scm;

//The current orientation of the robot. This variable must be preserved so we know where to go.
/*
NORTH: 0, EAST: 1, SOUTH: 2, WEST: 3
*/
int currentOrientation = 0;

//Position of the robot
int CURRENT_ROW = 6, CURRENT_COL = 0;

//Round 3 flag just in case
bool isRoundThree = false;

//Directions for the cache
QueueList<byte> mapQuest;

void initBoard()
{
	for(int i = 0; i < 49; ++i)
	{
		if((i/7) == 6 && (i%7) == 0)
		{
			scm.setPixelYellow(0, 6); //Update the Colorduino accordingly
		}
	}
}

//*********************CACHE FINDING**************************//
/*
//Part 1
1. Back up
2. inch forward
3. Left
4. inch forward
5. Left
6. Check for Metal
7. If Metal = True -> Part 2, else Part 3
*/
void setupPartOne()
{
	mapQuest.push(7);
	mapQuest.push(6);
	mapQuest.push(2);
	mapQuest.push(6);
	mapQuest.push(2);
}

/**
//*********CACHE CLOSED LOOP**********
1. inch forward
2. Arm (up and down)
3. inch forward
4. Take picture
5. Back up
*/

/*
//Part 2
1. Undo Left Turn
2-6. Cache Closed Loop
7. Back up
8. inch forward
9. Right
10. Motion Complete!
*/
void setupPartTwo()
{
	mapQuest.push(5); //Enter cache mode
	mapQuest.push(2); //Undo left
	mapQuest.push(1);
	mapQuest.push(3);//Arm
	mapQuest.push(1);
	mapQuest.push(4);//Camera
	mapQuest.push(6);
	mapQuest.push(5);//Exit cache mode
	mapQuest.push(7);//Back
	mapQuest.push(6);
	mapQuest.push(3);//Right
}

/*
//Part 3
1. inch forward
2. Left
3-7. Cache Closed Loop
8. inch forward
9. Left
10. Forward
11. Motion Complete!!
*/
void setupPartThree()
{
	mapQuest.push(6);
	mapQuest.push(2);//Left
	mapQuest.push(5); //Enter cache mode
	mapQuest.push(1);
	mapQuest.push(3);//Arm
	mapQuest.push(1);
	mapQuest.push(4);//Camera
	mapQuest.push(6);
	mapQuest.push(5);//Exit cache mode
	mapQuest.push(6);
	mapQuest.push(2);//Left
	mapQuest.push(1);//Forward
}

//*********************END CACHE FINDING**************************//

void setup() 
{
	int startSquare = scm.setPixelYellow(0, 6);
	
	//int startSquare = scm.setPixelRed(2, 2);
	
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
	
	initBoard();
	
	//Metal Detection
	pinMode(metalDetectorPin, INPUT);
	
	//Obstacle Detection
	pinMode(sharpSensorPin, INPUT);
	
	Serial.begin(9600);
	
	CURRENT_COL = 1;
	CURRENT_ROW = 5;
		
	delay(10000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0, lastCommand = 0;
	bool commandApproved = true;
	int obstacleLocation;
	
	setupPartOne();
	
	//Test part 2
	//setupPartTwo();
	
	//Test Part 3
	setupPartThree();
	
	//Run code repeatedly based on what Google Maps tells us to do.
	while(mapQuest.count() > 0)
	{
		while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
		{ 
			// Do whatever here while we wait 
		}
		
		//Cache Operations
		command = mapQuest.pop();
		
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
		
		//Update lastCommand for future calculations
		lastCommand = command;
	}
	
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}

