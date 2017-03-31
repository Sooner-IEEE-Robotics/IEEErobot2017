#include "Wire.h"
//A0 on MPU6050 set to 0
//6050 7 bit address is 0x68
//PWMGT address is 107; bit 6 controls sleep mode
//z axis gryo is 71 and 72; low is 72, high is 71
int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10;
int right_motor_pin = 44, right_in_1 = 48, right_in_2 = 46;
int leftEncoderA = 18;
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;
int moving = 35;
int instruct = 30;   
const int MPU_addr=0x68;  // I2C address of the MPU-6050
double gyro = 0;
int E = 11, F = 12, G = 13;
volatile int leftEncoderPos = 0;
volatile int rightEncoderPos = 0;
int Et = 0, Ft = 0, Gt = 0;
double kR = 0.00395, kL = 0.00418;
long int t = millis();
double pos = 0;
double distance = 0, rotation = 0;

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
//**********************ENCODER BLOCK**************************************/
void doLeftEncoder()
            {//Serial.println("HERE1");
                leftEncoderPos++;
            }
void doRightEncoder()
            {//Serial.println("HERE2");
            rightEncoderPos++;
            }




//**************************END ENCODER BLOCK ****************************************/

//************************MOTOR BLOCK***************************//
void mcu(int dist, int rot){                      //prototype mcu function, dist in inches
           /*left_motor_pin     //enable/disable
           right_motor_pin    //enable/disable
           right_in_1
           right_in_2
           left_in_1
           left_in_2*/
           float error = 0, errorI = 0;
           int PID = 0;
           float P = 1/50; 
           distance = 0; 
           rotation = 0;
           int rotBase = 85;
           int distBool = 0;
           float I = 0.00005;
           float testing = 100000;
           Serial.println("MCU");
           //digitalWrite(right_motor_pin, HIGH);     //enables the motors
           //digitalWrite(left_motor_pin, HIGH);
           leftEncoderPos = 0; rightEncoderPos = 0;
           if(dist > 0) {distBool = 1;}
           else if(dist < 0){distBool = -1;}
           
           //Serial.print("DIST: ");
           //Serial.println(dist);
           distance = dist / kR;
           //Serial.print("DISTANCE: "); Serial.println(distance);
           
           //Serial.print("ERROR: "); Serial.println(error);
           switch(distBool){
              
              case -1:
                  digitalWrite(right_in_1, LOW);      //reverse
                  digitalWrite(right_in_2, HIGH);
                  digitalWrite(left_in_1, LOW);                       
                  
                  digitalWrite(left_in_2, HIGH);
                  distance = -distance;
                  error = distance - leftEncoderPos;
                  Serial.print("ENCODER1: "); 
                  Serial.println(distance);
                  Serial.print(leftEncoderPos); Serial.println("LEFT ENCODER POS");
                  while(error > 0)
                      {error = distance - leftEncoderPos;
                       errorI += error; 
                       PID = int(20 + P * error + I * errorI * error); 
                       analogWrite(right_motor_pin, PID);
                       analogWrite(left_motor_pin, PID);
                       Serial.print("ENCODER1"); 
                       Serial.println(error);
                      }
                  
                  
                  break;
              case 1:
                   error = distance - leftEncoderPos;
                   digitalWrite(right_in_1,HIGH);      //forward
                   digitalWrite(right_in_2,LOW);
                   digitalWrite(left_in_1, HIGH);                        
                   digitalWrite(left_in_2, LOW);
                   Serial.print("ENCODER2"); 
                  Serial.println(distance);
                   while(error > 0)
                      {error = distance - leftEncoderPos;
                       errorI += error; 
                       PID = int(20 + P * error + I * errorI * error); 
                       analogWrite(right_motor_pin, PID);
                       analogWrite(left_motor_pin, PID);
                       Serial.print("ENCODER2"); 
                       Serial.println(error);
                      }
                   
                  
                  break;
            
           }
           switch(rot){
              case -90:
                  digitalWrite(right_in_1, LOW);      
                  digitalWrite(right_in_2, HIGH);
                  digitalWrite(left_in_1, HIGH);                        //left
                  digitalWrite(left_in_2, LOW);

                   t = micros();
                  error = rotBase + rotation;
                    while(error > 0){
                        rotation += float((micros()-t)*(pollGyro()-6)/testing)*(250.0/32768.0);//-.05;//double(micros() - t)/testing * pollGyro();  //gyro goes negative here
                        t = micros();
                        error = rotBase + rotation;
                        errorI += error;
                        PID = int(20 + error + I * errorI * error);
                        if (PID > 255){PID = 255;}
                        analogWrite(right_motor_pin, PID);
                        analogWrite(left_motor_pin, PID);
                        //Serial.println(rotation);
                        delayMicroseconds(2000);
                    }
                  break;
              case 90:                                                //right
                  digitalWrite(right_in_1, HIGH);      
                  digitalWrite(right_in_2, LOW);
                  digitalWrite(left_in_1,LOW);       
                  digitalWrite(left_in_2,HIGH);
                  t = micros();
                  error = rotBase - rotation;
                    while(error > 0){
                        rotation += float((micros()-t)*(pollGyro()-6)/testing)*(250.0/32768.0);//-.05;//double(micros() - t)/testing * pollGyro();
                        t = micros();
                        error = rotBase - rotation;
                        errorI += error;
                        PID = int(20 + error + I * errorI * error);
                        if (PID > 255){PID = 255;}
                        analogWrite(right_motor_pin, PID);
                        analogWrite(left_motor_pin, PID);
                        //Serial.println(rotation);
                        delayMicroseconds(2000);
                       
                  }
                  break;
           }
           //delay(750);                             //allows the motors to run
           digitalWrite(right_motor_pin, LOW);     //disables the motors when leaving the MCU
           digitalWrite(left_motor_pin, LOW);      //disabling motors after moving avoids the need for an explicit idle state
           delay(2000);                            //allows time for the motors to turn off, makes individual states more visible
  
}
//********************END MOTOR BLOCK**********************************************//

