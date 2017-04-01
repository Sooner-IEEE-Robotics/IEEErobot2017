#include <Servo.h>
#include "Wire.h"
#include "PIDController.h"

//A0 on MPU6050 set to 0
//6050 7 bit address is 0x68
//PWMGT address is 107; bit 6 controls sleep mode
//z axis gryo is 71 and 72; low is 72, high is 71
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
double gyro = 0;
long int t = millis();
float testing = 100000;
float calVal;

float gyroConvert = .978 * float(250)/(float(30500) * float(1000000.0));

double FORWARD_DIST = 11.1; //11.4
double BACKWARD_DIST = -11.3;
float DRIVE_STRAIGHT = 0;//1.1
float LEFT_TURN  = 73.35;
float RIGHT_TURN = -72.7;
float FULL_TURN = 154;

double STOP_SPEED_THRESHOLD = 0.2;

//Make sure we stay at the setpoints for long enough
long setpointTimerThreshold = 750; //1 second
long turnTimerStart, turnTimerElapsed;
long driveTimerStart, driveTimerElapsed;
bool driveInRange = false;
bool turnInRange = false;

//Max time as a last ditch effort to be good
long maxTime = 3000;
long maxTimerStart, maxTimerElapsed;

//Make sure to only reset the gyro after turning
int lastState = 0;
float GYRO_OFFSET = 0;

enum State {
	MAIN_STATE, IDLE_STATE
};

State stateMachine = IDLE_STATE;

//Debug flags
bool is_debug = true;
bool is_nav_debug = true;

//Navigation mode flags
bool isTurnInPlace = false;
bool turnComplete = false;
bool driveComplete = false;
bool isMotionFinished = false; //When the main control loop decides that it is finished, set this to true
float startYaw = 0;

volatile int leftEncoderPos = 0;
volatile int rightEncoderPos = 0;
double yaw = 0;

int n = LOW, m = LOW;
int leftEncoderALast = LOW;
int rightEncoderALast = LOW;

int E = A3, F = A2, G = A1;
int Et = 0, Ft = 0, Gt = 0;
 
int moving = A4;//comm line
int instruct = A0;
 
int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10; //left motor controller pins
int right_motor_pin = 2, right_in_1 = 48, right_in_2 = 46; //right motor controller pins

//The left encoder is actually the right encoder
int leftEncoderA = 18;
int leftEncoderB = 17;
//The right encoder is actually the left encoder
int rightEncoderA = 3;
int rightEncoderB = 4;

double distance = 0.0, gyro_error = 0;
double kR = 0.009569377, kL = 0.009569377;//Encoder constants to convert to inches
double Y, X;

//Targets
float targetYaw = 0;
double distance_target = 0;

//Which command key are we using?
bool isCacheCommands = false;

//Cache variables
int nucleoCommandPin = A6;
int armPin = 12;
Servo arm;
bool backwards = false;

//Stores the PID constants for driving a distance and turning. [kP, kI, kD]
float turnPID[3] = {0.13, 0.0001, 0.005}; 
float distPID[3] = {0.17, 0.000, 0.003}; 

PIDController turningPID(0, turnPID);
PIDController distancePID(0, distPID);

//************************GYRO BLOCK*******************************//
double pollGyro(bool flag){
	Wire.beginTransmission(MPU_addr);
	Wire.write(71);  // starting with register 0x3B (ACCEL_XOUT_H)
	Wire.endTransmission(false);
	Wire.requestFrom(MPU_addr,2,true);  // request a total of 2 registers
	gyro = (~((Wire.read()<<8 | Wire.read())-1));
	//gyro = (((~(Wire.read()-1))<<8|(~(Wire.read()-1))));//*(250/32768));
	
	if(abs(gyro) < 10 && flag)
	{
		gyro = 0;
	}
	
	return gyro;
} 
//************END GYRO BLOCK************************************//

//********************Reset Functions****************/
void resetEncoders()
{
	leftEncoderPos = 0;
	rightEncoderPos = 0;
}

void resetGyro()
{
	yaw = 0;
}
//***************End Reset Functions****************/

//***********Encoders*************//

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
//******************End Encoders*************//

//***************Driving functions***************///Arcade Drive and filtering
double filter(double p)
{
  if(abs(p) < 0.1)
  {
    return 0;
  }
  return p;
}

