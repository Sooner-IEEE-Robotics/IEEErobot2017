var lib = require("../lib/index.js");

var pwmLeft, pwmRight;

var Drivetrain = function(pwmPinLeft, pwmPinRight)
{
	this.pwmLeft = new lib.PWM(pwmPinLeft);
	this.pwmRight = new lib.PWM(pwmPinRight);
};

Drivetrain.prototype.setAngle = function(angle)
{
	
	
	return true; //turn complete
}

Drivetrain.prototype.distanceDrive = function(distance)
{

	return true; //drive complete
}