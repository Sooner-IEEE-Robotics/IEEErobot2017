//This file serves as a registry of all of the devices attached to our beaglebone
//This file also initializes all of the devices we have

//Get the SCR IEEE libraries for ease of use
var lib = require('./lib/index.js');

/*
 * Register all devices and initialize
 */
var pinIndicatorLED = "P8_13"; //Pin for the Indicator LED
var indicatorLED = new lib.LED(pinIndicatorLED);

var pinStopButton = "P8_19"; //Pin for the Stop Button
var stopButton = new lib.Button(pinStopButton);

//Package all devices into this exports array for public access
module.exports = {
	indicatorLED: indicatorLED,
	
}