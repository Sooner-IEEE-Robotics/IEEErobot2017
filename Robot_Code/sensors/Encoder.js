var bone = require("bonescript");
var bone = require("bonescript");

var reference;
var Kt; //Tick Constant from Ticks to inches
var pinA, pinB;

var position;

var Encoder = function(pinA, pinB, Kt)
{
	this.pinA = pinA;
	this.pinB = pinB;
	
	this.Kt = Kt;
	
	this.position = 0;
	
	bone.pinMode(this.pinA, bone.INPUT);
	bone.pinMode(this.pinB, bone.INPUT);
}

Encoder.prototype.update = function()
{
	var a = 0;
	var b = 0;
	
	bone.digitalRead(this.pinA, function(x)
	{
		a = x.value;
	});
	bone.digitalRead(this.pinB, function(x)
	{
		b = x.value;
	});
	
	if(a == b)
	{
		this.position++;
	}
	else
	{
		this.position--;
	}
}

//reset the encoder
Encoder.prototype.reset = function()
{
	this.position = 0;
}

//Get ticks counted by encoder
Encoder.prototype.getTicks = function()
{
	return this.position;
}

//Get distance travelled in inches
Encoder.prototype.getDistance = function()
{
	return (this.Kt * this.position);
}

Encoder.prototype.setKt = function(Kt)
{
	this.Kt = Kt;
}

module.exports = Encoder;