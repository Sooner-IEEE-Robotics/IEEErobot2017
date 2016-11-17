var bone = require('bonescript');

//Import Encoders
var encoder = require('../sensors/Encoder.js');
var imu = require('../sensors/IMU.js');

//Import the logger for debugging
var log = require('../logger/Log.js');
var logger = new log();

//Import all the device libraries for ease of use.
var lib = require('../lib/index.js');

//Left Encoder
var leftPinA = "P8_26", leftPinB = "P8_29";//For some reason P8_25 is not in the pinmux
var leftEncoder = new encoder(leftPinA, leftPinB, 1);

//Right Encoder
var rightPinA = "P8_28", rightPinB = "P8_27";
var rightEncoder = new encoder(rightPinA, rightPinB, 1);

//Gyro
//var imu_gyro = new imu("","");

var left = 0;
var right = 0;
var distance= 0;

var yaw = 0;

//Make sure encoder reading works
while(true)
{
	//readGyro();
	readEncoders();
}



function readEncoders()
{
	left = leftEncoder.read();
	right = rightEncoder.read();
	
	distance = (left + right)/2;
	
	console.log(distance);
}

function readGyro()
{
	imu_gyro.update();
	yaw = imu_gyro.getYaw();
	
	console.log(yaw);
}