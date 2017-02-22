#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include "SPI.h"
#include "PIDController.h"

double FORWARD_DIST = 20;
float LEFT_TURN  = -90;
float RIGHT_TURN = 90;
float FULL_TURN = 180;

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


//The gyro device itself
MPU6050 mpu;
float reference[3]; //[yaw, pitch, roll]

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

int leftEncoderPos = 0;
int rightEncoderPos = 0;
float yaw = 0;

int n = LOW, m = LOW;
int leftEncoderALast = LOW;
int rightEncoderALast = LOW;

int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10;
int right_motor_pin = 44, right_in_1 = 48, right_in_2 = 46;
int leftEncoderA = 18;
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;
int moving = 35;
int instruct = 30;   

int E = 11, F = 12, G = 13;
int Et = 0, Ft = 0, Gt = 0;


double distance = 0.0, gyro_error = 0;
double kR = -0.00395, kL = 0.00418;//Encoder constants to convert to inches
double Y, X;

//Targets
float targetYaw = 0;
double distance_target = 0;

//Stores the PID constants for driving a distance and turning. [kP, kI, kD]
float turnPID[3] = {0.15, 0.0001, 0.0002};
float distPID[3] = {0.30, 0.0001, 0.0010}; 

PIDController turningPID(0, turnPID);
PIDController distancePID(0, distPID);


/*
	Reset Functions
*/
void resetEncoders()
{
	leftEncoderPos = 0;
	rightEncoderPos = 0;
}

void resetGyro()
{
  reference[0] = ypr[0];
  reference[1] = ypr[1];
  reference[2] = ypr[2];
}
/*
	End Reset Functions
*/

/*
	Interrupt Functions
*/

//We need interrupts to make sure we have the most accurate data
volatile bool mpuInterrupt = false;// indicates whether MPU interrupt pin has gone high
void dmpDataReady() 
{
    mpuInterrupt = true;
}

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

/*
	Driving functions
	Arcade Drive and filtering
*/
double filter(double p)
{
  if(abs(p) < 0.1)
  {
    return 0;
  }
  return p;
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

/*
	Driving Routines
	These functions set targets once a state change takes place
*/
void forwardOne()
{
	isTurnInPlace = false;
	
	distance_target = FORWARD_DIST;
	targetYaw = 0;
}

void leftTurn()
{
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = LEFT_TURN;
}

void rightTurn()
{
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = RIGHT_TURN;
}

void fullTurn()
{
	isTurnInPlace = true;
	
	distance_target = 0;
	targetYaw = FULL_TURN;
}

void cacheSequenceClosedLoop()
{
	
}

void idle(int ms)
{
	distance_target = 0;
	targetYaw = 0;
	delay(ms);
	
	driveComplete = true;
	turnComplete = true;
}
/*
	End Driving Routines
*/

/*
	Control Loops
*/
bool mainControlLoop()
{
	if((!driveComplete || !turnComplete))
	{	
		//If the robot is within a half inch of distance target, stop
		if(abs(distance - distance_target) < 0.5)
		{
			X = 0;
			driveComplete = true;
		}
	
		//Get the output variables based on PID Control
		if(!isTurnInPlace)
		{
			X = distancePID.GetOutput(distance_target, distance); //Calculate the forward power of the motors
		}
		else
		{
			driveComplete = true;
			X = 0;
		}
		
		Y = turningPID.GetOutput(0, gyro_error); //Calculate the turning power of the motors
	
		//Don't output if the output won't move the robot (save power)
		X = filter(X);
		Y = filter(Y);
		
		if(is_nav_debug)
		{
			Serial.print(X);
			Serial.print(" ");
			Serial.println(Y);
		}
	
		//if we are only off by 1.5 degrees, dont turn
		if(abs(gyro_error) < 1.5)
		{
			Y = 0;
			turnComplete = true;
		}
		else
		{
			turnComplete = false;
		}
		
		arcadeDrive(X,Y);
		
		if(is_nav_debug)
		{
			//Serial.print("X: ");
			//Serial.print(X);
			//Serial.print("\tY: ");
			//Serial.print(Y);
			//Serial.print("\tYAW: \t");
			//Serial.print(yaw);
			//Serial.print("\tYAW ERROR: \t");
			//Serial.println(gyro_error);
			//Serial.print("\tDistance: ");
			//Serial.println(distance);
		}
		
		return false;
	}
	else
	{
		return true;
	}
}
/*
	End Control Loops
*/
//******************************STATE MGR BLOCK*********************************/
void state_mgr(int instructions){
          
		driveComplete = false;
		turnComplete = false;
		arcadeDrive(0, 0);
		
		delay(100);
		
		resetEncoders();
         
          switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         //state 0 
                  idle(1000);                   //these are of course practice instructions
                  Serial.println("MCU 0");
                break;
              case 1:                         //state 1, etc
                forwardOne();
                Serial.println("MCU 1");
                break;
              case 2:
                leftTurn();
                Serial.println("MCU 2");
                break;
              case 3:
                rightTurn();
                Serial.println("MCU 3");
                break;
              case 4:
				fullTurn();
                break;
              case 5:
				idle(1000);
                break;
              case 6:
			  idle(1000);
                break;
              case 7:
			  idle(1000);
                break;
              }
           
		   stateMachine = MAIN_STATE;
            Serial.println("STATE_MGR");
}
//***********************************END STATE BLOCK******************************//


//*********************************SETUP BLOCK************************************//
void setup() //Initilizes some pins
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
    attachInterrupt(1, doRightEncoder, CHANGE); //pin 3 interrupt
    attachInterrupt(5, doLeftEncoder, CHANGE);  
    delay(5000);  //setup delay
}
//**************END SETUP BLOCK***************************************************



//*********************************************MAIN LOOP*********************************************
void loop() {
    
	distance = ((double)(kL*leftEncoderPos) + (double)(kR * rightEncoderPos))/2;
	
	if(stateMachine == IDLE_STATE)
	{
		arcadeDrive(0,0);
		//setup intializes automatically in the arduino ide
		if(digitalRead(instruct)==HIGH )    //VERY IMPORTANT, loops while there are no instructions present; exits loop when instructions are asserted
		{}
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

      //********code could theoretically go here safely as well, although this is after the bot has moved
      isMotionFinished = mainControlLoop();
	  
      if(isMotionFinished)
	  {
		  digitalWrite(moving, HIGH); //VERY IMPORTANT, allows further instructions
		  stateMachine = IDLE_STATE;
	  }
      
	}
}