//******************************STATE MGR BLOCK*********************************/
void state_mgr(int instructions){
          
         
          switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         //forward 12" 
                  mcu(12,0);                   //these are of course practice instructions
                  Serial.println("MCU 0");
                break;
              case 1:                         //backwards 12"
                mcu(-12,0);
                Serial.println("MCU 1");
                break;
              case 2:                         //turn right; turn right is +90 degrees
                mcu(0,90);
                Serial.println("MCU 2");
                break;
              case 3:                         //turn left; turn left is -90 degrees
                mcu(0,-90);
                Serial.println("MCU 3");
                break;
              case 4:
                break;
              case 5:
                break;
              case 6:
                break;
              case 7:
                break;
              }
           
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
            
            //Communication pins
            pinMode(E, INPUT);  // state bit 0
            pinMode(F, INPUT);  // state bit 1
            pinMode(G, INPUT);  // state bit 2
            pinMode(instruct, INPUT); //lets the robot know that instructions on the state lines are valid; high when invalid; low when valid
            pinMode(moving, OUTPUT);  //asserts low when robot is moving, high when robot is stationary
            digitalWrite(moving, HIGH); //tells AI that the robot is not moving at this time
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
            delay(5000);  //setup delay


}
//**************END SETUP BLOCK***************************************************



//*********************************************MAIN LOOP*********************************************
void loop() {
              
              
        
  
              
              
              //setup intializes automatically in the arduino ide
              while(digitalRead(instruct)==HIGH )    //VERY IMPORTANT, loops while there are no instructions present; exits loop when instructions are asserted
              {}
                digitalWrite(moving,LOW);  //VERY IMPORTANT; disallows instructions, must be activated very quickly after the detection of a LOW instruction line
                Et = digitalRead(E);      //these statements input the state
                Ft = digitalRead(F);
                Gt = digitalRead(G);
               
                
                //***********Whatever code can be relatively safely placed anywhere between here and state_mgr without significantly affecting the bot's operation
                
         
                
                state_mgr(Et + 2*Ft + 4*Gt);    //sends the proper state, avoids use of bitshifting due to bugginess; state mgr will reset moving to HIGH when it is finished with its task
                                                //note bot will iterate very quickly through states if it does not receive a proper state and enter the MCU function as currently coded
                
                
                
                //********code could theoretically go here safely as well, although this is after the bot has moved
                
                
                
                digitalWrite(moving, HIGH); //VERY IMPORTANT, allows further instructions
                
             
    















 

}
