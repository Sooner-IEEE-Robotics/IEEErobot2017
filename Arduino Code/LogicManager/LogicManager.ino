#include <QueueList.h>
#include <StackList.h>
#include "SoonerColorduinoMaster.h"

//Threshold to determine if there is an obstacle or not
#define OBSTACLE_THRESHOLD 100

//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;
int stateTest[6] = {1,2,1,3,1,4};    //test array for various states, bot will iterate through these states 

//Metal Detector
int metalDetectorPin = 3;

//IR Sensor (Obstacle Detection)
int sharpSensorPin = A0; //This is an analog sensor

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

//Cache finding variables
bool findingCache = false, fakeCache = false;
int cachesFound = 0;
bool foundTopCache = false;
bool foundBottomCache = false;
bool foundLeftCache = false;
bool foundRightCache = false;
bool onPartTwo = false;

//Edge Management variables used to constrain new spirals

//If we are doing the 7x7 outer
//int MIN_COL = 1, MAX_COL = 6, MIN_ROW = 0, MAX_ROW = 6;

//If we are doing the 5x5 pattern
int MIN_COL = 2, MAX_COL = 5, MIN_ROW = 1, MAX_ROW = 5;


//Directions to follow in order to win
QueueList<byte> googleMaps;

//cache instructions
QueueList<byte> mapQuest;

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

	int r = CURRENT_ROW, c = CURRENT_COL, directionOfTravel = direction;
	
	int boardSize = (MAX_COL - MIN_COL + 2) * (MAX_ROW - MIN_ROW + 1);
	
	for(int i = 0; i < boardSize; ++i)
	{
		
		if(r == (topEdge+1) && directionOfTravel == 0) //If we are going north at the top of the board.
		{
			if(topEdge == 1)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			googleMaps.push(6); //Go an inch for tankSteer
			googleMaps.push(3); //turn right
			//googleMaps.push(1); //drive straight
			directionOfTravel = 1;
			
			++topEdge;
			
			//--c;
			--r;
		}
		else if(r == (botEdge-1) && directionOfTravel == 2) //If we are going south at the bottom of the board
		{
			if(botEdge == 5)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			googleMaps.push(6); //Go an inch for tankSteer
			googleMaps.push(3); //turn right
			//googleMaps.push(1); //drive straight
			directionOfTravel = 3;
			
			--botEdge;
			
			//++c;
			++r;
		}
		else if(c == (leftEdge+1) && directionOfTravel == 3)//If we are going West at the left of the board
		{
			if(leftEdge == 2)
			{
				googleMaps.push(1);
				googleMaps.push(1);
				googleMaps.push(7);
				googleMaps.push(7);
			}
			googleMaps.push(6); //Go an inch for tankSteer
			googleMaps.push(3); //turn right
			//googleMaps.push(1); //drive straight
			directionOfTravel = 0;
			
			++leftEdge;
			
			//++r;
			--c;
		}
		else if(c == (rightEdge-1) && directionOfTravel == 1)//If we are going east at the right of the board
		{
			if(rightEdge == 5)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			googleMaps.push(6); //Go an inch for tankSteer
			googleMaps.push(3); //turn left
			//googleMaps.push(1); //drive straight
			directionOfTravel = 2;
			
			--rightEdge;
			
			//--r;
			++c;
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
	//3 back ups
	googleMaps.push(7);
	googleMaps.push(7);
	googleMaps.push(7);
	
	//Turn right
	googleMaps.push(3);
	
	//Drive forward 2 times
	googleMaps.push(1);
	googleMaps.push(1);
	
	//Turn Right
	googleMaps.push(3);
	
	//Forward
	googleMaps.push(1);
	
	//DONE!
}

void avoidObstacle(int obstacle_location)
{
	updateBounds();
	
	activelyAvoidingObstacles = true;
	
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
	
	bool borderSecurity = true;
	
	if(targetRow > MAX_ROW)
	{
		targetRow = MAX_ROW;
		targetCol++;
		borderSecurity = false;
	}
	else if(targetRow < MIN_ROW)
	{
		targetRow = MIN_ROW;
		targetCol--;
		borderSecurity = false;
	}
	
	if(targetCol > MAX_COL)
	{
		targetCol = MAX_COL;
		targetRow--;
		borderSecurity = false;
	}
	else if(targetCol < MIN_COL)
	{
		targetCol = MIN_COL;
		targetRow++;
		borderSecurity = false;
	}
	
	int targetLocation = (targetRow*7) + targetCol;

	endPoints.push(targetLocation);
	
	googleMaps.push(2); //Left
	googleMaps.push(1); //Forward
	googleMaps.push(3); //Right
	
	if(borderSecurity) //Normal Case
	{
		googleMaps.push(1);
		googleMaps.push(1);
		googleMaps.push(3);
		googleMaps.push(1);
	}
	else //We are on a corner or edge. Only go forward 1 and we are at an O
	{
		googleMaps.push(1);
	}
	
}

void findTheOs()
{
	int currentLocation = (CURRENT_ROW*7) + CURRENT_COL;
	
	int nextO = endPoints.pop(), lastO = -1;
	
	int lastRow = CURRENT_ROW;
	int lastCol = CURRENT_COL;
	
	int rDiff, cDiff;
	
	while(!endPoints.isEmpty())
	{
		
		if(nextO == lastO || nextO == currentLocation)
		{
			nextO = endPoints.pop();
		}
		
		int r = nextO/7;
		int c = nextO%7;
		
		rDiff = lastRow - r;
		cDiff = lastCol - c;
		
		
		for(int i = 0; i < abs(cDiff); ++i)
		{
			googleMaps.push(1);
		}
		
		if(abs(rDiff) > 0)
		{
			googleMaps.push(3);
			
			for(int i = 0; i < abs(rDiff); ++i)
			{
				googleMaps.push(1);
			}
		}
		
		
		lastO = nextO;
		lastRow = nextO/7;
		lastCol = nextO%7;
	}
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

//***********CACHE SEQUENCING*******************************//
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
	//Instead of going too far forward, inch forward and back
	mapQuest.push(6);
	mapQuest.push(5); //Enter cache sequence
	mapQuest.push(7); //Back off the suspected cache
	mapQuest.push(5);
	//mapQuest.push(1);
	//mapQuest.push(6);
	//mapQuest.push(2);
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
1. Back up //1. Undo Left Turn
2-6. Cache Closed Loop
7. Back up
8. inch forward
9. Right
10. Motion Complete!
*/
void setupPartTwo()
{
	//mapQuest.push(7);//Back
	mapQuest.push(5); //Enter cache mode
	//mapQuest.push(2); //Undo left
	mapQuest.push(1);//Go forward a short distance
	mapQuest.push(3);//Arm
	mapQuest.push(4);//Camera
	mapQuest.push(6);
	mapQuest.push(5);//Exit cache mode
	mapQuest.push(7);//Back
	mapQuest.push(6);
	mapQuest.push(3);//Right
}


//Escape if there is no Cache
void escapeCacheSequence()
{
	//mapQuest.push(7);//Back off the fake cache
	mapQuest.push(6); //inch forward so we end up ok when we undo the turn
	mapQuest.push(5);
	mapQuest.push(2);//Undo left turn
	mapQuest.push(7);//Undo inch forward
	mapQuest.push(5);//Exit cache sequence
	mapQuest.push(1);//Back to starting position
}

//***********END CACHE SEQUENCING*******************************//

void setup() 
{
	//int startSquare = scm.setPixelYellow(0, 6);
	
	int startSquare = scm.setPixelRed(2, 2);
	
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
	Serial.println(startSquare);
	
	Serial.println("Calculating Route...");
	
	initBoard();
	
	//make a zig zag pattern to get to the middle 5x5 bottom left corner square, facing north
	/*
	googleMaps.push(6);
	googleMaps.push(3);
	//googleMaps.push(6);
	googleMaps.push(2);
	*/
	
	//Take the Backwards L
	googleMaps.push(6);
	googleMaps.push(2);
	googleMaps.push(1);
	
	//I know this looks useless but keep it this way because it makes the path work
	CURRENT_COL = 1;
	CURRENT_ROW = 5;
	
	//Generate the first path
 	//googleMaps = *spiral.getDefaultPath();
	getPath(MIN_ROW, MAX_ROW, MIN_COL, MAX_COL, currentOrientation);
	
	//Serial.println("Route Calculated");
	Serial.println(googleMaps.count());
	printPath();
	
	//Reset the coordinates back to the real coordinates
	CURRENT_COL = 0;
	CURRENT_ROW = 6;
	currentOrientation = 1; //Reset this to east because we are actually facing that way.
	
	//Show robot ready light
	//scm.setPixel(6, 0, 63, 63, 63); 
	//scm.setPixelRed(6,0);
		
	delay(10000);
}

void loop() 
{
	//The command to send to the robot
	int command = 0;
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
		
		//*****Go through the checklist of things to do before actually moving.****//
		if(!findingCache && digitalRead(metalDetectorPin) == HIGH && !(CURRENT_COL == 0 && CURRENT_ROW == 6))//If metal is found on the regular route, update map and display to colorduino. Also, begin cache sequence
		{
			int i = (CURRENT_ROW*7) + CURRENT_COL;
			board[i] = 1;
			
			//Update the colorduino to show the main tunnel
			scm.setPixelRed(CURRENT_ROW, CURRENT_COL);
			/*
			if(cachesFound < 2)
			{
				//We only want to search for caches if they have not been found on this edge
				if(((currentOrientation == 0 && !foundLeftCache) || (currentOrientation == 1 && !foundTopCache) || (currentOrientation == 2 && !foundRightCache) || (currentOrientation == 3 && !foundBottomCache)) && (CURRENT_COL == 1 || CURRENT_COL == 5 || CURRENT_ROW == 1 || CURRENT_ROW == 5) && !(CURRENT_COL == 0 || CURRENT_COL == 6 || CURRENT_ROW == 0 || CURRENT_ROW == 6))
				{
					findingCache = true;
					setupPartOne();
				}
			}
			*/
		}
		else if(findingCache && !fakeCache && mapQuest.count() == 0 && digitalRead(metalDetectorPin) == HIGH && !onPartTwo)//If we have found a cache
		{
			if(currentOrientation == 0)
			{
				foundLeftCache = true;
			}
			else if(currentOrientation == 1)
			{
				foundTopCache = true;
			}
			else if(currentOrientation == 2)
			{
				foundRightCache = true;
			}
			else if(currentOrientation == 3)
			{
				foundBottomCache = true;
			}
			
			//If we still have caches to find
			if(cachesFound < 2)
			{
				//If we have not found a cache before, we should try to take a picture
				if(cachesFound < 1)
				{
					setupPartTwo();
					onPartTwo = true;
				}
				//Otherwise we should escape the cache sequence like it was never there
				else
				{
					escapeCacheSequence();
					onPartTwo = true;
				}
			}
			
			cachesFound++;
			
			int i;
			
			if(currentOrientation == 0)//If we were facing north, left column
			{
				i = (CURRENT_ROW*7) + (CURRENT_COL-1);
				board[i] = 1;
				
				scm.setPixelRed(CURRENT_ROW, CURRENT_COL-1);
			}
			else if(currentOrientation == 1) //If we are facing east, up one row
			{
				i = ((CURRENT_ROW-1)*7) + (CURRENT_COL);
				board[i] = 1;
				
				scm.setPixelRed(CURRENT_ROW-1, CURRENT_COL);
			}
			else if(currentOrientation == 2)//If we are facing south, right one column
			{
				i = (CURRENT_ROW*7) + (CURRENT_COL+1);
				board[i] = 1;
				
				scm.setPixelRed(CURRENT_ROW, CURRENT_COL+1);
			}
			else if(currentOrientation == 3)//If we are facing west, down one row
			{
				i = ((CURRENT_ROW+1)*7) + (CURRENT_COL);
				board[i] = 1;
				
				scm.setPixelRed(CURRENT_ROW+1, CURRENT_COL);
			}
						
		}
		else if(findingCache && !fakeCache && mapQuest.count() == 0 && digitalRead(metalDetectorPin) == LOW)//We have determined that this is a fake cache
		{
			escapeCacheSequence();
			fakeCache = true;
		}
		
		
		//*****Determine what type of plan we are following******//
		
		if(findingCache)
		{
			if(mapQuest.count() == 0)
			{
				findingCache = false;
				fakeCache = false;
				onPartTwo = false;
			}
			else
			{
				command = mapQuest.pop();
			}
		}
		
		if(!findingCache)
		{
			command = googleMaps.pop(); //Gets the next command from the queue.
			
			//If the obstacle has been avoided, but we have not explored behind the obstacles enough,
			//generate more waypoints so the program does not end prematurely.
			if(activelyAvoidingObstacles && googleMaps.isEmpty() && !endPoints.isEmpty())
			{
				findTheOs();
			}
			//If the obstacles have fully been avoided, recalculate the spiral path and continue with the program
			else if(activelyAvoidingObstacles && googleMaps.isEmpty() && endPoints.isEmpty())
			{
				activelyAvoidingObstacles = false;
				getPath(MIN_ROW, MAX_ROW, MIN_COL, MAX_COL, currentOrientation);
			}
		}
		/*
		else
		{
			while(!googleMaps.isEmpty())
			{
				googleMaps.pop();
			}
			
			avoidObstacle(obstacleLocation);
			
			command = googleMaps.pop();
		}
		*/
		
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
		
		//Update command for future calculations
		if(!findingCache)
		{
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
	}
	
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}
