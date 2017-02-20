
int left_motor_pin = 6, left_in_1 = 9, left_in_2 = 10;
int right_motor_pin = 44, right_in_1 = 48, right_in_2 = 46;
int leftEncoderA = 18;
int leftEncoderB = 17;
int rightEncoderA = 3;
int rightEncoderB = 4;
int moving = 35;
int instruct = 30;   

int E = 11, F = 12, G = 13;
int leftEncoderPos = 0;
int rightEncoderPos = 0;
int Et = 0, Ft = 0, Gt = 0;

//**********************ENCODER BLOCK**************************************/
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



void resetEncoders()
            {
              leftEncoderPos = 0;
              rightEncoderPos = 0;
            }
//**************************END ENCODER BLOCK ****************************************/

//************************MOTOR BLOCK***************************//
void mcu(int right, int left){                      //prototype mcu function
           /*left_motor_pin     //enable/disable
           right_motor_pin    //enable/disable
           right_in_1
           right_in_2
           left_in_1
           left_in_2*/
           Serial.println("MCU");
           digitalWrite(right_motor_pin, HIGH);     //enables the motors
           digitalWrite(left_motor_pin, HIGH);
           switch(right){
              case -1:
                  digitalWrite(right_in_1, HIGH);      //reverse
                  digitalWrite(right_in_2, LOW);
                  break;
              case 1:
                  digitalWrite(right_in_2,HIGH);      //forward
                   digitalWrite(right_in_1,LOW);
                  break;
            
           }
           switch(left){
              case -1:
                  digitalWrite(left_in_1, HIGH);       //reverse
                  digitalWrite(left_in_2, LOW);
                  break;
              case 1:
                  digitalWrite(left_in_1,LOW);       //forward
                  digitalWrite(left_in_2,HIGH);
                  break;
           }
           delay(750);                             //allows the motors to run
           digitalWrite(right_motor_pin, LOW);     //disables the motors when leaving the MCU
           digitalWrite(left_motor_pin, LOW);      //disabling motors after moving avoids the need for an explicit idle state
           delay(1000);                            //allows time for the motors to turn off, makes individual states more visible
  
}
//********************END MOTOR BLOCK**********************************************//

//******************************STATE MGR BLOCK*********************************/
void state_mgr(int instructions){
          
         
          switch(instructions){//program enters the state and does whatever action it was told to do; currently 8 states available
              case 0:                         //state 0 
                  mcu(1,1);                   //these are of course practice instructions
                  Serial.println("MCU 0");
                break;
              case 1:                         //state 1, etc
                mcu(1,-1);
                Serial.println("MCU 1");
                break;
              case 2:
                mcu(-1,1);
                Serial.println("MCU 2");
                break;
              case 3:
                mcu(-1,-1);
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
