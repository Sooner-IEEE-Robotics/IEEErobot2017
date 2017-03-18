
#include <Wire.h>

#define metal 3
//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;                     //comms lines
int moving = 8;                                               //comms lines
int indicator = 13;                                             //not used
int stateTest[37] = {4,2,4,3,0,0,0,4,2,0,0,0,4,2,0,0,0,4,2,0,0,4,2,0,0,4,2,0,4,2,0,4,2,4,2,0,5};    //test array for various states, bot will iterate through these states const int MPU_addr=0x68;  // I2C address of the MPU-6050                
int location[36] = {0,1,2,3,4,5,5,6,7,8,9,9,10,11,12,13,13,14,15,16,16,17,18,19,19,20,21,21,22,23,23,24,24,25};
int atlas[][2] = {{1,1},{2,1},{3,1},{4,1},{5,1},{5,2},{5,3},{5,4},{5,5},{4,5},{3,5},{2,5},{1,5},{1,4},{1,3},{1,2},{2,2},{3,2},{4,2},{4,3},{4,4},{3,4},{2,4},{2,3},{3,3}};
int cacheFound = 0;
void yellowDot(){
  Wire.beginTransmission(8);
  Wire.write(0);
  Wire.write(0);
  Wire.write(63);
  Wire.write(63);
  Wire.write(63);
  Wire.endTransmission(8);
  }

void randMcNally(int counted){
  Wire.beginTransmission(8);
  Wire.write(atlas[location[counted]][0]);
  Wire.write(atlas[location[counted]][1]);
  Wire.write(255);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission(8);
  
  
  
  }

void colombus(int count){
        digitalWrite(E, stateTest[count] & 1);                //for some reason, bit shifting works okay here? 
        //digitalWrite(indicator, stateTest[count] & 1);
        digitalWrite(F, stateTest[count]>>1 & 1);
        digitalWrite(G, stateTest[count]>>2 & 1);
        delay(10);                                            //allows time for the lines to actually be asserted as their written value
        digitalWrite(instruct,0);                             //tells the bot that instructions are ready
        while(digitalRead(moving) == 1) {}   //VERY IMPORTANT, waits until robot_mgr realizes that it has incoming data
        digitalWrite(instruct,1);            //disables the instruction line
        delay(10);                           //this delay probably not that important, but allows time for the instruct line to become the proper value before continuing
  }

void setup() {
  
 //Comms pins
  pinMode(E, OUTPUT);//bit 0        //output pins for the states
  pinMode(F, OUTPUT);//bit 1
  pinMode(G, OUTPUT);//bit 2
  pinMode(metal, INPUT);
  pinMode(moving, INPUT);  //low when moving, high when not moving
  pinMode(instruct, OUTPUT);  //low when info is valid, high when info is not valid
  pinMode(indicator, OUTPUT); //indicator led
  digitalWrite(instruct, HIGH);  //tells the other board that the info is not ready
  digitalWrite(indicator, LOW);  //pin 13 LED indicator
  digitalWrite(E, 0);            //initializes the state as state 0
  digitalWrite(F, 0);
  digitalWrite(G, 0);
  Wire.begin();
  
  delay(5000);
  yellowDot();
}

void loop() {
  Serial.begin(9600);
  
  // put your main code here, to run repeatedly: 
  for(int count = 0; count < sizeof(stateTest)/sizeof(int); count++){           //sends first 4 ints in the stateTest array
        while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
                  {Serial.println("Moving");}//do whatever 
        if((digitalRead(metal) == 1) && (count > 3)){
                randMcNally(count-4);
                if(cacheFound == 0){
                  colombus(36);
                  
                  cacheFound = 1;
                  }
            }
        if(count < 36){
        colombus(count);
        }
  }  
  
  while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}
