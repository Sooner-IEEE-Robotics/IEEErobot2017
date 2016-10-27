var fs = require('fs');

var _name = "log.txt"

var Log = function()
{
	this._name = createDateStringName();
	var time = this.getTime();
	fs.appendFile("./logs/"+this._name, time +": Log Opened!\n", errorHandler);
};

//Writes a message to a log file.
//Flag can be whatever you want, but it should make it easy to identify the event being logged, or the error itself
Log.prototype.write = function(flag, message)
{
	var time = this.getTime();
	fs.appendFile("./logs/"+this._name, "[" + flag + "] at " + time + ": " + message +"\n", errorHandler);
}

Log.prototype.getTime = function()
{
	var date = new Date();
	
	var hour = date.getHours();
	var minutes = date.getMinutes();
	var seconds = date.getSeconds();
	
	if(hour < 10)
	{
		hour = "0" + hour;
	}
	if(minutes < 10)
	{
		minutes = "0" + minutes;
	}
	if(seconds < 10)
	{
		seconds = "0" + seconds;
	}
	
	var time = hour + ":" + minutes + "." + seconds;
	return time;
}

module.exports = Log;

//Handle any file errors
function errorHandler()
{
	
}

//Create the log file's name using a timestamp
function createDateStringName()
{
	var date = new Date();

	var month = date.getMonth() + 1;
	var day = date.getDate();
	
	var hour = date.getHours();
	var minutes = date.getMinutes();
	var seconds = date.getSeconds();
	
	if(month < 10)
	{
		month = "0" + month;
	}
	if(day < 10)
	{
		day = "0" + day;
	}
	
	if(hour < 10)
	{
		hour = "0" + hour;
	}
	if(minutes < 10)
	{
		minutes = "0" + minutes;
	}
	if(seconds < 10)
	{
		seconds = "0" + seconds;
	}
	
	var str = date.getFullYear() + "-" + day + "-" + month + "_" + hour + "_" + minutes + "_" + seconds + ".txt";
	
	return str;
}