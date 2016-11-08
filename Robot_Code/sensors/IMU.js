var lib = require("../lib/index.js");

var i2cDevice;

var IMU = function(deviceName, address)
{
	this.i2cDevice = new lib.I2C(deviceName, address);
}

IMU.prototype.getYaw = function()
{
	
}

IMU.prototype.getPitch = function()
{
	
}

IMU.prototype.getRoll = function()
{
	
}

IMU.prototype.getXAccel = function()
{
	
}

IMU.prototype.getYAccel = function()
{
	
}

IMU.prototype.getZAccel = function()
{
	
}