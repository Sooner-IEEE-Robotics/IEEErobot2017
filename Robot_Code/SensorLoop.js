var bone = require('bonescript');

var imu = require('./sensors/IMU.js');
var encoder = require('./sensors/Encoder.js');

var d = require('./Devices.js');

var running;

var SensorLoop = function()
{
	this.running = true;
};

//Update the sensors asynchronous of the main loop
SensorLoop.prototype.loop = function()
{
	while(this.running) //Never stop updating sensors while the program is running
	{
		d.imu.update();
		d.rightEncoder.read();
		d.leftEncoder.read();
	}
}

SensorLoop.prototype.stop = function()
{
	this.running = false;
}