var bone = require('bonescript');

var async = require('async');

var gpio = require('onoff');

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

//Setup interrupts
//Start watching the left encoder for signal changes
leftPinA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	leftEncoder.update();
	
});

//Start watching the right encoder for signal changes
rightPinA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	rightEncoder.update();
	
});

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
		left = leftEncoder.getTicks();
		right = rightEncoder.getTicks();
		
		distance = (left + right)/2;
	}
}