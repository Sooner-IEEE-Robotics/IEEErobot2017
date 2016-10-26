var bone = require('bonescript');

//Instance variables for LED
var pin = "";
var _state = 0;

//Setup the LED
var LED = function(pin)
{
	this.pin = pin;
	bone.pinMode(this.pin, bone.OUTPUT);
}

//Set the pin name of the LED
LED.prototype.setPin = function(pin)
{
	this.pin = pin;
}
//Get current LED pin ID
LED.prototype.getPin = function(pin)
{
	return this.pin;
}

//Set the LED brightness
LED.prototype.setBrightness = function(state)
{
	this._state = state;
	bone.digitalWrite(this.pin, state);
}
//Toggle the LED brightness
LED.prototype.toggleBrightness = function()
{
	this._state = !this._state;
	bone.digitalWrite(this.pin, this._state);
}
//Get LED Brightness
LED.prototype.getBrightness = function(state)
{
	return this._state;
}


//Exports the entire class to be used by other modules
module.exports = LED;