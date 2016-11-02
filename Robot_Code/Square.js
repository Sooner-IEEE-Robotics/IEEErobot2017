//Directions Enum
var globals = 
{
	DIRECTION_NORTH:0, DIRECTION_EAST:1, DIRECTION_SOUTH:2, DIRECTION_WEST:3, DIRECTION_NONE:10, ERROR_NOSUCHSQUARE:11
};

//Square Status Booleans
var explored = false;
var mainTunnel = false;
var deadEnd = false;
var obstacle = false;
var start = false;
var empty = true;
var futurePath = true;

//Is this the current square?
var occupying = false;

//Square coordinates
var row = 0;
var col = 0;

//Backtracking Instance Variables
//This stack contains squares from which this square has branched.
var squaresBranchedFromStack = [];

//Arrays in this stack contain directions we will attempt to branch in the future. 
var directionsToConsiderStack = [];

//Changes depending upon whether a square is closed, open, forbidden, etc.
var status, originalStatus;


var Square = function(row, col)
{
	this.row = row;
	this.col = col;
	
	this.directionsToConsiderStack = [];
	this.squaresBranchedFromStack = [];
};

//Set values of the square below
Square.prototype.setExplored = function(x)
{
	this.explored = x;
}

Square.prototype.setMainTunnel = function(x)
{
	this.mainTunnel = x;
}

Square.prototype.setDeadEnd = function(x)
{
	this.deadEnd = x;
}

Square.prototype.setObstacle = function(x)
{
	this.obstacle = x;
}

Square.prototype.setStart = function(x)
{
	this.start = x;
}

Square.prototype.setEmpty = function(x)
{
	this.empty = x;
}

Square.prototype.setOccupied = function(x)
{
	this.occupying = x;
}
//Set values of the square above

//Get various statuses of the square below
Square.prototype.isExplored = function()
{
	return this.explored;
}

Square.prototype.isMainTunnel = function()
{
	return this.mainTunnel;
}

Square.prototype.isDeadEnd = function()
{
	return this.deadEnd;
}

Square.prototype.isObstacle = function()
{
	return this.obstacle;
}

Square.prototype.isStart = function()
{
	return this.start;
}

Square.prototype.isEmpty = function()
{
	return this.empty;
}

Square.prototype.isOccupied = function()
{
	return this.occupying;
}
//Get various statuses of the square above


//Backtracking stuff


//Needs to be run in order to preserve status states.
Square.prototype.setupSquare = function()
{
	this.status = !this.isExplored() && !this.isObstacle();
	
	//The square remembers its original status.
	this.originalStatus = !this.isExplored() && !this.isObstacle();
}

//Returns the square's status.
Square.prototype.getStatus = function()
{
	return this.status;
}

//Sets the square's status to given newStatus.
Square.prototype.setStatus = function(newStatus)
{
	this.status = newStatus;
}	

//Returns an array representing the square's row and column on the field.
Square.prototype.getCoordinates = function()
{
	return [this.row, this.col];
}	

//Put a [West, South, East, North] array on top of the directionsToConsider stack.
Square.prototype.addAllDirectionsToStack = function() 
{
	
	this.directionsToConsiderStack.push([globals.DIRECTION_WEST, globals.DIRECTION_SOUTH, globals.DIRECTION_EAST, globals.DIRECTION_NORTH]);
};

//Put a square on top of the squaresBranchedFrom stack.
Square.prototype.addSquareToStack = function(square)
{
	
	this.squaresBranchedFromStack.push(square);
}		

//If I haven't searched every direction since I've been branched to, return the next direction in line. 
//Failing that, if I have been branched from by a square, return the last square that branched to me.
//Return undefined if I'm out of directions and was not branched to by any square (AKA I'm out of directions and I'm the initial square)
//Update stacks and status accordingly.
Square.prototype.getNextAction = function()
{
	//Pop off the last direction in the last direction array in the directionsToConsider stack.
	var nextDirection = this.directionsToConsiderStack[this.directionsToConsiderStack.length - 1].pop();

	if(typeof nextDirection != "undefined") //I haven't searched every direction since I was last branched to.
	{
		return nextDirection;					
	}
	else //I've searched every direction since I was last branched to. 
	{
		//Pop the now depleted direction array from the top of the directionsToConsiderStack.
		this.directionsToConsiderStack.pop();

		//Pop the last square who branched to me from the squaresBranchedFromStack stack.
		var previousSquare = this.squaresBranchedFromStack.pop();

		//If I am no longer being branched to by any squares, revert to my original status. I haven't been visited on the planned path, so I shouldn't necessarily be closed.
		if(this.squaresBranchedFromStack.length == 0)
		{
			this.setStatus(this.originalStatus);

			if(typeof previousSquare != "undefined")
			{
				//console.log("Backtracking to " + previousSquare.getCoordinates());
			}						
		}

		return previousSquare; //Will be undefined if there was no previous square that branched to me.
	}
}		

//Hit up another square to continue the search.
Square.prototype.branchTo = function(square)
{
	square.addSquareToStack(this);
	square.addAllDirectionsToStack();
	//console.log("Branching to " + square.getCoordinates());
}
//End of Backtracking stuff

module.exports = Square;