#include <QueueList.h>
#include "SoonerColorduinoMaster.h"

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;
int stateTest[6] = {1,2,1,3,1,4};    //test array for various states, bot will iterate through these states 

//The current orientation of the robot. This variable must be preserved so we know where to go.
/*
NORTH: 0, EAST: 1, SOUTH: 2, WEST: 3
*/
int currentOrientation = 1;

//Directions to follow in order to win
QueueList<byte> googleMaps;

void getDefaultPath()
{	
	int r = 6, c = 0, directionOfTravel = 1;
	
	int topEdge = 0, botEdge = 5, leftEdge = 0, rightEdge = 6;
	
	for(int i = 0; i < 49; ++i)
	{
		
		if(r == topEdge && directionOfTravel == 0) //If we are going north at the top of the board.
		{
			googleMaps.push(2); //turn left
			googleMaps.push(1); //drive straight
			directionOfTravel = 3;
			
			++topEdge;
			
			--c;
		}
		else if(r == botEdge && directionOfTravel == 2) //If we are going south at the bottom of the board
		{
			googleMaps.push(2); //turn left
			googleMaps.push(1); //drive straight
			directionOfTravel = 1;
			
			--botEdge;
			
			++c;
		}
		else if(c == leftEdge && directionOfTravel == 3)//If we are going West at the left of the board
		{
			googleMaps.push(2); //turn left
			googleMaps.push(1); //drive straight
			directionOfTravel = 2;
			
			++leftEdge;
			
			++r;
		}
		else if(c == rightEdge && directionOfTravel == 1)//If we are going east at the right of the board
		{
			googleMaps.push(2); //turn left
			googleMaps.push(1); //drive straight
			directionOfTravel = 0;
			
			--rightEdge;
			
			--r;
		}
		else //Normal conditions, keep going the same direction
		{
			//drive straight
			googleMaps.push(1);
			
			if(directionOfTravel == 0)
			{
				--r;
			}
			else if(directionOfTravel == 1)
			{
				++c;
			}
			else if(directionOfTravel == 2)
			{
				++r;
			}
			else
			{
				--c;
			}
		}
	}
	
	//Go back to start
	googleMaps.push(3); //Turn right (toward south)
	googleMaps.push(1);
	googleMaps.push(1);
	googleMaps.push(1);
	googleMaps.push(3); //Turn Right (toward west)
	googleMaps.push(1);
	googleMaps.push(1);
	googleMaps.push(1);
	googleMaps.push(7);
}

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
  
  Serial.begin(9600);
  Serial.println("Calculating Route...");
  
  //Generate the first path
 //googleMaps = *spiral.getDefaultPath();
 getDefaultPath();
  
  Serial.println("Route Calculated");
  Serial.println(googleMaps.count());
	
  delay(5000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0;
	
	Serial.println("Path:");
	
	//TEST CODE FOR DIRECTIONS
	int len = googleMaps.count();
	for(int i = 0; i < len; ++i)
	{
		byte direction = googleMaps.peek();
		googleMaps.pop();
		
		if(direction == 0)
		{
			Serial.println("Idle");
		}
		else if(direction == 1)
		{
			Serial.println("Forward");
		}
		else if(direction == 2)
		{
			Serial.println("Left");
		}
		else if(direction == 3)
		{
			Serial.println("Right");
		}
		else if(direction == 4)
		{
			Serial.println("U-Turn");
		}
		else
		{
			Serial.println(direction);
		}
		
		//googleMaps.push(direction);
	}
	
	Serial.println("EOF");
	
	//Run code repeatedly based on what Google Maps tells us to do.
	while(googleMaps.count() > 0)
	{
		while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
		{ 
			// Do whatever here while we wait 
		}
		
		command = googleMaps.pop();
		
		//TODO: Go through the checklist of things to do before actually moving.
		
		//TODO: Update Orientation if necessary.
		
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
	}
	
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}

//**********OLD TEST CODE BLOCK (VERY IMPORTANT)****************//
  //Serial.println("HELP ME");
  /*
  // put your main code here, to run repeatedly: 
  for(int count = 0; count < sizeof(stateTest)/sizeof(int); count++){           //sends first 4 ints in the stateTest array
        while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
                  {}//do whatever 
        
        Serial.println((stateTest[count] & 1)+(stateTest[count]>>1 & 1)+(stateTest[count]>>2 & 1));
        digitalWrite(E, stateTest[count] & 1);                //for some reason, bit shifting works okay here? 
        //digitalWrite(indicator, stateTest[count] & 1);
        digitalWrite(F, stateTest[count]>>1 & 1);
        digitalWrite(G, stateTest[count]>>2 & 1);
        delay(10);                                            //allows time for the lines to actually be asserted as their written value
        digitalWrite(instruct,0);                             //tells the bot that instructions are ready
        while(digitalRead(moving) == 1) {}   //VERY IMPORTANT, waits until robot_mgr realizes that it has incoming data
        digitalWrite(instruct,1);            //disables the instruction line
        delay(10);                           //this delay probably not that important, but allows time for the instruct line to become the proper value before continuing
  }  */
