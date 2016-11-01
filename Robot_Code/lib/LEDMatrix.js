var bone = require('bonescript');

//2D array that contains the board's status

//KEY
//Starting Square (Yellow)
//Unexplored Square or Empty Square or Obstacle over empty square(Blank)
//Main Tunnel (Red)
//Dead-end Tunnel (Blue)
//Debug Mode Only
//Explored Empty Square (Purple)
//Obstacle over tunnel (Pink)
//Obstacle not over tunnel (Green)
//Future Path (White)

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
