#include <QueueList.h>
#include "SoonerColorduinoMaster.h"

//Decide which endgame to pursue. 
/*
0 = None (The only way to win is to not play)
1 = A7 (Fall off the board)
2 = Cache (???)
3 = Cache, unless no caches found, then do Nothing
4 = Cache, unless no caches found, then do A7
*/
#define ENDGAME 2

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;

//Go Button
int goButtonPin = 2;

//Metal Detector
int metalDetectorPin = 13;

//Colorduino
SoonerColorduinoMaster scm;

//The current orientation of the robot. This variable must be preserved so we know where to go.
/*
NORTH: 0, EAST: 1, SOUTH: 2, WEST: 3
*/
int currentOrientation = 0;

//Are we trying to avoid obstacles right now?
bool activelyAvoidingObstacles = false;

//Position of the robot
int CURRENT_ROW = 6, CURRENT_COL = 0;

//Edge Management variables used to constrain new spirals
//If we are doing the 7x7 outer
//int MIN_COL = 1, MAX_COL = 6, MIN_ROW = 0, MAX_ROW = 6;

//If we are doing the 5x5 pattern
int MIN_COL = 2, MAX_COL = 5, MIN_ROW = 1, MAX_ROW = 5;

//Directions to follow in order to win
QueueList<byte> googleMaps;

//Finishing Instructions
QueueList<byte> mapQuest;

//Board, as a 1D array
//(i/7) = Row, (i%7) = Column
/*
0 = Unexplored
1 = Main Tunnel
2 = Dead End Tunnel
3 = Empty
4 = Start
5 = Obstacle
*/
int board[49];

//Variables to find the caches
bool foundCacheBefore = false;
bool openingCache = false;

void initBoard()
{
	for(int i = 0; i < 49; ++i)
	{
		if((i/7) == 6 && (i%7) == 0)
		{
			board[i] = 4; //A7 is the start
		}
		else
		{
			board[i] = 0;
		}
	}
}

void makeASquare()
{
	for(int ii = 0; ii < 4; ++ii)
	{
		for(int i = 0; i < 5; ++i)
		{
			googleMaps.push(1);
		}
		googleMaps.push(6);
		googleMaps.push(2);
	}
}

void cacheSequence()
{
	//Back off the square
	mapQuest.push(7);
	//General Cache Sequence
	mapQuest.push(5); //Enter the cache sequence
	mapQuest.push(1);//Drive up to the lid
	mapQuest.push(3);//Open the cache
	mapQuest.push(4);//Take a picture of the die
	//Go back to normal
	mapQuest.push(5);
	mapQuest.push(6);
}

void setup() 
{	
	//Go Button
	pinMode(goButtonPin, INPUT);
	digitalWrite(goButtonPin, HIGH); //Pull up resistor
	
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
	Serial.println("Calculating Route...");
	
	initBoard();
	
	//Setup the two sequences
	makeASquare();
	cacheSequence();
	
	//Reset the coordinates back to the real coordinates
	CURRENT_COL = 0;
	CURRENT_ROW = 6;
	currentOrientation = 1; //Reset this to east because we are actually facing that way.
	
	//Try multiple times to set the A7 LED
	scm.setPixelYellow(6,0);	
	delay(10000);
	scm.setPixelYellow(6,0);
	
	while(digitalRead(goButtonPin) == HIGH)
	{
		scm.setPixelYellow(6,0);
	}
	delay(1000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0;
	
	//********************MAIN GAME****************************//
	
	//Run code repeatedly based on what Google Maps tells us to do.
	while(googleMaps.count() > 0)
	{
		while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
		{ 
			// Do whatever here while we wait 
		}
		
		//Stops and waits for a bit
		delay(3000);

		//*****Go through the checklist of things to do before actually moving.****//
		if(digitalRead(metalDetectorPin) == HIGH && !(CURRENT_COL == 0 && CURRENT_ROW == 6))//If metal is found on the regular route, update map and display to colorduino.
		{
			int i = (CURRENT_ROW*7) + CURRENT_COL;
			board[i] = 1;
			
			//Update the colorduino to show the main tunnel
			scm.setPixelRed(CURRENT_ROW, CURRENT_COL);
			
			if(!foundCacheBefore)
			{
				openingCache = true;
				foundCacheBefore = true;
			}
		}
		
		//Gets the next command from the queue.
		//If we are trying to open the cache
		if(openingCache)
		{
			command = mapQuest.pop();
			
			//Return to normal if we have gone through the full sequence
			if(mapQuest.count() == 0)
			{
				openingCache = false;
			}
		}
		//Normal Operation
		else
		{
			command = googleMaps.pop();	
		}	
		
		//Bitshift to encode command for sending
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
		
		//Update Orientation and position based on the prior command
		if(command == 1)//We just drove straight
		{
			if(currentOrientation == 0) //North
			{
				CURRENT_ROW--;
			}
			else if(currentOrientation == 1) //East
			{
				CURRENT_COL++;
			}
			else if(currentOrientation == 2) //South
			{
				CURRENT_ROW++;
			}
			else //West
			{
				CURRENT_COL--;
			}
		}
		else if(command == 2)//We just turned left
		{
			if(currentOrientation == 0) //North
			{
				CURRENT_ROW--;
			}
			else if(currentOrientation == 1) //East
			{
				CURRENT_COL++;
			}
			else if(currentOrientation == 2) //South
			{
				CURRENT_ROW++;
			}
			else //West
			{
				CURRENT_COL--;
			}
			
			currentOrientation--;
			if(currentOrientation < 0)
			{
				currentOrientation = 3;
			}
		}
		else if(command == 3)//We just turned right
		{
			if(currentOrientation == 0) //North
			{
				CURRENT_ROW--;
			}
			else if(currentOrientation == 1) //East
			{
				CURRENT_COL++;
			}
			else if(currentOrientation == 2) //South
			{
				CURRENT_ROW++;
			}
			else //West
			{
				CURRENT_COL--;
			}
			
			currentOrientation++;
			if(currentOrientation > 3)
			{
				currentOrientation = 0;
			}
		}
		else if(command == 4)//We just turned around
		{
			if(currentOrientation == 0) //North
			{
				CURRENT_ROW--;
			}
			else if(currentOrientation == 1) //East
			{
				CURRENT_COL++;
			}
			else if(currentOrientation == 2) //South
			{
				CURRENT_ROW++;
			}
			else //West
			{
				CURRENT_COL--;
			}
			
			currentOrientation += 2; //Turn full circle
			if(currentOrientation > 3)//Orientation used to be 2 or 3, so now is 4 or 5.
			{
				currentOrientation -= 4; //wrap around compass rose accordingly
			}
		}
		else if(command == 7)
		{
			if(currentOrientation == 0) //North
			{
				CURRENT_ROW++;
			}
			else if(currentOrientation == 1) //East
			{
				CURRENT_COL--;
			}
			else if(currentOrientation == 2) //South
			{
				CURRENT_ROW--;
			}
			else //West
			{
				CURRENT_COL++;
			}
		}
	}
	
	//********************END MAIN GAME****************************//
	
	//Looks like we are done
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}
