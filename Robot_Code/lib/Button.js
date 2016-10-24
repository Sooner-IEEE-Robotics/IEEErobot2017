var bone = require('bonescript');

//Instance variables for Button
var pin = "";
var _state = 0;

//Setup the LED
var Button = new function(pin)
{
	this.pin = pin;
	bone.pinMode(pin, bone.INPUT);
}

module.exports = Button;