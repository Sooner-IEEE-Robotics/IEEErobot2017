#include <Servo.h>

Servo testServo;

void setup()
{
	testServo.attach(11);
}

void loop()
{
	//Go to 0 degrees
	testServo.write(0);
	
	//Wait for arm to travel to setpoint
	delay(1000);
	
	while(1)
	{}
}