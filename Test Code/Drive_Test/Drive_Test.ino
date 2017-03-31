#include "PIDController.h"

//PID
double distPID[3] = {0.5, .0004, 0.0004}; 
PIDController distancePID(0, distPID);

//Navigation variables
int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10;
int right_motor_pin = 44, right_in_1 = 48, right_in_2 = 46;

//Encoder Pins
int leftEncoderA = 18;
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;

long leftEncoderPos = 0;
long rightEncoderPos = 0;

double distance_target = 20;
double distance = 0.0, gyro_error;
double kR = -0.00395, kL = -0.00418;//Encoder constants to convert to inches
double Y, X;

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

void arcadeDrive(double forward_power, double turn_power)
{
  double right, left;
  
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

void setup() 
{
  Serial.begin(9600);
  
   pinMode(leftEncoderA, INPUT);
  pinMode(leftEncoderB, INPUT);
  digitalWrite(leftEncoderA, HIGH);//pull up resistor
  digitalWrite(leftEncoderB, HIGH);//pull up resistor
  
  pinMode(rightEncoderA, INPUT);
  pinMode(rightEncoderB, INPUT);
  digitalWrite(rightEncoderA, HIGH);//pull up resistor
  digitalWrite(rightEncoderB, HIGH);//pull up resistor

  attachInterrupt(1, doRightEncoder, CHANGE); //pin 3 interrupt
  attachInterrupt(5, doLeftEncoder, CHANGE);
  
   //Motor Initialization
  pinMode(left_motor_pin, OUTPUT);
  pinMode(right_motor_pin, OUTPUT);
  pinMode(right_in_1, OUTPUT);
  pinMode(right_in_2, OUTPUT);
  pinMode(left_in_1, OUTPUT);
  pinMode(left_in_2, OUTPUT);

  distancePID.SetOutputRange(0.5, -0.5);
}

void loop() {

  Serial.print(leftEncoderPos);
  Serial.print("\t");
  Serial.print(rightEncoderPos);
  Serial.print("\t");
   
  distance = (kL*((double)leftEncoderPos) + (kR * ((double)rightEncoderPos)))/2;
  Serial.println(distance);

  X = distancePID.GetOutput(distance_target, distance); //Calculate the forward power of the motors

  if(abs(X) < 0.1)
  {
    X = 0;
  }

 // Serial.println(distance);
  /*
  Serial.print(rightEncoderPos);
  Serial.print("\t");
  Serial.println(leftEncoderPos);
  */
 // arcadeDrive(X, 0);

	return;
}
