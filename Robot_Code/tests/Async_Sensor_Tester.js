var bone = require('bonescript');

var async = require('async');

//Import Encoders
var encoder = require('../sensors/Encoder.js');
var imu = require('../sensors/IMU.js');

//Import the logger for debugging
var log = require('../logger/Log.js');
var logger = new log();

//Import all the device libraries for ease of use.
var lib = require('../lib/index.js');

//Left Encoder
var leftPinA = "P8_26", leftPinB = "P8_25";
var leftEncoder = new encoder(leftPinA, leftPinB, 1);

//Right Encoder
var rightPinA = "P8_28", rightPinB = "P8_27";
var rightEncoder = new encoder(rightPinA, rightPinB, 1);

//Gyro
//var imu_gyro = new imu("","");

var distance = 0;

async.parallel([
    readEncoders(callback),
    mainLoop(callback)
], 	function(err, results) {
    // optional callback
});

function mainLoop(callback)
{
	console.log(distance);
}

function readEncoders(callback)
{
	while(true)
	{
		left = leftEncoder.read();
		right = rightEncoder.read();
		
		distance = (left + right)/2;
	}
}