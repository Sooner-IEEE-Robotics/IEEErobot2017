#include <Wire.h>
#include <Servo.h>

//Astronomicon
//A0 on MPU6050 set to 0
//6050 7 bit address is 0x68
//PWMGT address is 107; bit 6 controls sleep mode
//z axis gryo is 71 and 72; low is 72, high is 71
#define pictureTake A6

int left_motor_pin = 6, left_in_1 = 10, left_in_2 = 9;           //motor controller pins
int right_motor_pin = 2, right_in_1 = 46, right_in_2 = 48;       //motor controller pins
int leftEncoderA = 18;                                              //encoder pins
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;
int moving = A4;                                                //comm line
int instruct = A0;                                              //comm line
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int gyro = 0;
int E = A3, F = A2, G = A1;
volatile long int leftEncoderPos = 0;
volatile long int rightEncoderPos = 0;
int Et = 0, Ft = 0, Gt = 0;
long int t = millis();
int pos = 0;
float calVal = 0;

Servo myservo;  // create servo object to control a servo 

//************************GYRO BLOCK*******************************//
double pollGyro(){
 Wire.beginTransmission(MPU_addr);
 Wire.write(71);  // starting with register 0x3B (ACCEL_XOUT_H)
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,2,true);  // request a total of 2 registers
 gyro = (~((Wire.read()<<8 | Wire.read())-1));
 return(gyro);
} 
//************END GYRO BLOCK************************************//
//**********************ENCODER AND SERVO BLOCK**************************************/
void doLeftEncoder()
            {//Serial.println("HERE1");
                leftEncoderPos++;
            }
void doRightEncoder()
            {//Serial.println("HERE2");
            rightEncoderPos++;
            }
            
