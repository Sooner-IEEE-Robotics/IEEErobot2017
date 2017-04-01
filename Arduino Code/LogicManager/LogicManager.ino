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
bool cache1_found = false, cache2_found = false;
int cache1_row, cache1_col;
int cache2_row, cache2_col;

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
			/**
			if(topEdge == 1)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			**/
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
			/**
			if(botEdge == 5)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			**/
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
			/**
			if(leftEdge == 2)
			{
				googleMaps.push(1);
				googleMaps.push(1);
				googleMaps.push(7);
				googleMaps.push(7);
			}
			**/
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
			/*
			if(rightEdge == 5)
			{
				googleMaps.push(1);
				googleMaps.push(7);
			}
			*/
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
}

//Infer where the caches are based on the mapping of the inner squares
void figureOutWhereTheCachesAre()
{
	//This is a list of the inner 5x5 borders, not actual border rows
	int row[4] = {1, 5, 1, 5};
	for(int i = 0; i < 4; ++i)
	{
		for(int col = 2; col < 5; ++col)
		{
			int index;
			//Translate the 1D array into 2D
			if(i < 2)
			{
				index = (row[i]*7) + col;
			}
			else
			{
				index = (col*7) + row[i];
			}
			
			//If a spot has metal, count neighbors
			if(board[index] == 1)
			{
				int top, top_stats;
				int bot, bot_stats;
				int left, left_stats;
				int right, right_stats;
				
				if(i < 2)
				{
					//Is the top neighbor metal?
					top = ((row[i]-1)*7) + col;
					top_stats = board[top];
					
					//Is the bottom neighbor metal?
					bot = ((row[i]+1)*7) + col;
					bot_stats = board[bot];
					
					//Is the left neighbor metal?
					left = (row[i]*7) + col - 1;
					left_stats = board[left];
					
					//Is the right neighbor metal?
					right = (row[i]*7) + col + 1;
					right_stats = board[right];
				}
				else
				{
					//Is the top neighbor metal?
					top = ((col-1)*7) + row[i];
					top_stats = board[top];
					
					//Is the bottom neighbor metal?
					bot = ((col+1)*7) + row[i];
					bot_stats = board[bot];
					
					//Is the left neighbor metal?
					left = (col*7) + row[i] - 1;
					left_stats = board[left];
					
					//Is the right neighbor metal?
					right = (col*7) + row[i] + 1;
					right_stats = board[right];
				}
				
				//How many neighboring squares are metal?
				int sum = top_stats + bot_stats + left_stats + right_stats;
				
				//If there is only one neighbor with metal, we found a cache
				if(sum == 1)
				{
					//If the first cache hasnt been found yet
					if(!cache1_found)
					{
						if(i == 0)
						{
							cache1_row = row[i] - 1;
							cache1_col = col;
						}
						else if(i == 1)
						{
							cache1_row = row[i] + 1;
							cache1_col = col;
						}
						else if(i == 2)
						{
							cache1_row = row[i];
							cache1_col = col - 1;
						}
						else if(i == 3)
						{
							cache1_row = row[i];
							cache1_col = col + 1;
						}
						
						scm.setPixelRed(cache1_row, cache1_col);
						
						cache1_found = true;
					}
					//If the second hasnt been found but the first has
					else if(!cache2_found)
					{
						if(i == 0)
						{
							cache2_row = row[i] - 1;
							cache2_col = col;
						}
						else if(i == 1)
						{
							cache2_row = row[i] + 1;
							cache2_col = col;
						}
						else if(i == 2)
						{
							cache2_row = row[i];
							cache2_col = col - 1;
						}
						else if(i == 3)
						{
							cache2_row = row[i];
							cache2_col = col + 1;
						}
						
						scm.setPixelRed(cache2_row, cache2_col);
						
						cache2_found = true;
					}
				}
			}
		}
	}
}

//Go back to start
void goBackToStart()
{
	//3 back ups
	mapQuest.push(7);
	mapQuest.push(7);
	mapQuest.push(7);
	
	//Inch forward
	//mapQuest.push(6);
	
	//Turn right
	mapQuest.push(3);
	
	//Drive forward 2 times
	mapQuest.push(1);
	mapQuest.push(1);
	
	//Inch forward
	//mapQuest.push(6);
	
	//Turn Right
	mapQuest.push(3);
	
	//Forward
	mapQuest.push(1);
	mapQuest.push(1);
}

