//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug and other flags
bool is_debug = true;
bool is_test_demo = false;

int MSG_LED = 13;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);

//Message System (IN)
int E = 4, F = 5, G = 6, H = 7;
int L, M, N, O;

int PR = 8; //Set high to indicate that package from RM has been recieved

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
int startButton = 0;
bool robotStart = true;
bool firstRun = true;
bool progStart = true;


//TODO: Replace sampleMessages with an actual path.
void sendMessage(int messageToSend, bool useArg)
{	
	digitalWrite(MSG_LED, HIGH);

	if(!useArg)
	{
		if(index < sizeof(sampleMessages))
		{
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
	
	Serial.print(messageToSend);
	
}

void getMessage()
{
	digitalWrite(PR, HIGH);
	digitalWrite(MSG_LED, HIGH);
	
	L = digitalRead(E);
	M = digitalRead(F);
	N = digitalRead(G);
	O = digitalRead(H);
	
	delay(500);
	
	
	if(is_debug)
	{
		//Serial.println(L);
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
	else if(L == 1) //If motion complete
	{
		Serial.println("Sending...");
		while(digitalRead(E))
		{
			sendMessage(0, false);
			delay(250);
		}
		Serial.println(" was sent!");
		
		if(index < sizeof(sampleMessages)-1)
		{
			++index;
		}
		else
		{
			sendMessage(0, true);
		}
	}
	
	//digitalWrite(PR, LOW);
	digitalWrite(MSG_LED, LOW);
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
	//Drop everything to get data from NAV
	attachInterrupt(0, getMessage, CHANGE);
	
	//Interrupt to indicate transmission of data
	pinMode(I, OUTPUT);
	
	//AI -> NAV multiplexer
	pinMode(B, OUTPUT);
	pinMode(C, OUTPUT);
	pinMode(D, OUTPUT);
	
	//NAV -> AI Pins
	pinMode(E, INPUT);
	pinMode(F, INPUT);
	pinMode(G, INPUT);
	pinMode(H, INPUT);
	
	//Pin to indicate that transmission of data was recieved
	pinMode(PR, OUTPUT);
	digitalWrite(PR, LOW);
	
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
		//If the start button has been pressed, begin the driving sequence
		if(robotStart && digitalRead(E) && firstRun)
		{
			delay(1000); //Pause so the button can stop being pressed
			Serial.println(true);
			
			Serial.println("Sending...");
			while(digitalRead(E))
			{
				sendMessage(1, false);
				delay(1000);
			}
			Serial.println("Sent!");
			
			if(index < sizeof(sampleMessages))
			{
				++index;
			}
			
			firstRun = false;
		}
	}
	//Serial.println(digitalRead(E));
	digitalWrite(PR, LOW);
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
