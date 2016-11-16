var bone = require('bonescript');

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

var left = 0;
var right = 0;
var distance= 0;

//Make sure encoder reading works
readEncoders();


function readEncoders()
{
	left = leftEncoder.read();
	right = rightEncoder.read();
	
	distance = (left + right)/2;
	
	console.log(distance);
}