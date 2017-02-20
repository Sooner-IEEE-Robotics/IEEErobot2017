//Message System (OUT)
int E = 9, F = 10, G = 11, instruct = 12;
int moving = 8;
int indicator = 13;
int stateTest[4] = {0,1,2,3};    //test array for various states, bot will iterate through these states 

void setup() {
  
 //Comms pins
  pinMode(E, OUTPUT);//bit 0        //output pins for the states
  pinMode(F, OUTPUT);//bit 1
  pinMode(G, OUTPUT);//bit 2
  pinMode(moving, INPUT);  //low when moving, high when not moving
  pinMode(instruct, OUTPUT);  //low when info is valid, high when info is not valid
  pinMode(indicator, OUTPUT); //indicator led
  digitalWrite(instruct, HIGH);  //tells the other board that the info is not ready
  digitalWrite(indicator, LOW);  //pin 13 LED indicator
  digitalWrite(E, 0);            //initializes the state as state 0
  digitalWrite(F, 0);
  digitalWrite(G, 0);
  delay(5000);
}

void loop() {
  Serial.begin(9600);
  
  // put your main code here, to run repeatedly: 
  for(int count = 0; count < 4; count++){           //sends first 4 ints in the stateTest array
        while(digitalRead(moving) == LOW)           //VERY IMPORTANT, waits until the robot_mgr is no longer actively moving the robot to assert more instructions
                  {}//do whatever 
        
        //Serial.println((stateTest[count] & 1)+(stateTest[count]>>1 & 1)+(stateTest[count]>>2 & 1));
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
  
  while(1){}  //terminate the program in an infinite loop, allows quick retesting of the robot_mgr
}
