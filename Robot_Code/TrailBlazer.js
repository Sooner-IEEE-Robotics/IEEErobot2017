"use strict";

var global = require("./Globals.js");
var s = require("./Square.js");

var globals = 
{
	DIRECTION_NORTH:0, DIRECTION_EAST:1, DIRECTION_SOUTH:2, DIRECTION_WEST:3, DIRECTION_NONE:10, ERROR_NOSUCHSQUARE:11
};

//The program allows for more and more redundant steps as it fails to find optimal paths. Decide when you want it to just give up:
var allowedRedundantSteps = 3;

//With a lot of open squares on the board, looking for a path with only a few redundant squares can take an absurd amount of time. 
//After searching for this many milliseconds, the program will decide to speed up the calculation but accept another redundant square on the path. 
var initialMillisecondsBeforeIncreasingRedundantSteps = 5000;

//Every time the program increases the number of redundant steps it will aim for, it increases the milliseconds it is willing to wait by this much. 
//If this is zero, it waits the same amount no matter how many redundant steps it is allowing.
var additionalMillisecondsBeforeIncreasingRedundantSteps = 7500;

//Reference for timer
var startTime;
var squares = [];

//Initialize variables that will be used in backtracking loop.
var currentSquare;
var actionOfCurrentSquare;
var nextSquareToBranchTo;
var closedSquaresOnPathSoFar = 0;
var triedAllPossiblePaths;	
var searchedForTooLong;	
var millisecondsBeforeIncreasingAllowedClosedSquares;

//Constructor to make Node easy
var TrailBlazer = function()
{
	//this.nextSquareToBranchTo = 
	this.closedSquaresOnPathSoFar = 0;
};

TrailBlazer.prototype.beginCounting = function()
{
	this.startTime = Date.now();
}

TrailBlazer.prototype.initializeBoard = function()
{
	this.squares = [];
	for(var i = 0; i < 7; i++) 
	{
		this.squares.push(new Array(7));
	}
}

TrailBlazer.prototype.getElapsedTime = function()
{
	return Date.now() - this.startTime;
}

//Get the information of a board square
TrailBlazer.prototype.getSquareInfo = function(row, column)
{
	return global.BOARD[row][column];
}

//Returns the Square object at given coordinates.
//Returns globals.ERROR_NOSUCHSQUARE if relative square is out of bounds.
TrailBlazer.prototype.getMimickedSquare = function(row, column)
{
	if(row > this.squares.length - 1 || row < 0 || column > this.squares[0].length - 1 || column < 0) //The coordinates do not refer to a square on the field.
	{
		return globals.ERROR_NOSUCHSQUARE;
	}
	
	return this.squares[row][column];
}

//Returns the Square object in the specified direction from the given coordinates. 
//Returns globals.ERROR_NOSUCHSQUARE if relative square is out of bounds.
TrailBlazer.prototype.getSquareRelatively = function(row, column, direction)
{
	if(direction == globals.DIRECTION_NORTH)
	{
		return this.getMimickedSquare(row - 1, column);
	} 
	else if(direction == globals.DIRECTION_EAST)
	{
		return this.getMimickedSquare(row, column + 1);
	}
	else if(direction == globals.DIRECTION_SOUTH)
	{
		return this.getMimickedSquare(row + 1, column);
	}
	else if(direction == globals.DIRECTION_WEST)
	{
		return this.getMimickedSquare(row, column - 1);
	}
}

//Returns true if there are open squares left. Otherwise returns false.
TrailBlazer.prototype.openSquaresRemain = function()
{
	for(let row of this.squares) //For each row in the field...
	{
		for(let square of row) //For each square in the row...
		{
			if(square.getStatus() == true) //The square is open.
			{
				return true;
			}
		}
	}

	return false;
}

TrailBlazer.prototype.resetSquares = function()
{
	for(let row of this.squares) //For each row in the field...
	{
		for(let square of row) //For each square in the row...
		{
			square.setStatus(square.originalStatus);
			square.directionsToConsiderStack = [];
			square.squaresBranchedFromStack = [];
		}
	}
}

