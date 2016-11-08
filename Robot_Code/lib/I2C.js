var bone = require('bonescript');

//Instance variables for I2C
var address = "";//Sensor file location
var iic = '/sys/class/i2c-adapter/i2c-1/';

//Setup the I2C bus
var I2C = function(deviceName, address)
{
	bone.writeTextFile(iic + 'new_device', deviceName);//I2C device location
	this.address = address;
};

I2C.prototype.read = function(output)
{
	var data = "";
	bone.readTextFile(this.address, function(x)
	{
		data = x.data;
	});
	
	return data;
}

I2C.prototype.write = function(output)
{
	bone.writeTextFile(this.address, output)
}

//Exports the I2C module for global usage
module.exports = I2C;