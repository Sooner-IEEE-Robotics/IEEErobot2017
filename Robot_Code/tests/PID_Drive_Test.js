var bone = require('bonescript');

//Encoder Interrupts
var gpio = require('onoff');

//Import Encoders
var encoder = require('../sensors/Encoder.js');

//Import the logger for debugging
var log = require('../logger/Log.js');
var logger = new log();

//Import all the device libraries for ease of use.
var lib = require('../lib/index.js');

var leftPinA = "xxxx", leftPinB = "yyyy";
var leftEncoder = new encoder(leftPinA, leftPinB, 1);

var rightPinA = "xxxx", rightPinB = "yyyy";
var rightEncoder = new encoder(rightPinA, rightPinB, 1);

//Setup Encoders to use interrupts
//Replace with actual GPIO pin values
var pinLeftA = new gpio(1, 'in', 'both');
var pinRightA = new gpio(3, 'in', 'both');

var leftMotor = new lib.PWM("zzzz");
var rightMotor = new lib.PWM("zzzz");

//Start watching the left encoder for signal changes
pinLeftA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	leftEncoder.update();
	
});

//Start watching the right encoder for signal changes
pinRightA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	rightEncoder.update();
	
});

var TARGET = 24; //Go 24 inches

var pidDriveControl = lib.PIDController(24, -0.5, 0.5);

pidDriveControl.setP(1);
pidDriveControl.setI(0);
pidDriveControl.setD(0);

pidDriveControl.init(getDistance());

while(abs(TARGET-getDistance())>0.01)//While the distance travelled is not within the target error tolerance.
{
	var output = pidDriveControl.getOutput(getDistance());
	
	leftMotor.setOutput(output);
	rightMotor.setOutput(output);
}

function getDistance()
{
	var d = (rightEncoder.getDistance() + leftEncoder.getDistance()) / 2;
	return d;
}