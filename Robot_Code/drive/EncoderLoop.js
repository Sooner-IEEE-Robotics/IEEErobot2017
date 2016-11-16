
var pin1A, pin1B;
var pin2A, pin2B;

var encoder1Pos, encoder2Pos;

var EncoderLoop = function(enc1A, enc1B, enc2A, enc2B)
{
	this.pin1A = enc1A;
	this.pin1B = enc1B;
	this.pin2A = enc2A;
	this.pin2B = enc2B;
	
	this.encoder1Pos = 0;
	this.encoder2Pos = 0;
};

EncoderLoop.prototype.readEncoders = function()
{
	
}

EncoderLoop.prototype.resetEncoders = function()
{
	this.encoder1Pos = 0;
	this.encoder2Pos = 0;
}