var bone = require('bonescript');

//Import all the device libraries for ease of use.
var lib = require('./lib/index.js');

//Import our devices
var d = require('./Devices.js');

//Global Variables
//Code Completeion flag
var codeComplete = false;

console.log("Robot Code Running!");

d.indicatorLED.setBrightness(1);

/*
//Autonomous Loop. Run the code until the button is pressed, or until we finish the code.
while(codeComplete == false)
{
	
}*/

//close the program
process.exit();