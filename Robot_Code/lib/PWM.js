//NOTE: the only pins on BBG that are PWM enabled are as follows:
//P8: 13, 19, 34, 36, 45, 46
//P9: 14, 16, 21, 22, 28, 29, 31, 42
//Only 8 ports can be used at a time before the CPU cycles slow down.

var bone = require('bonescript');

//Instance variables for PWM
var pin = "";
var _output = 0;
var frequency = 2000; //Default value is 2000 Hz

//Setup the PWM pin
var PWM = function(pin)
{
	//Set the PWM pin
	this.pin = pin;
	bone.pinMode(this.pin, bone.OUTPUT);
}

PWM.prototype.setOutput = function(output)
{
	this._output = output;
	bone.analogWrite(this.pin,this._output,this.frequency);
}

PWM.prototype.setFrequency = function(f)
{
	this.frequency = f;
}

//Exports the PWM module for global usage
module.exports = PWM;