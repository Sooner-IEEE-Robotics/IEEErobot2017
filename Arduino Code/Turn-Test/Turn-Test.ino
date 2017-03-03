#include "Wire.h"
#include "PIDController.h"

//A0 on MPU6050 set to 0
//6050 7 bit address is 0x68
//PWMGT address is 107; bit 6 controls sleep mode
//z axis gryo is 71 and 72; low is 72, high is 71
const int MPU_addr=0x68;  // I2C address of the MPU-6050
double gyro = 0;
long int t = millis();
float testing = 100000;

float gyroConvert = .978 * float(250)/(float(30500) * float(100000.0));

double FORWARD_DIST = 14;
float LEFT_TURN  = -90.25;
float RIGHT_TURN = 90.25;
float FULL_TURN = 166;

double STOP_SPEED_THRESHOLD = 0.15;

//Navigation mode flags
bool isTurnInPlace = false;
bool turnComplete = false;
bool driveComplete = false;
bool isMotionFinished = false; //When the main control loop decides that it is finished, set this to true
float startYaw = 0;

int n = LOW, m = LOW;
int leftEncoderALast = LOW;
int rightEncoderALast = LOW;

int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10;
int right_motor_pin = 44, right_in_1 = 48, right_in_2 = 46;
int leftEncoderA = 18;
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;

double distance = 0.0, gyro_error = 0;
double kR = -0.00395, kL = 0.00418;//Encoder constants to convert to inches
double Y, X;


volatile int leftEncoderPos = 0;
volatile int rightEncoderPos = 0;
double yaw = 0;

//Targets
float targetYaw = 0;
double distance_target = 0;

//Stores the PID constants for driving a distance and turning. [kP, kI, kD]
float turnPID[3] = {0.21, 0.0007, 0.0005};
float distPID[3] = {0.35, 0.0005, 0.000}; 

PIDController turningPID(0, turnPID);
PIDController distancePID(0, distPID);


//************************GYRO BLOCK*******************************//
double pollGyro(){
 Wire.beginTransmission(MPU_addr);
 Wire.write(71);  // starting with register 0x3B (ACCEL_XOUT_H)
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,2,true);  // request a total of 2 registers
 gyro = (~((Wire.read()<<8 | Wire.read())-1));
 //gyro = (((~(Wire.read()-1))<<8|(~(Wire.read()-1))));//*(250/32768));
 //Serial.println(gyro);
 
 return(gyro);//*(250.0/32768.0)-.05
 //Serial.println();
} 
//************END GYRO BLOCK************************************//

//If the signals are the same, the encoder is rotating forward, else backwards
void doLeftEncoder()
{
  if(digitalRead(leftEncoderA) == digitalRead(leftEncoderB))
  {
    ++leftEncoderPos;
  }
  else
  {
    --leftEncoderPos;
  }
}
void doRightEncoder()
{
  if(digitalRead(rightEncoderA) == digitalRead(rightEncoderB))
  {
    ++rightEncoderPos;
  }
  else
  {
    --rightEncoderPos;
  }
}

void arcadeDrive(float forward_power, float turn_power)
{
  float right, left;
  
  if(forward_power > 0)
  {
    if(turn_power > 0)
    {
      right = forward_power - turn_power;
      left = max(forward_power, turn_power);
    }
    else
    {
      right = max(forward_power, -turn_power);
      left = forward_power + turn_power;
    }
  }
  else
  {
    if(turn_power > 0)
    {
      right = (-1) * max(-forward_power, turn_power);
      left = forward_power + turn_power;
    }
    else
    {
      right = forward_power - turn_power;
      left = (-1) * max(-turn_power, -forward_power);
    }
  }
  
//Configure motors for directional driving
  if(left < 0)
  {
	digitalWrite(left_in_1, HIGH);
	digitalWrite(left_in_2, LOW); 
  }
  else if(left == 0)
  {
	digitalWrite(left_in_1, LOW);
	digitalWrite(left_in_2, LOW); 
  }
  else
  {
	digitalWrite(left_in_1, LOW);
	digitalWrite(left_in_2, HIGH); 
  }
  
  if(right < 0)
  {
	digitalWrite(right_in_1, HIGH);
	digitalWrite(right_in_2, LOW);
  }
  else if(right == 0)
  {
	digitalWrite(right_in_1, LOW);
	digitalWrite(right_in_2, LOW);
  }
  else
  {
	digitalWrite(right_in_1, LOW);
	digitalWrite(right_in_2, HIGH);
  }
  
  //Output to motors
  analogWrite(left_motor_pin, abs(left)  * 255);
  analogWrite(right_motor_pin, abs(right) * 255);
}
/*
	End Driving Functions
*/