void tankSteer(float turn_power)
{
	float right, left;
	 
	// Serial.println(turn_power);
	if(backwards)
	{
		if(targetYaw > 0)
		{
			right = 0.1;
			left = turn_power;
		}
		else if(targetYaw < 0)
		{
			right = -turn_power;
			left = 0.1;//this isnt 0 because it cant turn in place on backwards left turns for some reason
		}
		else
		{
			right = 0;
			left = 0;
		}
	}
	else
	{
		if(targetYaw < 0)
		{
			right = -0.1;
			left = turn_power;
		}
		else if(targetYaw > 0)
		{
			right = -turn_power;
			left = -0.1;//this isnt 0 because it cant turn in place on left turns for some reason
		}
		else
		{
			right = 0;
			left = 0;
		}
	}
	
	
	//Configure motors for directional driving
  if(left < 0)
  {
	digitalWrite(left_in_1, LOW);
	digitalWrite(left_in_2, HIGH); 
	
  }
  else if(left == 0)
  {
	digitalWrite(left_in_1, LOW);
	digitalWrite(left_in_2, LOW); 
  }
  else
  {
	digitalWrite(left_in_1, HIGH);
	digitalWrite(left_in_2, LOW); 
  }
  
  if(right < 0)
  {
	digitalWrite(right_in_1, LOW);
	digitalWrite(right_in_2, HIGH);
  }
  else if(right == 0)
  {
	digitalWrite(right_in_1, LOW);
	digitalWrite(right_in_2, LOW);
  }
  else
  {
	digitalWrite(right_in_1, HIGH);
	digitalWrite(right_in_2, LOW);
  }
  
  //Output to motors
  analogWrite(left_motor_pin, abs(left)  * 255);
  analogWrite(right_motor_pin, abs(right) * 255);
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
	digitalWrite(left_in_1, LOW);
	digitalWrite(left_in_2, HIGH); 
  }
  else if(left == 0)
  {
	digitalWrite(left_in_1, HIGH);
	digitalWrite(left_in_2, HIGH); 
	analogWrite(left_motor_pin, 255);
  }
  else
  {
	digitalWrite(left_in_1, HIGH);
	digitalWrite(left_in_2, LOW); 
  }
  
  if(right < 0)
  {
	digitalWrite(right_in_1, LOW);
	digitalWrite(right_in_2, HIGH);
  }
  else if(right == 0)
  {
	digitalWrite(right_in_1, HIGH);
	digitalWrite(right_in_2, HIGH);
	analogWrite(right_motor_pin, 255);
  }
  else
  {
	digitalWrite(right_in_1, HIGH);
	digitalWrite(right_in_2, LOW);
  }
  /*
	Serial.print(left);
	Serial.print("\t");
	Serial.println(right);
  */
  //Output to motors
  analogWrite(left_motor_pin, abs(left)  * 255);
  analogWrite(right_motor_pin, abs(right) * 255);
}
//********************End Driving Functions***************************/


//********Driving Routines****************///These functions set targets once a state change takes place
void forwardOne()
{
	backwards = false;
	isTurnInPlace = false;
	
	distance_target = FORWARD_DIST;
	targetYaw = DRIVE_STRAIGHT;
}

void leftTurn()
{
	backwards = false;
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = LEFT_TURN;
}

void rightTurn()
{
	backwards = false;
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = RIGHT_TURN;
}

void fullTurn()
{
	backwards = false;
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = FULL_TURN;
}

void idle(int ms)
{
	backwards = false;
	distance_target = 0;
	targetYaw = 0;
	delay(ms);
	
	driveComplete = true;
	turnComplete = true;
}

void goOneInch()
{
	backwards = false;
	isTurnInPlace = false;
	
	distance_target = 3.4;//just changed this from 3
	targetYaw = 0;
}

void backOne()
{
	backwards = true;
	isTurnInPlace = false;
	
	distance_target = BACKWARD_DIST;
	targetYaw = 0;
}

//Cache motions
void forwardShort()
{
	backwards = false;
	isTurnInPlace = false;
	
	distance_target = 6;
	targetYaw = 0;
}

void backHalf()
{
	backwards = true;
	isTurnInPlace = false;
	
	distance_target = -8;
	targetYaw = 0;
}

void openCache()
{
	arm.attach(armPin);
	
	backwards = false;
	distance_target = 2; //Setup for the camera
	targetYaw = 0;
	isTurnInPlace = false;
	
	//Open the lid
	arm.write(45);//Angle to set the arm so that it hits the lid is 45?
	delay(1000);
	arm.write(0);
	delay(250);
}

void camera()
{
	backwards = false;
	distance_target = 0;
	targetYaw = 0;
	isTurnInPlace = false;
	
	digitalWrite(nucleoCommandPin, HIGH);
	delay(2000);
	digitalWrite(nucleoCommandPin, LOW);
}

