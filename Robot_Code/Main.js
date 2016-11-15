var bone = require('bonescript');

//Run other loops off to the side asynchronously
var async = require('async');

//Encoder Interrupts
var gpio = require('onoff');

//Import the logger for debugging
var logger = require('./logger/Log.js');

//Import all the device libraries for ease of use.
var lib = require('./lib/index.js');

//Import our devices and global variables
var device = require('./Devices.js');
var global = require('./Globals.js');
var s = require("./Square.js");

//Import the Sensor Loop
var sensorLoop = require('./SensorLoop.js');
var sensors = new sensorLoop();

//Import Trailblazer for navigation
var TB = require('./TrailBlazer.js');
var trailblazer = new TrailBlazer();

//Setup the board
createCacheBoard();
createBoard();

//Code Completeion flag
var codeComplete = false;

//console.log("Robot Code Running!");
logger.write("STARTUP","Robot Code Running!");

//We want to start driving around the perimeter
global.FuturePath = trailblazer.calculateForayPath(global.CACHE_BOARD, 6, 0);


//Asynchronous execution of robot actions and sensor reading
async.parallel([
    function(callback)
	{ ... },
    function(callback)
	{ ... }
], 	function(err, results) {
    // optional callback
});

var shouldStopRobot = findCaches();

if(shouldStopRobot)
{
	stopRobot();
}

//Drive around until 2 caches have been found
function findCaches()
{
	while(cachesFound < 2)
	{
		
	}
	
	global.FuturePath = [];
	global.FuturePath = trailblazer.calculateForayPath(global.BOARD, global.currentRow, global.currentColumn);
	
	return autonomousLoop();
}

//Drive around until we have visited all possible squares and mapped the tunnels
function autonomousLoop()
{
	while(codeComplete != true)
	{
		
	}
	
	return stopRobot();
}

//Create the actual game board.
function createBoard()
{
	for(var row = 0; row<7; ++row)
	{
		var r = [];
		
		for(var col = 0; col<7; ++col)
		{
			r.push(new s(row,col));		
		}
		
		global.BOARD.push(r);
	}
	
	logger.write("BOARD","Board Created Successfully!");
}

//create a board that will be used to drive around the outside of the field at the start.
function createCacheBoard()
{
	for(var row = 0; row<7; ++row)
	{
		var r = [];
		
		for(var col = 0; col<7; ++col)
		{
			var sq = new s(row,col);
			sq.setExplored(global.cacheBoard[row][col]); //sets the square as open or closed for our path around the wall
			r.push(sq);			
		}
		
		global.CACHE_BOARD.push(r);
	}
	
	logger.write("CACHE_BOARD","Board Created Successfully!");
}

//function that stops the robot from moving
function stopRobot()
{
	sensors.stop(); //End the sensor loop
}


//Make sure to close the encoder watching processes
process.on('SIGINT', function () 
{
	pinLeftA.unexport();
	pinRightA.unexport();
});









/*
//Setup Encoders to use interrupts
//Replace with actual GPIO pin values
var pinLeftA = new gpio(1, 'in', 'both');
var pinRightA = new gpio(3, 'in', 'both');

//Start watching the left encoder for signal changes
pinLeftA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	device.leftEncoder.update();
	
});

//Start watching the right encoder for signal changes
pinRightA.watch(function(err, value)
{
	if(err)
	{
		throw err;
	}
	
	device.rightEncoder.update();
	
});
*/





