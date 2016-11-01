var bone = require('bonescript');

//2D array that contains the board's status

//KEY
//-1: Starting Square (Yellow)
//0: Unexplored Square or Empty Square or Obstacle over empty square(Blank)
//1: Main Tunnel (Red)
//2: Dead-end Tunnel (Blue)
//Debug Mode Only
//3: Explored Empty Square (Purple)
//4: Obstacle over tunnel (Pink)
//5: Obstacle not over tunnel (Green)
//6: Future Path (White)
var board = 
[
[0,0,0,0,0,0,0]
[0,0,0,0,0,0,0]
[0,0,0,0,0,0,0]
[0,0,0,0,0,0,0]
[0,0,0,0,0,0,0]
[0,0,0,0,0,0,0]
[-1,0,0,0,0,0,0]
];

var isDebug = false;

var LEDMatrix = function(pin)
{
	//Setup the LED Matrix as an I2C device
	
}

var LEDMatrix = function(pin, debug)
{
	//Setup the LED Matrix
	
	//Set Debug flag
	this.isDebug = debug;
}

LEDMatrix.prototype.setDebug = function(debug)
{
	this.isDebug = debug;
}

//Returns the current status of the board
LEDMatrix.prototype.getBoard = function()
{
	return this.board;
}

LEDMatrix.prototype.show = function()
{
	//Output the board status to the LED Matrix
}

LEDMatrix.prototype.showDebug = function()
{
	//Output the debug board status to the LED Matrix 
}

module.exports = LEDMatrix;
