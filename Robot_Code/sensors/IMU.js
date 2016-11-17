var lib = require("../lib/index.js");

var i2cDevice;
var yaw;

var IMU = function(deviceName, address)
{
	this.i2cDevice = new lib.I2C(deviceName, address);
	
	this.yaw = 0;
}

//Z axis rotation. Uses complimentary filter to find angle.
IMU.prototype.getYaw = function()
{
	return this.yaw;
}

//Read the sensor values
IMU.prototype.update = function()
{
	var data = this.i2cDevice.read();
	
	return data; //Return the raw data for now because who knows what's going on
}
/*
IMU.prototype.getPitch = function()
{
	
}

IMU.prototype.getRoll = function()
{
	
} */

IMU.prototype.getXAccel = function()
{
	
}

IMU.prototype.getYAccel = function()
{
	
}

IMU.prototype.getZAccel = function()
{
	
}