TrailBlazer.prototype.calculateForayPath = function(board, currentRow, currentColumn)
{
	this.initializeBoard();

	//Get our hands on the occupied square so we can treat it as closed in our mimickedField.
	var occupiedSquareCoordinates = [currentRow, currentColumn];

	//Fill our mimickedField with SquaresForBacktracking having the same statuses as the squares in the actual field.  
	for(var i = 0; i < 7; i++)
	{
		for(var j = 0; j < 7; j++)
		{
			board[i][j].setupSquare();
			this.squares[i][j] = board[i][j];
			//this.squares[i][j].setupSquare();
		}
	}

	//================:::::BACKTRACKING PROCESS BEGINS:::::==========================

	//The first square to consider on our search for a path will be the occupied square because that's where the robot starts.
	var initialSquareCoordinates = occupiedSquareCoordinates;//.slice(0);			
	
	//Do backtracking to find a path that will let the robot close all the open squares while visiting the fewest possible closed squares (path w/ fewest redundant steps).
	for(var allowedClosedSquares = 0; allowedClosedSquares <= allowedRedundantSteps; allowedClosedSquares++)
	{
		//Output for debugging. Will be removed eventually.
		console.log("Now looking for a path with " + allowedClosedSquares + " redundant steps.");

		//We're just starting with this number of closed squares, so we haven't tried all the paths or searched too long yet.
		this.triedAllPossiblePaths = false;
		this.searchedForTooLong = false;

		//For each number of allowed closed squares, we start our search at the occupied square.
		this.currentSquare = this.getMimickedSquare(initialSquareCoordinates[0], initialSquareCoordinates[1]);	
		
		//Since nothing will branch to the initial square, we need to tell it to consider all of the directions.
		this.currentSquare.addAllDirectionsToStack();

		//We will try for only so long to find a path with each number of redundant steps before compromising on the redundant steps for the sake of time.
		//General, we will be willing to try longer if we are on a higher number of allowed closed squares.
		this.millisecondsBeforeIncreasingAllowedClosedSquares = initialMillisecondsBeforeIncreasingRedundantSteps + 
			allowedClosedSquares * additionalMillisecondsBeforeIncreasingRedundantSteps;

		//We start the timer now.
		this.beginCounting();

		//While there are still paths left to search with sufficiently few closed squares and we have not been searching too long...
		while((!this.triedAllPossiblePaths) && (!this.searchedForTooLong)) 
		{
			if((this.actionOfCurrentSquare = this.currentSquare.getNextAction()) == globals.DIRECTION_NORTH 
				|| this.actionOfCurrentSquare == globals.DIRECTION_EAST || this.actionOfCurrentSquare == globals.DIRECTION_SOUTH
				|| this.actionOfCurrentSquare == globals.DIRECTION_WEST) //The current square has more directions to consider.
			{
				this.nextSquareToBranchTo = this.getSquareRelatively(this.currentSquare.getCoordinates()[0], this.currentSquare.getCoordinates()[1], this.actionOfCurrentSquare);
				
				//console.log(this.nextSquareToBranchTo != globals.ERROR_NOSUCHSQUARE);
				
				if(this.nextSquareToBranchTo != globals.ERROR_NOSUCHSQUARE) //There is a square to branch to in the direction being considered by the current square.
				{
					
					if(this.nextSquareToBranchTo.getStatus() == true) //The square under consideration for branching is open.
					{
						this.currentSquare.branchTo(this.nextSquareToBranchTo);
						this.nextSquareToBranchTo.setStatus(false);
						this.currentSquare = this.nextSquareToBranchTo;

						if(this.openSquaresRemain() == false) //The previously open square was the last one on the field. We found a path that eliminates all the open squares!
						{
							//Work backwards from the final square to extract and return the path.
							var calculatedForayPath = [this.currentSquare.getCoordinates()];								
							while(typeof (this.currentSquare = this.currentSquare.squaresBranchedFromStack.pop()) != "undefined") //While there is an earlier square in the path... 
							{
								//Add that earlier square's coordinates to the beginning of the calculatedForayPath array.
								calculatedForayPath.unshift(this.currentSquare.getCoordinates());
							}
							//Remove the starting square from the path.
							calculatedForayPath.shift();

							return calculatedForayPath; //gg no re
						} 							
					}
					else if(this.nextSquareToBranchTo.getStatus() == false) //The square under consideration for branching is closed.
					{
						//console.log(this.closedSquaresOnPathSoFar);
							
						if(this.closedSquaresOnPathSoFar < allowedClosedSquares && this.nextSquareToBranchTo.isObstacle() == false) //We are still accepting closed squares. We can branch in this direction. This Square also should not be an obstacle.
						{
							this.currentSquare.branchTo(this.nextSquareToBranchTo);
							this.currentSquare = this.nextSquareToBranchTo;
							this.closedSquaresOnPathSoFar++;								 
						}							
					}						
				}					
			}
			else //The current square has no more directions to consider.
			{
				if(typeof this.actionOfCurrentSquare != "undefined") //The current square was branched to by another square. Let's backtrack to that square.
				{
					if(this.currentSquare.getStatus() == false) //The square we are backtracking from is closed even without our being there.
					{
						//By backtracking from this square, we take a redundant step off our path.
						this.closedSquaresOnPathSoFar--;
					}

					//The square that most recently branched to the current square becomes the new current square. 
					this.currentSquare = this.actionOfCurrentSquare;
				}
				else //The current square was the initial square. We're out of options for this number of allowed closed squares.
				{
					this.triedAllPossiblePaths = true;
				}
			}
			
			if(this.getElapsedTime() > this.millisecondsBeforeIncreasingAllowedClosedSquares) //We have searched for too long with this number of closed squares.
			{
				this.searchedForTooLong = true;

				//Force the squares back to their starting states and return closedSquaresOnPath to 0.
				//These things would have happened naturally in the process of searching all the paths, but we stopped it midway.
				this.resetSquares();
				this.closedSquaresOnPathSoFar = 0;
			}
					
		}

		//Output for debugging. Will be removed eventually.
		if(!this.triedAllPossiblePaths) 
		{
			console.log(this.getElapsedTime() + " milliseconds passed. Gave up on finding a path with "
				+ "only " + allowedClosedSquares + " redundant steps."); 
			
		}
		else
		{
			console.log("No paths exist with only " + allowedClosedSquares + " redundant steps.");
		}				
	}

	//Failed to find a path with as few closed squares as required.
	return undefined;	
}

module.exports = TrailBlazer;