//Go to a cache and open it at the end of the routine
void travelToCache()
{
	bool goodCacheFound = false;
	int bestR, bestC;
	
	int min_col_diff = 8;
	
	if(cache1_found)
	{
		int col_diff = cache1_col - CURRENT_COL;
		
		bestR = cache1_row;
		bestC = cache1_col;
			
		if(col_diff > 0)
		{
			min_col_diff = col_diff;
			goodCacheFound = true;
		}
	}
	
	if(cache2_found)
	{
		int col_diff = cache2_col - CURRENT_COL;
		
		if(col_diff > 0)
		{
			if(col_diff < min_col_diff && goodCacheFound)
			{
				bestR = cache2_row;
				bestC = cache2_col;
			}
			else if(!goodCacheFound)
			{
				bestR = cache2_row;
				bestC = cache2_col;
				
				goodCacheFound = true;
			}
		}
		else if(!goodCacheFound && cache2_col > cache1_col)
		{
			bestR = cache2_row;
			bestC = cache2_col;
		}
	}
	
	//Robot Coordinates
	int simulated_col = CURRENT_COL;
	int simulated_row = CURRENT_ROW;
	
	//Calculate path
	if(!goodCacheFound)
	{
		//Turn around
		mapQuest.push(6);//inch
		mapQuest.push(2);//left
		mapQuest.push(6);//inch
		mapQuest.push(2);//left
		
		simulated_col++;
		simulated_row--;
	}
	
	//If the cache is on the North or south ends of the board
	if(bestR == 0 || bestR == 6)
	{
		//Go forward until the robot is ready to turn toward the cache
		int first_fwd = abs(simulated_col - bestC) - 1;
		for(int i = 0; i < first_fwd; ++i)
		{
			mapQuest.push(1);//Forward
		}
		
		//Turn to the cache
		if(goodCacheFound)
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(3);//Turn right (South)
			}
			else
			{
				mapQuest.push(2);//Turn left (North)
			}
		}
		else
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(2);//Turn left (South)
			}
			else
			{
				mapQuest.push(3);//Turn right (North)
			}
		}
		
		//Go forward until right before the cache
		int next_fwd = abs(simulated_row - bestR) - 1;
		for(int i = 0; i < next_fwd; ++i)
		{
			mapQuest.push(1);//Forward
		}
		
	}
	//Otherwise the cache is on the east or west end
	else
	{
		mapQuest.push(6); //Inch forward
		
		//Turn parallel to the cache
		if(goodCacheFound)
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(3);//Turn right (South)
			}
			else
			{
				mapQuest.push(2);//Turn left (North)
			}
		}
		else
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(2);//Turn left (South)
			}
			else
			{
				mapQuest.push(3);//Turn right (North)
			}
		}
		
		//Drive until the row right below the cache
		int next_fwd = abs(simulated_row - bestR) - 1;
		for(int i = 0; i < next_fwd; ++i)
		{
			mapQuest.push(1);//Forward
		}
		
		//Do the opposite turn of what we did first
		if(goodCacheFound) //Turn West
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(2);//Turn left
			}
			else
			{
				mapQuest.push(3);//Turn right
			}
		}
		else //Turn East
		{
			if(bestR > simulated_row)
			{
				mapQuest.push(3);//Turn right
			}
			else
			{
				mapQuest.push(2);//Turn left
			}
		}
		
		//Drive to right before the cache
		int first_fwd = abs(simulated_col - bestC) - 2; //Note the -2, we turned forward at the start
		for(int i = 0; i < first_fwd; ++i)
		{
			mapQuest.push(1);//Forward
		}
		
	}
	
	//General Cache Sequence
	mapQuest.push(5); //Enter the cache sequence
	mapQuest.push(1);//Drive up to the lid
	mapQuest.push(3);//Open the cache
	mapQuest.push(4);//Take a picture of the die
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
			
			if(CURRENT_COL == 0 || CURRENT_COL == 6 || CURRENT_ROW == 0 || CURRENT_ROW == 6)
			{
				if(!cache1_found)
				{
					cache1_col = CURRENT_COL;
					cache1_row = CURRENT_ROW;
					cache1_found = true;
				}
				else if(!cache2_found)
				{
					cache2_col = CURRENT_COL;
					cache2_row = CURRENT_ROW;
					cache2_found = true;
				}
			}
		}
		
		//Gets the next command from the queue.
		command = googleMaps.pop();			
		
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
	
	//Now that we have visited all of the inner 5x5, we should infer where the caches are.
	figureOutWhereTheCachesAre();
	
	//Either go back to start, or open the cache
	if(ENDGAME == 1)
	{
		goBackToStart();
	}
	else if(ENDGAME == 2)
	{
		travelToCache();
	}
	else if(ENDGAME == 3)
	{
		if(cache1_found || cache2_found)
		{
			//Find the cache
			travelToCache();
		}
		//If the above statement is false, the robot should do nothing
	}
	else if(ENDGAME == 4)
	{
		if(!(cache1_found || cache2_found))
		{
			//Go back to start
			goBackToStart();
		}
		else //We found at least 1 cache, so let's open it
		{
			travelToCache();
		}
	}
	
	//Note that if ENDGAME == 0, the robot does nothing. Therefore we do not set mapQuest and the below loop does not occur
	
	//*************END GAME*************************//
	
	//Run code repeatedly based on what MapQuest tells us to do.
	while(mapQuest.count() > 0)
	{
		while(digitalRead(moving) == LOW)//VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
		{ 
			// Do whatever here while we wait 
		}
		
		//Stops and waits for a smaller amount than before
		delay(500);
		
		//Gets the next command from the queue.
		command = mapQuest.pop();			
		
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
	}
	
	//Looks like we are done
	while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}
