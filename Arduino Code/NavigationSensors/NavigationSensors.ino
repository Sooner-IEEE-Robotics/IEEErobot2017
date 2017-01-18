#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

//The device itself
MPU6050 mpu;

//Variables required to have a gyro reset
bool firstRun = true;
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

//Encoder Pins
int leftEncoderA = 3;
int leftEncoderB = 4;
int rightEncoderA = 5;
int rightEncoderB = 6;

int leftEncoderPos = 0;
int rightEncoderPos = 0;

int n = LOW, m = LOW;
int leftEncoderLast = LOW;
int rightEncoderLast = LOW;

//We need interrupts to make sure we have the most accurate data
volatile bool mpuInterrupt = false;// indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void resetGyro()
{
  reference[0] = ypr[0];
  reference[1] = ypr[1];
  reference[2] = ypr[2];
}

void setup() 
{
  pinMode(leftEncoderA, INPUT);
  pinMode(leftEncoderB, INPUT);
  pinMode(rightEncoderA, INPUT);
  pinMode(rightEncoderB, INPUT);
  
  Wire.begin();
  Serial.begin(115200);
  
  //Gyro initialization
  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
  devStatus = mpu.dmpInitialize();
  
  if(devStatus == 0)
  {
    mpu.setDMPEnabled(true);
    
    //Attaches the interrupt used for detecting new data
    attachInterrupt(0, dmpDataReady, RISING);
    
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    Serial.println("Error connecting to MPU6050");
  }
}

void loop() 
{
  if(!dmpReady) //Die if there are errors
  {
    Serial.println("RIP: There were errors.");
    return;
  }
  
  //while the gyro isn't giving any data, do other things
  while(!mpuInterrupt && packetSize >= fifoCount)
  {
    n = digitalRead(leftEncoderA);
    m = digitalRead(rightEncoderA);
    
    if ((leftEncoderALast == LOW) && (n == HIGH)) 
    {
      if (digitalRead(leftEncoderB) == LOW) 
      {
        leftEncoderPos--;
      } 
      else 
      {
        leftEncoderPos++;
      }
    } 
    leftEncoderALast = n;
    
    if ((rightEncoderALast == LOW) && (n == HIGH)) 
    {
      if (digitalRead(rightEncoderB) == LOW) 
      {
        rightEncoderPos--;
      } 
      else 
      {
        rightEncoderPos++;
      }
    } 
    rightEncoderALast = n;
  }
  
  // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
  
  fifoCount = mpu.getFIFOCount();
  
  //check for overflow (should not overflow often)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
  {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));
    } 
  else if (mpuIntStatus & 0x02) // otherwise, check for DMP data ready interrupt (this should happen frequently)
  {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize)
    {
      fifoCount = mpu.getFIFOCount();
    }     

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

    //Get Yaw, Pitch and Roll
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    
    /*Show data
    Serial.print("YPR\t");
    Serial.print(ypr[0] * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180/M_PI);
    Serial.print("\t");
    Serial.println(ypr[2] * 180/M_PI);
    */
    
    Serial.print("YAW: \t");
    Serial.println((ypr[0] - reference[0]) * 180 / M_PI);
    
    if(firstRun)
    {
      resetGyro();
      firstRun = false;
    }
  }
}
