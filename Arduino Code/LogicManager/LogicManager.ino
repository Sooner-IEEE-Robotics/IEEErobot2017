//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug and other flags
bool is_debug = true;
bool is_display_SCR = true;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);


//Test message communication
int sampleMessages = {1,2,1,3,1,4,1,2,1,3,1};


void setup() 
{
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