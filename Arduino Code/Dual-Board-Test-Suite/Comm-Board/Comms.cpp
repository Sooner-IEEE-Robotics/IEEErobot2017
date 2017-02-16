#include "Comms.h"

Comms::Comms()
{
	
}

void Comms::setup3PinReceive(int a, int b, int c)
{
	/*
	A_in = a;
	B_in = b;
	C_in = c;
	
	pinMode(a, INPUT);
	pinMode(b, INPUT);
	pinMode(c, INPUT);*/
}

void Comms::setup3PinSend(int a, int b, int c)
{
	/*
	A_out = a;
	B_out = b;
	C_out = c;
	
	pinMode(a, OUTPUT);
	pinMode(b, OUTPUT);
	pinMode(c, OUTPUT);
	
	digitalWrite(a, LOW);
	digitalWrite(b, LOW);
	digitalWrite(c, LOW);
	*/
}

void Comms::setupReadyIn(int ready_in)
{
	//readyPin_in = ready_in;
}

void Comms::setupReadyOut(int ready_out)
{
	//readyPin_out = ready_out;
}

void Comms::setupNotifyIn(int notify_in)
{
	//notifyPin_in = notify_in;
}

void Comms::setupNotifyOut(int notify_out)
{
	//notifyPin_out = notify_out;
}

void Comms::send3Bits(byte msb, byte b, byte lsb)
{
	Serial.write(msb);
	Serial.write(b);
	Serial.write(lsb);
	/*
	digitalWrite(A_out, msb);
	digitalWrite(B_out, b);
	digitalWrite(C_out, lsb);
	*/
}

void Comms::sendNumber(unsigned int number)
{
	int msb = (number & 4)>>2;
	int b = (number & 2)>>1;
	int lsb = (number & 1);
	
	Serial.write(msb);
	Serial.write(b);
	Serial.write(lsb);
	/*
	digitalWrite(A_out, msb);
	digitalWrite(B_out, b);
	digitalWrite(C_out, lsb);
	*/
}

int Comms::receiveBitsAndEncode()
{
	/*
	int msb = digitalRead(A_in);
	int b = digitalRead(B_in);
	int lsb = digitalRead(C_in);
	*/
	/*
	Serial.print(msb);
	Serial.print(b);
	Serial.println(lsb);
	*/
	
	int msb = Serial.read() - 48;
	int b = Serial.read() - 48;
	int lsb = Serial.read() - 48;
	
	int data = (msb<<2)|(b<<1)|lsb;
	
	return data;
}

void Comms::notify(int notification)
{
	//digitalWrite(notifyPin_out, notification);
}

void Comms::notifyReady(int notification)
{
	Serial.write(notification);
}

bool Comms::getReady()
{
	return Serial.available() > 0;
	//return digitalRead(readyPin_in);
}