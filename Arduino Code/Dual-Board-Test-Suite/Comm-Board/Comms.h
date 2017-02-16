#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"

class Comms
{
	public:
		Comms();
		
		void setup3PinReceive(int a, int b, int c);
		void setup3PinSend(int a, int b, int c);
		
		void setupReadyIn(int ready_in);
		void setupReadyOut(int ready_out);
		
		void setupNotifyIn(int notify_in);
		void setupNotifyOut(int notify_out);
		
		//Send and receive messages using 3 bits
		void send3Bits(byte msb, byte b, byte lsb);
		void sendNumber(unsigned int number);
		int receiveBitsAndEncode();
		
		//Communicate that a transmission is complete
		void notify(int notification);
		
		//Communicate that the board is ready to receive data
		void notifyReady(int ready); //LOW if not ready, HIGH if ready
		bool getReady();
		
		
	private:
		int A_in, B_in, C_in;
		int A_out, B_out, C_out;
		
		int notifyPin_in, notifyPin_out;
		
		int readyPin_in, readyPin_out;
};

#endif COMMS_H