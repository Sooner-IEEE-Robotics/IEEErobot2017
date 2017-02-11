//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug and other flags
bool is_debug = true;
bool is_display_SCR = true;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);

//Message System (IN)
int E = 4, F = 5, G = 6, H = 7;
int L, M, N, O;

//Message System (OUT)
int B = 9, C = 10, D = 11;
int I = 12;
int lastIValue = LOW;

//Test message communication
int index = 0;
int sampleMessages = {1,2,1,3,1,4,1,2,1,3,1};

//Current Coordinates
int x = 0, y = 6; // x = column, y = row


//TODO: Replace sampleMessages with an actual path.
void sendMessage()
{
	int messageToSend = 6;
	if(index < sampleMessages.length())
	{
		messageToSend = sampleMessages[index];
	}
	
	int x = messageToSend | 4;
	int y = messageToSend | 2;
	int z = messageToSend | 1;
	
	digitalWrite(B, x);
	digitalWrite(C, y);
	digitalWrite(D, z);	
}

void getMessage()
{
	L = digitalRead(E);
	M = digitalRead(F);
	N = digitalRead(G);
	O = digitalRead(H);
	
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
		index++;
		sendMessage();
	}
}

void setup() 
{
	
	attachInterrupt(0, getMessage, CHANGE);
	
	pinMode(I, OUTPUT);
	
	pinMode(A, OUTPUT);
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
	
	if(is_display_SCR)
	{
		//S
		scm.setPixelRed(0,6);
		scm.setPixelRed(0,5);
		scm.setPixelRed(0,4);
		scm.setPixelRed(0,2);
		scm.setPixelRed(1,6);
		scm.setPixelRed(1,4);
		scm.setPixelRed(1,3);
		scm.setPixelRed(1,2);
		
		//C
		scm.setPixel(2,6,63,63,63);
		scm.setPixel(2,5,63,63,63);
		scm.setPixel(2,4,63,63,63);
		scm.setPixel(2,3,63,63,63);
		scm.setPixel(2,2,63,63,63);
		scm.setPixel(3,6,63,63,63);
		scm.setPixel(3,2,63,63,63);
		
		//R
		scm.setPixelRed(4,6);
		scm.setPixelRed(4,5);
		scm.setPixelRed(4,4);
		scm.setPixelRed(4,3);
		scm.setPixelRed(4,2);
		scm.setPixelRed(5,6);
		scm.setPixelRed(5,4);
		scm.setPixelRed(6,6);
		scm.setPixelRed(6,5);
		scm.setPixelRed(6,3);
		scm.setPixelRed(6,2);
	}
}

void loop() 
{ 
	delay(50);
}

 /* Mapping for SCR Matrix display
 _ _ _ _ _ _ _
|s s c c r r r
|s   c   r   r
|s s c   r r
|  s c   r   r
|s s c c r   r
|
|
*/