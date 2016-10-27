var bone = require('bonescript');

//2D array that contains the board's status

//KEY
//-1: Starting Square (Yellow)
//0: Unexplored Square (Blank)
//1: Main Tunnel (Red)
//2: Dead-end Tunnel (Blue)
//Debug Mode Only
//3: Explored Empty Square (Purple)
//4: Obstacle over tunnel (Pink)
//5: Obstacle not over tunnel (Green)
//6: Future Path: (White)
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
	//Setup the LED Matrix
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

//Sets a board square based on 2 dimensional array coordinates
//Note: this does not turn on the light on the matrix, the show() function must be called for the lights to update.
LEDMatrix.prototype.setSquare = function(row,col,id)
{
	if(!(row==6 && col==6)) //Do not update the starting square ever
	{
		board[row][col] = id;
	}
}

//Sets a board square based on the name of the space (Ex: A1)
//Note: this does not turn on the light on the matrix, the show() function must be called for the lights to update.
LEDMatrix.prototype.setSquare = function(name,id)
{
	var colKey = "ABCDEFG";
	var rowKey = "1234567";
	var row = rowKey.indexOf(name.substr(1));
	var col = colKey.indexOf(name.substr(0,1));
	
	if(!(x==6 && y==6)) //Do not update the starting square ever
	{
		board[row][col] = id;
	}
}

LEDMatrix.prototype.show = function()
{
	//Output the board status to the LED Matrix
}



module.exports = LEDMatrix;