void undoLeftTurn()
{
	isTurnInPlace = true;
	
	//Invert tankSteer
	backwards = true;
	targetYaw = RIGHT_TURN;
	
	distance_target = 0;
}

void undoOneInch()
{
	backwards = true;
	isTurnInPlace = false;
	
	distance_target = -3.45;
	targetYaw = 0;
}

//**************End Driving Routines*********************/

//*************************Main Control Loop********************/
bool mainControlLoop()
{

	//Update the yaw of the robot
	yaw += float(((micros()-t)*(pollGyro(true)-calVal)))*gyroConvert;
    t = micros();
	
	//Find the error between setpoint and current yaw
    gyro_error = (yaw + GYRO_OFFSET) - targetYaw;
    
	//Wrap the gyro to the bounds
    if(gyro_error > 180)
    {
		gyro_error = -(360 - gyro_error); 
    }
	
	//Get the output variables based on PID Control
	//If normal driving operation is taking place
	if(!isTurnInPlace)
	{
		//Calculate forward power based on encoders
		X = distancePID.GetOutput(distance_target, distance); 
		
		//Calculate turning power based on gyro error Note that the PID must be inverted in the current setup
		Y = turningPID.GetOutput(0, gyro_error) * (-1);
		
		//If the robot is within a half inch of distance target and has slowed down
		if(abs(distance - distance_target) < 0.5 && abs(X) < STOP_SPEED_THRESHOLD)
		{
			//If this is the first time being in range
			if(!driveInRange)
			{
				//Start the timer
				driveTimerStart = millis();
				
				//Set to true to keep timer reference point
				driveInRange = true;
			}
			
			//Calculate time elapsed while on target
			driveTimerElapsed = millis() - driveTimerStart;
			
			//If we have been on target for a long enough amount of time, we have completed the drive
			if(driveTimerElapsed > setpointTimerThreshold)
			{
				driveComplete = true;
			}
		}
		//If the robot is outside the set target
		else
		{
			//Drive is not complete, we are not in range, reset the timer
			driveComplete = false;
			driveInRange = false;
			driveTimerStart = millis();
		}
	}
	else
	{
		//Calculate the turning power of the motors
		Y = turningPID.GetOutput(0, gyro_error) * (-1) * 0.8;
		
		//Since we are turning "in place", we dont care about encoders
		driveComplete = true;
		
		//Only turn, no forward motion
		X = 0;
	}
	
	//If the gyro error is low enough (dependent on driving forward or turning in place), and the robot is moving slow enough
	if(((abs(gyro_error) < 1.0 && isTurnInPlace) || (abs(gyro_error) < 1.0 && !isTurnInPlace)) && abs(Y) < STOP_SPEED_THRESHOLD)
	{
		//If this is the first time we have been at the angle goal, reset the timer
		if(!turnInRange)
		{
			turnTimerStart = millis();
			turnInRange = true;
		}
		
		//Calculate how long we have been at the target
		turnTimerElapsed = millis() - turnTimerStart;
		
		//If we have been at the target long enough, the turn is complete
		if(turnTimerElapsed > setpointTimerThreshold)
		{
			turnComplete = true;
		}
	}
	else
	{
		//Turn is incomplete, not in range, and reset the timer
		turnComplete = false;
		turnInRange = false;
		turnTimerStart = millis();
	}
	
	//If the root is not turning in place, use the regular driving mode
	if(!isTurnInPlace)
	{
		arcadeDrive(X,Y);
	}
	//Otherwise use tank steer to turn and move forwards since we cannot perform a legit 0 point turn
	else
	{
		tankSteer(Y);
	}
	
	maxTimerElapsed = millis() - maxTimerStart;
	
	if(maxTimerElapsed > maxTime)
	{
		driveComplete = true;
		turnComplete = true;
	}
	
	//Let the main loop know if the motion has completed or not
	return driveComplete && turnComplete;
	
}
//*****End Control Loops****/

