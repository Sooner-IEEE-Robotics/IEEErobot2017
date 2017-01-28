#include "SPI.h"
#include "PIDController.h"

//Debug flags
bool is_debug = true;

//Stores the PID constants for driving a distance and turning. [kP, kI, kD]
float turnPID[3] = {0.875, .0014, 0.0004};
float distPID[3] = {0.875, .0014, 0.0004}; 

PIDController turningPID(0, turnPID);
PIDController distancePID(0, distPID);

int incomingByte, dataByte;

float turnIntegrator = 0, distIntegrator = 0;
float Y, X;

int left_motor_pin = 6, right_motor_pin = 7;
int ss = 10;

//Sensor Data
float leftEncoderPos = 0, rightEncoder = 0, yaw = 0;
float distance = 0.0, gyro_error;
float kR = 1, kL = 1;//Encoder constants to convert to inches

//Target Info
float targetYaw = 0;
float distance_target = 0;

void arcadeDrive(float y_power, float turn_power, int leftPin, int rightPin)
{
  float right, left;
  
  if(y_power > 0)
  {
    if(turn_power > 0)
    {
      right = y_power - turn_power;
      left = max(y_power, turn_power);
    }
    else
    {
      right = max(y_power, -turn_power);
      left = y_power + turn_power;
    }
  }
  else
  {
    if(turn_power > 0)
    {
      right = (-1) * max(-y_power, turn_power);
      left = y_power + turn_power;
    }
    else
    {
      right = y_power - turn_power;
      left = (-1) * max(-turn_power, -y_power);
    }
  }
  
  //Output to motors
  digitalWrite(leftPin, left);
  digitalWrite(rightPin, right);
}

float getSPIData(int id)
{
	SPI.transfer(id);
	
	unsigned char bit1 = SPI.transfer(0);
	unsigned char bit2 = SPI.transfer(0);
	unsigned char bit3 = SPI.transfer(0);
	unsigned char bit4 = SPI.transfer(0);
	
	float f = (float)(bit1 - '0');
}

void setup() 
{
  //Setup Motors
  pinMode(left_motor_pin, OUTPUT);
  pinMode(right_motor_pin, OUTPUT);
  	
  //Setup Serial Comms
  //Serial.begin(115200);
  SPI.begin();
}

void loop() 
{	
  //Calculate real distance of robot
  distance = ((leftEncoderPos * kL) + (rightEncoder * kR)) / 2;
  
  //Calculate gyro error
  gyro_error = targetYaw - yaw;
  gyro_error = ((int)(gyro_error + 180)) % 360 - 180;
  
  X = distancePID.GetOutput(distance_target, distance); //Calculate the forward power of the motors
  Y = turningPID.GetOutput(0, gyro_error); //Calculate the turning power of the motors
  
  arcadeDrive(X,Y, left_motor_pin, right_motor_pin);
}
