#include <Servo.h>

Servo testServo;

void setup()
{
	testServo.attach(12);
}

void loop()
{
	//Go to 0 degrees
	testServo.write(45);
	
	//Wait for arm to travel to setpoint
	delay(1000);
	
	while(1)
	{}
}