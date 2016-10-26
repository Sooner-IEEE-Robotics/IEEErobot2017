var bone = require('bonescript');

//Instance variables for Button
var pin = "";
var _state = false;

//Setup the Button
var Button = function(pin)
{
	//Set the pin
	this.pin = pin;
	bone.pinMode(this.pin, bone.INPUT);
}

Button.prototype.isPressed = function()
{
	//Read the digital signal
	bone.digitalRead(this.pin, function(x)
	{
		if(x.value == 1) //pressed
		{
			this._state = true;
		}
		else //not pressed
		{
			this._state = false;
		}
	});
	
	return this._state;
}

//Exports the entire Button module for global usage
module.exports = Button;