float turnPID[3], distancePID[3]; //Stores the PID constants for driving a distance and turning. [kP, kI, kD]
float turnIntegrator = 0, distIntegrator = 0;
float Y, X;

float PIDControl(float *k, float setpoint, float process, float delta_process, float integrator)
{
  float error = setpoint - process;
  float P = k[0] * error;

  float I = (integrator + error
}

void arcadeDrive(float y_power, float turn_power)
{
  float right, left;
  
  if(y_power > 0)
  {
    if(turn_power > 0)
    {
      right = y_power - turn_power;
      left = max(y_power, turn_power);
    }
    else
    {
      right = max(y_power, -turn_power);
      left = y_power + turn_power;
    }
  }
  else
  {
    if(turn_power > 0)
    {
      right = (-1) * max(-y_power, turn_power);
      left = y_power + turn_power;
    }
    else
    {
      right = y_power - turn_power;
      left = (-1) * max(-turn_power, -y_power);
    }
  }
  
  //Output to motors
}

void setup() 
{
  //Initialize the PID constants for distance
  distancePID[0] = 1.0;
  distancePID[1] = 0.001;
  distancePID[2] = 0.0;

  //Initialize the PID constants for turning
  turnPID[0] = 1.0;
  turnPID[1] = 0.001;
  turnPID[2] = 0.0;
}

void loop() 
{
  Y = PIDControl(distancePID,0,0); // Calculate the forward power of the motors
  X = PIDControl(turnPID,0,0);
}