//******************************STATE MGR BLOCK*********************************/
void state_mgr(int instructions){
          
		driveComplete = false;
		turnComplete = false;
		arcadeDrive(0, 0);
		
		delay(100);
		
		resetEncoders();
		resetGyro();
		
		//For max time timer
		maxTimerStart = millis();
		
		Serial.println(instructions);
        if(isCacheCommands)
		{
			switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         
				idle(1000);             //Sit still for a second
                break;
              case 1:                         
                forwardShort();				//Drive forward to the cache lid
                break;
              case 2:
                undoLeftTurn();				//Turn -90 degrees left
                break;
              case 3:
                openCache();				//Open the cache lid
                break;
              case 4:
				camera();				//Count the dots on the die
                break;
              case 5:	
				isCacheCommands = false;	//Switch to the other instruction set
				idle(50);				
                break;
              case 6:
				backHalf();               //Back up off the square that we are currently halfway on
                break;
              case 7:
				undoOneInch();             //Undo the motion before the left turn
                break;
              }
		}
		else
		{
			switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         
                idle(1000);                 //Sit still for a second
                break;
              case 1:                         
                forwardOne();				//Drive forward 1 square
                break;
              case 2:
                leftTurn();					//Turn 90 degrees left
                break;
              case 3:
                rightTurn();				//Turn 90 degrees right
                break;
              case 4:
				fullTurn();					//Do a full 180 degree turn
                break;
              case 5:	
				isCacheCommands = true;		//Switch to the other instruction set
				idle(50);
                break;
              case 6:
				goOneInch();                 //Inch forward to turn
                break;
              case 7:
				backOne();                 //Back up 1 square
                break;
              }
		}
		
		lastState = instructions;
		t = micros();
		stateMachine = MAIN_STATE;
        Serial.println("STATE_MGR");
}
//***********************************END STATE BLOCK******************************//


//*********************************SETUP BLOCK************************************//
void setup() //Initilizes some pins
{
	//Servo and nucleo
	pinMode(nucleoCommandPin, OUTPUT);
	digitalWrite(nucleoCommandPin, LOW);
	
	pinMode(armPin, OUTPUT);
	digitalWrite(armPin, HIGH);
	//arm.attach(armPin);
	//arm.write(0);
	
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
	distancePID.SetOutputRange(0.29, -0.29);
	turningPID.SetOutputRange(0.5, -0.5);
	
	//Encoder
	attachInterrupt(1, doRightEncoder, CHANGE); //pin 3 interrupt
	attachInterrupt(5, doLeftEncoder, CHANGE);
    
    //Communication pins
    pinMode(E, INPUT);  // state bit 0
    pinMode(F, INPUT);  // state bit 1
    pinMode(G, INPUT);  // state bit 2
    pinMode(instruct, INPUT); //lets the robot know that instructions on the state lines are valid; high when invalid; low when valid
    pinMode(moving, OUTPUT);  //asserts low when robot is moving, high when robot is stationary
    digitalWrite(moving, HIGH); //tells AI that the robot is not moving at this time
	
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(1);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x1B);  // Gyro
    Wire.write(0x00);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
	Wire.beginTransmission(MPU_addr);
    Wire.write(0x1A);  // Gyro
    Wire.write(0x01);     
    Wire.endTransmission(true);
	
	for(int count = 0; count < 100; count++)
	{
		Serial.println(pollGyro(false));
        calVal += pollGyro(false);
        delayMicroseconds(10);
    }
    calVal = calVal/float(100);
	
	Serial.print("CALVAL: \t");
	Serial.println(calVal);
    
    delay(10000);  //setup delay
	resetGyro();
}
//**************END SETUP BLOCK***************************************************



//*********************************************MAIN LOOP*********************************************
void loop() {
    
	distance = (kL*((double)leftEncoderPos) + (kR * ((double)rightEncoderPos)))/2;
	
	if(stateMachine == IDLE_STATE)
	{
		arcadeDrive(0,0);
		//setup intializes automatically in the arduino ide
		if(digitalRead(instruct)==HIGH )    //VERY IMPORTANT, loops while there are no instructions present; exits loop when instructions are asserted
		{
			
		}
		else
		{
			digitalWrite(moving,LOW);  //VERY IMPORTANT; disallows instructions, must be activated very quickly after the detection of a LOW instruction line
			Et = digitalRead(E);      //these statements input the state
			Ft = digitalRead(F);
			Gt = digitalRead(G);
			
			//***********Whatever code can be relatively safely placed anywhere between here and state_mgr without significantly affecting the bot's operation
			
			state_mgr(Et + 2*Ft + 4*Gt);    //sends the proper state, avoids use of bitshifting due to bugginess; state mgr will reset moving to HIGH when it is finished with its task
			
			stateMachine = MAIN_STATE;
		}
	}
    else
	{
		//note bot will iterate very quickly through states if it does not receive a proper state and enter the MCU function as currently coded
		//Execute motion based on command, check for completion
		isMotionFinished = mainControlLoop();

		delayMicroseconds(2000);

		//If motion is complete, go back to the idle state
		if(isMotionFinished)
		{
			Serial.println("Motion Complete!");
			digitalWrite(moving, HIGH); //VERY IMPORTANT, allows further instructions
			stateMachine = IDLE_STATE;
		}
      
	}
}
