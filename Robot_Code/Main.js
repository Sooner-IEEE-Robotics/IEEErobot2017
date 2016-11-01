var bone = require('bonescript');

//Import the logger for debugging
var logger = require('./logger/Log.js');

//Import all the device libraries for ease of use.
var lib = require('./lib/index.js');

//Import our devices and global variables
var d = require('./Devices.js');
var global = require('./Globals.js');
var s = require("./Square.js");

//Setup the board
createBoard();

//Code Completeion flag
var codeComplete = false;

//console.log("Robot Code Running!");
logger.write("STARTUP","Robot Code Running!");

//d.indicatorLED.setBrightness(1);

/*
//Autonomous Loop. Run the code until the button is pressed, or until we finish the code.
while(codeComplete == false)
{
	
}*/

function createBoard()
{
	for(var row = 0; row<7; ++row)
	{
		var r = [];
		
		for(var col = 0; col<7; ++col)
		{
			r.push(new s.Square(row,col));		
		}
		
		global.BOARD.push(r);
	}
	
	global.BOARD[6][0].setStart(true);
	
	logger.write("BOARD","Board Created Successfully!");
}