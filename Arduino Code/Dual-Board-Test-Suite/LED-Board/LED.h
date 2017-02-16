#ifndef LED_H
#define LED_H

#include "Arduino.h"

class LED
{
	public:
		LED(int pin);
		void setupPin();
		void blink();
		void on();
		void off();
		
	protected:
	
	private:
		int pin;
};

#endif LED_H