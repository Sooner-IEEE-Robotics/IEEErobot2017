var bone = require("bonescript");
var bone = require("bonescript");

var reference;
var Kt; //Tick Constant from Ticks to inches
var pinA, pinB;

var position;

var lastValue = 0;

var Encoder = function(pinA, pinB, Kt)
{
	this.pinA = pinA;
	this.pinB = pinB;
	
	this.Kt = Kt;
	
	this.position = 0;
	
	bone.pinMode(this.pinA, bone.INPUT);
	bone.pinMode(this.pinB, bone.INPUT);
	
	bone.attachInterrupt(this.pinA, true, bone.CHANGE, this.interruptCallback);
}

Encoder.prototype.interruptCallback = function(x)
{
	var a = x.value;
	
	var b;
	
	bone.digitalRead(this.pinB,function(x)
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

Encoder.prototype.update = function()
{
	/*
	var a = 0;
	var b = 0;
	
	bone.digitalRead(this.pinA, function(x)
	{
		a = x.value;
	});
	bone.digitalRead(this.pinB, function(x)
	{
		b = x.value;
	}); */
	
	
	
	console.log(a + " ?=? " + b);
	
	if(a == b)
	{
		this.position++;
	}
	else
	{
		this.position--;
	}
}

Encoder.prototype.read = function()
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
	
	
	console.log(a + " ?=? " + b);
	
	if(this.lastValue == 0 && a == 1)
	{
		if(b == 0)
		{
			this.position--;
		}
		else
		{
			this.position++;
		}
	}
	
	this.lastValue = a;
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