bool mainControlLoop()
{

  //Update the yaw of the robot
    //yaw += float((micros()-t)*(pollGyro()-6)/testing)*(250.0/32768.0);
	yaw += float(((micros()-t)*(pollGyro()-6)))*gyroConvert;
    t = micros();
	
    Serial.print(driveComplete);
    Serial.print("\n");
    Serial.println(turnComplete);

    gyro_error = yaw - targetYaw;
    //gyro_error = fmod((gyro_error + 180), 360.0) - 180;
    if(gyro_error > 180)
    {
      gyro_error = -(360 - gyro_error); 
    }
  
	if((!driveComplete || !turnComplete))
	{	
		//Get the output variables based on PID Control
		if(!isTurnInPlace)
		{
			X = distancePID.GetOutput(distance_target, distance); //Calculate the forward power of the motors
			
			//If the robot is within a half inch of distance target, stop
			if(abs(distance - distance_target) < 0.5 && abs(X) < STOP_SPEED_THRESHOLD)
			{
				X = 0;
				driveComplete = true;
				Serial.println("Drive Complete.");
			}
			else
			{
				driveComplete = false;
			}
		}
		else
		{
			driveComplete = true;
			X = 0;
		}
		
		Y = turningPID.GetOutput(0, gyro_error); //Calculate the turning power of the motors
	
		//Don't output if the output won't move the robot (save power)
		//X = filter(X);
		//Y = filter(Y);


	
		//if we are only off by 1 degree, dont turn
		if(((abs(gyro_error) < 0.25 && isTurnInPlace) || (abs(gyro_error) < 0.5 && !isTurnInPlace)) && abs(Y) < STOP_SPEED_THRESHOLD)
		{
			Y = 0;
			turnComplete = true;
			Serial.println("Turn Complete.");
		}
		else
		{
			turnComplete = false;
		}
		
		arcadeDrive(X,Y);
		
		return false;
	}
	else
	{
		return true;
	}
}

void setup()
{
	Serial.begin(9600);
    //encoder initialization
    pinMode(leftEncoderA, INPUT);  //left encoder
    pinMode(leftEncoderB, INPUT);
    digitalWrite(leftEncoderA, HIGH);//pull up resistor
    digitalWrite(leftEncoderB, HIGH);//pull up resistor
    pinMode(rightEncoderA, INPUT);  //right encoder
    pinMode(rightEncoderB, INPUT);
    digitalWrite(rightEncoderA, HIGH);//pull up resistor
    digitalWrite(rightEncoderB, HIGH);//pull up resistor
    
    //Motor Initialization
    pinMode(left_motor_pin, OUTPUT);     //enable/disable
    pinMode(right_motor_pin, OUTPUT);    //enable/disable
    pinMode(right_in_1, OUTPUT);
    pinMode(right_in_2, OUTPUT);
    pinMode(left_in_1, OUTPUT);
    pinMode(left_in_2, OUTPUT);
	
	//PID Initialization
	distancePID.SetOutputRange(0.4, -0.4);
	turningPID.SetOutputRange(0.3, -0.3);
	
	//Encoder
	attachInterrupt(1, doRightEncoder, CHANGE); //pin 3 interrupt
	attachInterrupt(5, doLeftEncoder, CHANGE);
	
	Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(1);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x1B);  // Gyro
    Wire.write(0x18);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    
    delay(10000);  //setup delay
	
	t = micros();
}

void loop()
{
	targetYaw = 0;
	distance_target = 9;
	isTurnInPlace = false;
	
	distance = ((double)(kL*leftEncoderPos) + (double)(kR * rightEncoderPos))/2;
	
	isMotionFinished = mainControlLoop();

    delayMicroseconds(2000);
	
	if(isMotionFinished)
	{
		arcadeDrive(0,0);
		while(1){} //End the program for quick calibration
	}
}