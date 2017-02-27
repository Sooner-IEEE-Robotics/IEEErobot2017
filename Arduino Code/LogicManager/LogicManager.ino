#include <QueueList.h>
#include <StackList.h>
#include "SoonerColorduinoMaster.h"

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;
int stateTest[6] = {1,2,1,3,1,4};    //test array for various states, bot will iterate through these states 

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
int currentOrientation = 1;

//Position of the robot
int CURRENT_ROW = 6, CURRENT_COL = 0;

//Edge Management variables used to constrain new spirals
int MIN_COL = 0, MAX_COL = 6, MIN_ROW = 0, MAX_ROW = 5

//Directions to follow in order to win
QueueList<byte> googleMaps;

//Obstacle Avoidance Stack of endpoints
StackList<int> endPoints;

//Board, as a 1D array
//(i/7) = Row, (i%7) = Column
int board[49];
/*
0 = Unexplored
1 = Main Tunnel
2 = Dead End Tunnel
3 = Empty
4 = Start
5 = Obstacle
*/

void initBoard()
{
	for(int i = 0; i < 49; ++i)
	{
		if((i/7) == 6 && (i%7) == 0)
		{
			board[i] = 4; //A7 is the start
			scm.setPixelYellow(0, 6); //Update the Colorduino accordingly
		}
		else
		{
			board[i] = 0;
		}
	}
}

//*****************Path Generation*********************//

void getPath(int top, int bottom, int left, int right, int direction)
{	
	int topEdge = top, botEdge = bottom, leftEdge = left, rightEdge = right;

	int r = 6, c = 0, directionOfTravel = direction;
	
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

void avoidObstacle(int obstacle_location)
{
	updateBounds();
	
	int obstacle_row = obstacle_location/7;
	int obstacle_col = obstacle_location%7;
	
	int targetRow, targetCol;
	
	if(currentOrientation == 0)
	{
		targetRow = obstacle_row - 1;
	}
	else if(currentOrientation == 2)
	{
		targetRow = obstacle_row + 1;
	}
	
	if(currentOrientation == 1)
	{
		targetCol = obstacle_col + 1;
	}
	else if(currentOrientation == 3)
	{
		targetCol = obstacle_col - 1;
	}
	
	int targetLocation = (targetRow*7) + targetCol;
	
	endPoints.push(targetLocation);
}

void printPath()
{
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
		
		googleMaps.push(direction);
	}
	
	Serial.println("EOF");
}
//**********************END PATH GENERATION*******************//

//*******************EDGE MANAGEMENT********************//
void updateBounds()
{
	int t = 5, b = 0; //Rows
	int l = 6, r = 0; //Cols
	
	for(int i = 0; i < 49; ++i)
	{
		if(board[i] == 0)
		{
			//Row Boundaries
			if((i/7) > b)
			{
				b = i/7;
			}
			if((i/7) < t)
			{
				t = i/7;
			}
			
			//Column Boundaries
			if((i%7) > r)
			{
				r = i%7;
			}
			if((i%7) < l)
			{
				l = i%7;
			}
		}
	}
	
	MIN_ROW = t;
	MAX_ROW = b;
	MIN_COL = l;
	MAX_COL = r;
}
//***********************END EDGE MANAGEMENT********************//

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
	
	//Obstacle Detection
	pinMode(sharpSensorPin, INPUT);
	
	Serial.begin(9600);
	Serial.println("Calculating Route...");
	
	//Generate the first path
 	//googleMaps = *spiral.getDefaultPath();
	getPath(MIN_ROW, MAX_ROW, MIN_COL, MAX_COL, currentOrientation);
	
	Serial.println("Route Calculated");
	Serial.println(googleMaps.count());
		
	delay(5000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0, lastCommand = 0;
	bool commandApproved = true;
	int obstacleLocation;
	
	//Run code repeatedly based on what Google Maps tells us to do.
	while(googleMaps.count() > 0)
	{
		while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
		{ 
			// Do whatever here while we wait 
		}
		
		//Default to being allowed to move forward with the commands
		commandApproved = true;
		
		//Update Orientation and position based on the prior command
		if(lastCommand == 1)//We just drove straight
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
		else if(lastCommand == 2)//We just turned left
		{
			currentOrientation--;
			if(currentOrientation < 0)
			{
				currentOrientation = 3;
			}
		}
		else if(lastCommand == 3)//We just turned right
		{
			currentOrientation++;
			if(currentOrientation > 3)
			{
				currentOrientation = 0;
			}
		}
		else if(lastCommand == 4)//We just turned around
		{
			currentOrientation += 2; //Turn full circle
			if(currentOrientation > 3)//Orientation used to be 2 or 3, so now is 4 or 5.
			{
				currentOrientation -= 4; //wrap around compass rose accordingly
			}
		}
			
		//*****Go through the checklist of things to do before actually moving.****//
		if(digitalRead(metalDetectorPin) == HIGH) //If metal is found, update map and display to colorduino
		{
			int i = (CURRENT_ROW*7) + CURRENT_COL;
			board[i] = 1;
			
			//Update the colorduino to show the main tunnel
			scm.setPixelRed(CURRENT_COL, CURRENT_ROW);
		}
		if(analogRead(sharpSensorPin) < OBSTACLE_THRESHOLD) //If there is an obstacle in front of us
		{
			int i;
			if(currentOrientation == 0)
			{
				i = ((CURRENT_ROW-1)*7) + CURRENT_COL;
			}
			else if(currentOrientation == 1)
			{
				i = (CURRENT_ROW*7) + CURRENT_COL + 1;
			}
			else if(currentOrientation == 2)
			{
				i = ((CURRENT_ROW+1)*7) + CURRENT_COL;
			}
			else
			{
				i = (CURRENT_ROW*7) + CURRENT_COL - 1;
			}
			
			board[i] = 5;
			obstacleLocation = i;
			
			//We need to generate a new set of commands.
			commandApproved = false;
		}
		
		if(commandApproved)
		{
			command = googleMaps.pop(); //Gets the next command from the queue.
		}
		else
		{
			while(!googleMaps.isEmpty())
			{
				googleMaps.pop();
			}
			
			avoidObstacle(obstacleLocation);
		}
		
		
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
