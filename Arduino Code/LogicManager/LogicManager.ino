//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug and other flags
bool is_debug = true;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);

//Message System (IN)
int E = 4, F = 5, G = 6, H = 7;
int L, M, N, O;

//Message System (OUT)
int B = 9, C = 10, D = 11;
int I = 12;
int lastIValue = HIGH;

//Test message communication
int index = 0;
int sampleMessages[11] = {1,2,1,3,1,4,1,2,1,3,1};

//Current Coordinates
int x = 0, y = 6; // x = column, y = row

//Start & Stop Buttons
int startButton = 8;
bool robotStart = true;
bool firstRun = true;


//TODO: Replace sampleMessages with an actual path.
void sendMessage(int messageToSend, bool useArg)
{	
	if(!useArg)
	{
		if(index < sizeof(sampleMessages))
		{
			++index;
			messageToSend = sampleMessages[index];
		}
	}	
	
	int x = messageToSend & 4;
	int y = messageToSend & 2;
	int z = messageToSend & 1;
	
	digitalWrite(B, x);
	digitalWrite(C, y);
	digitalWrite(D, z);	
	
	if(lastIValue == LOW)
	{
		digitalWrite(I, HIGH);
		lastIValue = HIGH;
	}
	else
	{
		digitalWrite(I, LOW);
		lastIValue = LOW;
	}
}

void getMessage()
{
	L = digitalRead(E);
	M = digitalRead(F);
	N = digitalRead(G);
	O = digitalRead(H);
	
	if(is_debug)
	{
		Serial.println(L);
	}
	
	if(M == 1)
	{
		//scm.setPixelRed();
		//Do some stuff to the square.
	}
	else if(N == 1)
	{
		//scm.setPixelBlue();
		//Do some stuff to the Square
	}
	
	if(O == 1)
	{
		//Recalculate route
		//Set Square to obstacle
	}
	else if(L == 1 && robotStart) //If motion complete
	{
		//index++;
		//sendMessage(0, false);
	}
}

void stopRobot()
{
	digitalWrite(B, 1);
	digitalWrite(C, 1);
	digitalWrite(D, 1);	
	
	if(lastIValue == LOW)
	{
		digitalWrite(I, HIGH);
		lastIValue = HIGH;
	}
	else
	{
		digitalWrite(I, LOW);
		lastIValue = LOW;
	}
	
}

void setup() 
{
	
	attachInterrupt(0, getMessage, CHANGE);
	
	pinMode(I, OUTPUT);
	
	pinMode(B, OUTPUT);
	pinMode(C, OUTPUT);
	pinMode(D, OUTPUT);
	
	pinMode(E, INPUT);
	pinMode(F, INPUT);
	pinMode(G, INPUT);
	pinMode(H, INPUT);
	
	if(is_debug)
	{
		Serial.begin(115200);
	}
	
}

void loop() 
{ 
/*
	if(robotStart && digitalRead(E) && firstRun)
	{
		sendMessage(1, true);
		firstRun = false;
	}*/
	sendMessage(1, true);
	delay(15000);
}

 /* Mapping for SCR Matrix display
 _ _ _ _ _ _ _ _
|s s s c c r r r
|s     c   r   r
|s s s c   r r
|    s c   r   r
|s s s c c r   r
|
|
|
*/
