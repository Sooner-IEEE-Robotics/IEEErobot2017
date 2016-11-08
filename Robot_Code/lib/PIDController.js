//How to use this:
//Step 1: Create the object
//Step 2: Set the Target
//Step 3: Initialize the PID with init()
//Step 4: Run getOutput() every loop


var target;
var Kp, Ki, Kd;
var error;
var min, max;

var lastTime, lastProcessVar;

//Standard PID for PWM control
var PIDController = function(target)
{
	this.target = target;
	this.min = -1;
	this.max = 1;
};

//Set your own min and max
var PIDController = function(target, min, max)
{
	this.target = target;
	this.min = min;
	this.max = max;
};

//Sets proportional constant, Kp
PIDController.prototype.setP(P)
{
	this.Kp = P;
}

//Sets integral constant, Ki
PIDController.prototype.setI(I)
{
	this.Ki = I;
}

//Sets derivative constant, Kd
PIDController.prototype.setD(D)
{
	this.Kd = D;
}

//Set the target value
PIDController.prototype.setTarget = function(target)
{
	this.target = target;
}

//Initialize the PID so we can begin getting output. 
PIDController.prototype.init = function(processVariable)
{
	this.lastProcessVar = processVariable;
}

//Compute the PID output.
PIDController.prototype.getOutput = function(processVariable)
{
	//Current Error
	this.error = this.target - processVariable;
	
	//Calculate the Proprotional component
	var P = this.error * this.Kp;
	
	//Integrate to find past error
	var integrator += this.error;
	
	//Contain integrated error between output values
	if(integrator > this.max)
	{
		integrator = this.max;
	}
	if(integrator < this.min)
	{
		integrator = this.min;
	}
	//Calculate the Integral Component
	var I = this.Ki * integrator;

	//Find change in position since last sample
	var processVarDelta = processVariable - this.lastProcessVar;
	
	//Calculate the derivative component
	var D = this.Kd * processVarDelta;
	
	//Sum up the components to find output
	var output = P + I + D;
	
	//coerce output to output range
	if(output > this.max)
	{
		output = this.max;
	}
	if(output < this.min)
	{
		output = this.min;
	}
	
	return output;
}

module.exports = PIDController;