void servoCodeTotallyNotCopied(){
  for(pos = 0; pos <= 51; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  delay(500); 
  for(pos = 51; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  
}



//**************************END ENCODER BLOCK ****************************************/

//************************MOTOR BLOCK***************************//
void mcu(float dist, float rot){                      //prototype mcu function, dist in inches
           float distance = 0, rotation = 0, kE = .002719;
           float voltage = analogRead(A5)/float(700);//615 to 680
           int pidRight = 0; int pidLeft = 0;     //separate pid for each motor
           float error = 0; float errorI = 0;     //error terms
           float errorR = 0; float errorD = 0; float errorRI = 0; float errorDI = 0;
           float P = 6; float I = .04; float IR = .04; float PR = 1.5;
           //+distance = forward that amount, -distance = backwards that amount
           //+rotation = turn right, -rotation = turn left
           if(abs(dist) > 0){//checks to see if distance was requested
                    if(dist > 0){//checks to see if reverse is desired
                                            digitalWrite(left_in_1, LOW);         //forward
                                            digitalWrite(left_in_2, HIGH);
                                            digitalWrite(right_in_1, LOW);
                                            digitalWrite(right_in_2, HIGH);}
                          else if(dist < 0){//checks to see if reverse
                                            digitalWrite(left_in_1, HIGH);         //reverse
                                            digitalWrite(left_in_2, LOW);
                                            digitalWrite(right_in_1, HIGH);
                                            digitalWrite(right_in_2, LOW);}
                  }

           if(abs(rot) > 0){//checks to see if rotation was requested
            //Serial.println("rotation requested");
                  if(rot < 0){//checks to see if we want to go left
                                            //digitalWrite(left_in_1, HIGH);         //left
                                            //digitalWrite(left_in_2, LOW);
                                            digitalWrite(right_in_1, LOW);
                                            digitalWrite(right_in_2, HIGH);}
                    
                  else if(rot > 0){//checks to see if we want to go right
                                            digitalWrite(left_in_1, LOW);         //right
                                            digitalWrite(left_in_2, HIGH);
                                            //Serial.println("left_in_1");
                                            //digitalWrite(right_in_1, HIGH);
                                            //digitalWrite(right_in_2, LOW);
                    
                    }
           }
           error = abs(dist) + abs(rot);
           while(abs(error) > .1){         //PID loop
              if(abs(dist) > 0){
                  t = micros();//sets beginning time of PID loop
                  error = (abs(dist)-distance);//+abs(rot-rotation);
                  errorR = rot - rotation;
                  errorD = abs(dist) - distance;
                  errorRI += IR * errorR; //integrated error term
                  errorDI += errorD;
                  
                  rotation +=(float(2000)*(pollGyro()-calVal)* (float(250)/ (float(32768 * 1000000*18.3889))));//tracks rotation, + is right, - is left
                  distance = ((leftEncoderPos + rightEncoderPos)*kE)/float(2.0);//tracks encoder distance
                  
                  pidLeft = abs( P * (errorD) + I * (errorDI));//tracks left pid
                  pidRight = abs( P * (errorD) + I * (errorDI));//tracks right pid

                  if(pidRight > 128){pidRight = 128;} if(pidLeft > 128){pidLeft = 128;}  //limits speed of the PID
                  //pidLeft = int(pidLeft - 14 * rotation);//tracks left pid
                  pidRight = int(pidRight + 14 * rotation);//tracks right pid
                  if(pidRight < 0){pidRight = 0;} if(pidLeft < 0){pidLeft = 0;}
                  //Serial.println(pidLeft);
                  //Serial.println(pidRight);
              }//end if dist

              else if(abs(rot) > 0){
                  t = micros();//sets beginning time of PID loop
                  error = (abs(rot)-abs(rotation));//+abs(rot-rotation);
                  errorR = abs(rot) - abs(rotation);
                  
                  errorRI += IR * errorR; //integrated error term
                  
                  
                  rotation +=(float(2000)*(pollGyro() * 7.8 -calVal)* (float(250)/ (float(32768 * 1000000*18.3889))));//tracks rotation, + is right, - is left
                  pidLeft = abs( PR * (errorR) + IR * (errorRI));//tracks left pid
                  pidRight = abs( PR * (errorR) + IR * (errorRI));//tracks right pid
                  
                  if(pidRight > 128){pidRight = 128;} if(pidLeft > 128){pidLeft = 128;}  //limits speed of the PID
                  if(pidRight < 0){pidRight = 0;} if(pidLeft < 0){pidLeft = 0;}
                  //Serial.println((.92+(.02*(1 - voltage))) * pidLeft);
                    }//end if rot
             analogWrite(right_motor_pin, pidRight);    //writes motor power
             analogWrite(left_motor_pin,((.92+(.02*(1 - voltage)))) * pidLeft);   //accomadates for one motor being overpowered
             while(micros() - t < 2000){}//ensures PID loop only lasts 2ms 
              }
                  

           
                                            digitalWrite(left_in_1, HIGH);    //brake         
                                            digitalWrite(left_in_2, HIGH);
                                            digitalWrite(right_in_1, HIGH);
                                            digitalWrite(right_in_2, HIGH);
           digitalWrite(right_motor_pin, HIGH);     //brakes the motors when leaving the MCU
           digitalWrite(left_motor_pin, HIGH);      //disabling motors after moving avoids the need for an explicit idle state
           
           delay(2000);                            //allows time for the motors to turn off, makes individual states more visible
           leftEncoderPos = 0; rightEncoderPos = 0;
           digitalWrite(right_motor_pin, LOW);     //disables the motors when leaving the MCU
           digitalWrite(left_motor_pin, LOW);
}
//********************END MOTOR BLOCK**********************************************//

//******************************STATE MGR BLOCK*********************************/
void state_mgr(int instructions){
          
         
          switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         //forward 12" 
                  mcu(2.83,0);                   //these are of course practice instructions
                  Serial.println("MCU 0");
                break;
              case 1:                         //backwards 12"
                mcu(-2.83,0);
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
                mcu(.7641,0);
                Serial.println("MCU 4");
                break;
              case 5:
                state_mgr(1);//backup
                state_mgr(4); state_mgr(3);//turn left
                state_mgr(4);//inch forward
                servoCodeTotallyNotCopied(); //runs the servo
                state_mgr(4);//forward more
                digitalWrite(pictureTake, HIGH); delay(1000); digitalWrite(pictureTake, LOW); delay(2000); //take picture
                state_mgr(1);//backup
                state_mgr(4); state_mgr(2); state_mgr(4); state_mgr(2); state_mgr(4); state_mgr(3);//left, left, right
                
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
            myservo.attach(12);  // attaches the servo on pin 9 to the servo object 
            myservo.write(0);
            
            Serial.begin(230400);
            //encoder initialization
            pinMode(pictureTake, OUTPUT);
            digitalWrite(pictureTake, LOW);
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
            attachInterrupt(1, doRightEncoder, HIGH); //pin 3 interrupt
            attachInterrupt(5, doLeftEncoder, HIGH);  
            Wire.begin();
            Wire.beginTransmission(MPU_addr);
            Wire.write(0x6B);  // PWR_MGMT_1 register
            Wire.write(1);     // set to zero (wakes up the MPU-6050)
            Wire.endTransmission(true);
            Wire.beginTransmission(MPU_addr);
            Wire.write(0x1B);  // Gyro
            Wire.write(0x18);     // set to zero (wakes up the MPU-6050)
            Wire.endTransmission(true);
            for(int count = 0; count < 100; count++){
                  calVal += pollGyro();
                  delayMicroseconds(10);
            }
            calVal = calVal/float(100);
  
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
               
                Serial.println("error");
                //***********Whatever code can be relatively safely placed anywhere between here and state_mgr without significantly affecting the bot's operation
                
         
                
                state_mgr(Et + 2*Ft + 4*Gt);    //sends the proper state, avoids use of bitshifting due to bugginess; state mgr will reset moving to HIGH when it is finished with its task
                                                //note bot will iterate very quickly through states if it does not receive a proper state and enter the MCU function as currently coded
                
                
                
                //********code could theoretically go here safely as well, although this is after the bot has moved
                
                
                
                digitalWrite(moving, HIGH); //VERY IMPORTANT, allows further instructions
                
             
    















 

}
