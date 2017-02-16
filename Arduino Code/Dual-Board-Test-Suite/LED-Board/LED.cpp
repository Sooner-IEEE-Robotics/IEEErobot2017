#include "LED.h"

LED::LED(int pin)
{
	this->pin = pin;
}

void LED::setupPin()
{
	pinMode(pin, OUTPUT);
}

void LED::blink()
{
	digitalWrite(pin, HIGH);
	delay(250);
	digitalWrite(pin, LOW);
	delay(250);
}

void LED::on()
{
	digitalWrite(pin, HIGH);
}

void LED::off()
{
	digitalWrite(pin, LOW);
}