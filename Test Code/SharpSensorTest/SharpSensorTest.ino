
int sharpSensorPin = A0; //This is an analog sensor

void setup()
{
	Serial.begin(9600);
	
	pinMode(sharpSensorPin, INPUT);
}

void loop()
{
	Serial.println(analogRead(sharpSensorPin));
	
	delay(50);
}