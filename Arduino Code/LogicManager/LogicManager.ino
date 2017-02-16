//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug and other flags
bool is_debug = true;
bool is_test_demo = true;

int MSG_LED = 13;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);

//Message System (OUT)
int E = 8, F = 9, G = 10, INSTRUCT = 11;
int L, M, N, O;

int PR = 8; //Set high to indicate that package from RM has been recieved

//Message System (IN)
int NAV_READY = 7;
int I = 12;
int lastIValue = HIGH;

//Test message communication
int index = 0;
int sampleMessages[11] = {1,2,1,3,1,4,1,2,1,3,1};

//Current Coordinates
int x = 0, y = 6; // x = column, y = row

//Start & Stop Buttons
int startButton = 0;
bool robotStart = true;
bool firstRun = true;
bool progStart = true;


//TODO: Replace sampleMessages with an actual path.
void sendMessage(int messageToSend, bool useArg)
{	
	if(!useArg)
	{
		if(index < sizeof(sampleMessages))
		{
			messageToSend = sampleMessages[index];
		}
	}
	
	/*
	int x = (messageToSend & 4)>>2;
	int y = (messageToSend & 2)>>1;
	int z = messageToSend & 1;
	*/
	int x = (messageToSend & 4)>>2;
	int y = (messageToSend & 2)>>1;
	int z = messageToSend & 1;
	
	digitalWrite(E, x);
	digitalWrite(F, y);
	digitalWrite(G, z);
	
	//digitalWrite(INSTRUCT, HIGH);
	
	if(lastIValue == LOW)
	{
		digitalWrite(INSTRUCT, HIGH);
		lastIValue = HIGH;
	}
	else
	{
		digitalWrite(INSTRUCT, LOW);
		lastIValue = LOW;
	}
}

void getMessage()
{
	/*
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
	else if(L == 1) //If motion complete
	{
		if(index < sizeof(sampleMessages))
		{
			sendMessage(0, false);
			++index;
		}
		else
		{
			sendMessage(0, true);
		}
	}
	*/
}


void setup() 
{
	//Interrupt to indicate transmission of data
	//pinMode(I, OUTPUT);
	
	//NAV -> AI Pins
	pinMode(E, INPUT);
	pinMode(F, INPUT);
	pinMode(G, INPUT);
	pinMode(INSTRUCT, OUTPUT);
	
	//Pin to indicate that transmission of data was recieved
	//pinMode(PR, OUTPUT);
	//digitalWrite(PR, LOW);
	
	pinMode(MSG_LED, OUTPUT);
	digitalWrite(MSG_LED, HIGH);
	delay(500);
	digitalWrite(MSG_LED, LOW);
	delay(500);
	digitalWrite(MSG_LED, HIGH);
	delay(500);
	digitalWrite(MSG_LED, LOW);
	
	if(is_debug)
	{
		Serial.begin(115200);
	}
	
	firstRun = true;
}

void loop() 
{ 
	//Wait for everything to boot up
	if(progStart)
	{
		delay(5000);
		progStart = false;
	}

	//If we are testing the bot, use this
	if(is_test_demo)
	{
		sendMessage(1, true);
		delay(15000);
	}
	else //Competition mode
	{
		if(robotStart && digitalRead(NAV_READY) && firstRun)
		{
			delay(1000); //Pause so the button can stop being pressed
			
			sendMessage(1, false);
			
			if(index < sizeof(sampleMessages))
			{
				++index;
			}
			
			firstRun = false;
		}
	}
	
	if(digitalRead(NAV_READY))
	{
		//TODO: Read sensors before sending message
		if(index < sizeof(sampleMessages))
		{
			sendMessage(0, false);
			++index;
		}
		else
		{
			sendMessage(0, true);
		}
	}
	else
	{
		if(lastIValue == LOW)
		{
			digitalWrite(INSTRUCT, HIGH);
			lastIValue = HIGH;
		}
		else
		{
			digitalWrite(INSTRUCT, LOW);
			lastIValue = LOW;
		}
	}
	
	delay(50);
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
