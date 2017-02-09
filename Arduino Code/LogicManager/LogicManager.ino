//#include "Wire.h"
#include "SPI.h"
#include "SoonerColorduinoMaster.h"

//Debug flags
bool is_debug = true;

//SPI
int slave_select = 53;

SoonerColorduinoMaster scm(8);

void setup() 
{
  if(is_debug)
  {
	Serial.begin(115200);
  }
}

void loop() 
{ 
  int success = scm.setPixelRed(1,1);
  
  Serial.println(success);
}

