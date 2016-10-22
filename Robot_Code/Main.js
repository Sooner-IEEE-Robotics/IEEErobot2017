//var b = require('bonescript')

var Devices = require('Devices.js');
var Globals = require('Globals.js');
var Setup = require('Setup.js');

var d = new Devices();
var setup = new Setup();
var global = new Globals();

//Turn on indicator LED
b.digitalWrite(d.IndicatorLED, 1);

//Stop Button
var stop = false;
var codeComplete = false;

//Autonomous Loop. Run the code until the button is pressed, or until we finish the code.
while(stop == false && codeComplete == false)
{
	
}