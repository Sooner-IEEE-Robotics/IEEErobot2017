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

var leftPinA = "P8_26", leftPinB = "P8_25";
var leftEncoder = new encoder(leftPinA, leftPinB, 1);

var rightPinA = "P8_28", rightPinB = "P8_27";
var rightEncoder = new encoder(rightPinA, rightPinB, 1);

//Setup Encoders to use interrupts
//Replace with actual GPIO pin values
var pinLeftA = new gpio(61, 'in', 'both');
var pinRightA = new gpio(88, 'in', 'both');

var leftMotor = new lib.PWM("P9_29");
var rightMotor = new lib.PWM("P9_30");

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

var TARGET = 2400; //Go 2400 ticks

var pidDriveControl = lib.PIDController(TARGET, -0.5, 0.5);

pidDriveControl.setP(1);
pidDriveControl.setI(0);
pidDriveControl.setD(0);

pidDriveControl.init(getDistance());

while(abs(TARGET-getDistance())>0.01)//While the distance travelled is not within the target error tolerance.
{
	var drive = pidDriveControl.getOutput(getDistance());
	var turn = 0;
	
	setRealOutput(drive, turn);
}

function getDistance()
{
	var d = (rightEncoder.getDistance() + leftEncoder.getDistance()) / 2;
	return d;
}

function setRealOutput(drivePower, turnRate) //Arcade Drive
{
	var left = drivePower;
	var	right = drivePower;
	
	if(turnRate > 0) //Turn right
	{
		var val = Math.log(turnRate);
		var ratio = (val - 0.5) / (val + 0.5);
		
		if(ratio == 0)
		{
			ratio = 0.00000001;
		}
		
		right = drivePower / ratio;
	}
	else if(turnRate < 0) //Turn left
	{
		var val = Math.log(-turnRate);
		var ratio = (val - 0.5) / (val + 0.5);
		
		if(ratio == 0)
		{
			ratio = 0.00000001;
		}
		
		left = drivePower / ratio;
	}
	
	leftMotor.setOutput(left);
	rightMotor.setOutput(right);
}