"use strict";

//BELOW IS FOR TESTING ===============================================================================

//Build your field: 1 represents unvisited squares, 2 represents visited squares, 3 represents obstacles, and 4 is the robot itself.
var fieldForTesting =
[
	[3, 3, 3, 3, 3, 3, 3],
	[3, 1, 1, 1, 1, 1, 3],
	[3, 1, 1, 1, 1, 1, 3],
	[3, 1, 4, 1, 1, 1, 3],
	[3, 1, 1, 1, 1, 1, 3],
	[3, 1, 1, 1, 1, 1, 3],
	[3, 3, 3, 3, 3, 3, 3]
]

//The program allows for more and more redundant steps as it fails to find optimal paths. Decide when you want it to just give up:
var allowedRedundantSteps = 5;

//That's all you have to do. 
//The program will output your path to the console as a series of coordinates (vertical then horizontal) according to the following diagram:
//		 0  1  2  3  4  5  6
//	0	[_, _, _, _, _, _, _]
//	1	[_, _, _, _, _, _, _]
//	2	[_, _, _, _, _, _, _]
//	3	[_, _, _, _, _, _, _]
//	1	[_, _, _, _, _, _, _]
//	2	[_, _, _, _, _, _, _]
//	6	[_, _, _, _, _, _, _]

//ABOVE IS FOR TESTING=========================================================================================================




var globals = 
{
	STATUS_OPEN:1, STATUS_CLOSED:2, STATUS_FORBIDDEN:3, STATUS_OCCUPIED:4, STATUS_OUTOFBOUNDS:5, 
	DIRECTION_NORTH:6, DIRECTION_EAST:7, DIRECTION_SOUTH:8, DIRECTION_WEST:9, DIRECTION_NONE:10, ERROR_NOSUCHSQUARE:11
};	

var game = 
{
	field: fieldForTesting,
	
	plannedPath: [],

	currentDirection: globals.DIRECTION_EAST,

	//Returns the status of a square on the field. Return globals.STATUS_OUTOFBOUNDS if given coordinates do not refer to a square on the field. 
	getSquareStatus: function(row, column) 
	{
		if(row > this.field.length || row < 0 || column > this.field[0].length || column < 0) //The given coordinates do not refer to a square on the field.
		{			
			return globals.STATUS_OUTOFBOUNDS;
		} 
		else //The given coordinates do refer to a square on the field.
		{
			return this.field[row][column];
		}		
	},

	//Sets the status of a square on the field. Return true if given row and column exist on board, otherwise return false. 
	setSquareStatus: function(row, column, status) 
	{
		if(row > this.field.length || row < 0 || column > this.field[0].length || column < 0) //The given coordinates do not refer to a square on the field.
		{			
			return false;
		} 
		else //The given coordinates do refer to a square on the field.
		{
			this.field[row][column] = status;
			return true;
		}			
	},

	//Returns an array containing the row and column, respectively, of the occupied square.
	getOccupiedSquareCoordinates: function()
	{
		for(var i = 0; i < this.field.length; i++)
		{
			for(var j = 0; j < this.field[0].length; j++)
			{
				if(this.getSquareStatus(i, j) == globals.STATUS_OCCUPIED)
				{
					return [i, j];
				}
			}
		}
	},

	//Returns a path that closes all the open squares on the field while visiting the fewest possible closed squares. Returns undefined if it fails to find a path after
	//working up to allowedClosedSquaresOnPath. WARNING: If you allow too many closed squares and a path does not exist with a reasonably small number of closed
	//squares, method may run for a ridiculous amount of time or cause memory errors.
	calculateForayPath: function(allowedClosedSquaresOnPath)
	{
		//Field squares with special functionality for backtracking.
		//row and column: coordinates of square on its field.
		var SquareForBacktracking = function(row, column, initialStatus)
		{	
			//Changes depending upon whether a square is closed, open, forbidden, etc.
			this.status = initialStatus;

			//The square remembers its original status.
			this.originalStatus = initialStatus;

			//The coordinates of the square on its field.
			this.row = row;
			this.column = column;

			//Returns the square's status.
			this.getStatus = function()
			{
				return this.status;
			}

			//Sets the square's status to given newStatus.
			this.setStatus = function(newStatus)
			{
				this.status = newStatus;
			}	

			//Returns an array representing the square's row and column on the field.
			this.getCoordinates = function()
			{
				return [this.row, this.column];
			};

			//This stack contains squares from which this square has branched.
			this.squaresBranchedFromStack = [];

			//Arrays in this stack contain directions we will attempt to branch in the future. 
			this.directionsToConsiderStack = [];

			//Put a [West, South, East, North] array on top of the directionsToConsider stack.
			this.addAllDirectionsToStack = function() 
			{
				this.directionsToConsiderStack.push([globals.DIRECTION_WEST, globals.DIRECTION_SOUTH, globals.DIRECTION_EAST, globals.DIRECTION_NORTH]);
			};

			//Put a square on top of the squaresBranchedFrom stack.
			this.addSquareToStack = function(square)
			{
				this.squaresBranchedFromStack.push(square);
			};		

			//If I haven't searched every direction since I've been branched to, return the next direction in line. 
			//Failing that, if I have been branched from by a square, return the last square that branched to me.
			//Return undefined if I'm out of directions and was not branched to by any square (AKA I'm out of directions and I'm the initial square)
			//Update stacks and status accordingly.
			this.getNextAction = function()
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
			};			

			//Hit up another square to continue the search.
			this.branchTo = function(square)
			{
				square.addSquareToStack(this);
				square.addAllDirectionsToStack();
				//console.log("Branching to " + square.getCoordinates());
			};			
		};

		//Create an empty 2D array of the same dimensions as this.field. This "mimickedField" will be used to evaluate possible paths.
		var mimickedField = new Array(this.field.length);
		for(var i = 0; i < this.field.length; i++) 
		{
			mimickedField[i] = new Array(this.field[0].length);
		}

		//Get our hands on the occupied square so we can treat it as closed in our mimickedField.
		var occupiedSquareCoordinates = this.getOccupiedSquareCoordinates();

		//Fill our mimickedField with SquaresForBacktracking having the same statuses as the squares in the actual field.  
		for(var i = 0; i < this.field.length; i++)
		{
			for(var j = 0; j < this.field[0].length; j++)
			{
				if(i == occupiedSquareCoordinates[0] && j == occupiedSquareCoordinates[1]) //If the square is occupied, just call it closed for simplicity's sake. 
				{
					mimickedField[i][j] = new SquareForBacktracking(i, j, globals.STATUS_CLOSED);
				}
				else //Otherwise, mimic the real field as planned.
				{
					mimickedField[i][j] = new SquareForBacktracking(i, j, this.getSquareStatus(i, j));
				}								
			}
		}

		//A field with special functionality for backtracking.
		var fieldForBacktracking = 
		{	
			//A 2D array of the squares on the field.
			squares: mimickedField,

			//Returns the SquareForBacktracking object at given coordinates.
			//Returns globals.ERROR_NOSUCHSQUARE if relative square is out of bounds.
			getSquare: function(row, column)
			{
				if(row > this.squares.length - 1 || row < 0 || column > this.squares[0].length - 1 || column < 0) //The coordinates do not refer to a square on the field.
				{
					return globals.ERROR_NOSUCHSQUARE;
				}
								
				return this.squares[row][column];
			},

			//Returns the SquareForBacktracking object in the specified direction from the given coordinates. 
			//Returns globals.ERROR_NOSUCHSQUARE if relative square is out of bounds.
			getSquareRelatively: function(row, column, direction)
			{
				if(direction == globals.DIRECTION_NORTH)
				{
					return this.getSquare(row - 1, column);
				} 
				else if(direction == globals.DIRECTION_EAST)
				{
					return this.getSquare(row, column + 1);
				}
				else if(direction == globals.DIRECTION_SOUTH)
				{
					return this.getSquare(row + 1, column);
				}
				else if(direction == globals.DIRECTION_WEST)
				{
					return this.getSquare(row, column - 1);
				}				
			},

			//Returns true if there are open squares left. Otherwise returns false.
			openSquaresRemain: function()
			{
				for(let row of this.squares) //For each row in the field...
				{
					for(let square of row) //For each square in the row...
					{
						if(square.getStatus() == globals.STATUS_OPEN) //The square is open.
						{
							return true;
						}
					}
				}

				return false;
			}
		}

		//================:::::BACKTRACKING PROCESS BEGINS:::::==========================

		//The first square to consider on our search for a path will be the occupied square because that's where the robot starts.
		var initialSquareCoordinates = occupiedSquareCoordinates.slice(0);			

		//Initialize variables that will be used in backtracking loop.
		var currentSquare;
		var actionOfCurrentSquare;
		var nextSquareToBranchTo;
		var closedSquaresOnPathSoFar = 0;
		var triedAllPossiblePaths;		
		
		//Do backtracking to find a path that will let the robot close all the open squares while visiting the fewest possible closed squares (path w/ fewest redundant steps).
		for(var allowedClosedSquares = 0; allowedClosedSquares <= allowedClosedSquaresOnPath; allowedClosedSquares++)
		{
			//Output for debugging. Will be removed eventually.
			console.log("Now looking for a path with " + allowedClosedSquares + " redundant steps");

			//We're just starting with this number of closed squares, so we haven't tried any paths yet.
			triedAllPossiblePaths = false;

			//For each number of allowed closed squares, we start our search at the occupied square.
			currentSquare = fieldForBacktracking.getSquare(initialSquareCoordinates[0], initialSquareCoordinates[1]);	

			//Since nothing will branch to the initial square, we need to tell it to consider all of the directions.
			currentSquare.addAllDirectionsToStack();
			
			while((!triedAllPossiblePaths)) //While there are still paths left to search with sufficiently few closed squares...
			{
				if((actionOfCurrentSquare = currentSquare.getNextAction()) == globals.DIRECTION_NORTH 
					|| actionOfCurrentSquare == globals.DIRECTION_EAST || actionOfCurrentSquare == globals.DIRECTION_SOUTH
					|| actionOfCurrentSquare == globals.DIRECTION_WEST) //The current square has more directions to consider.
				{
					if((nextSquareToBranchTo = fieldForBacktracking.getSquareRelatively(currentSquare.getCoordinates()[0], currentSquare.getCoordinates()[1], 
						actionOfCurrentSquare)) != globals.ERROR_NOSUCHSQUARE) //There is a square to branch to in the direction being considered by the current square.
					{
						if(nextSquareToBranchTo.getStatus() == globals.STATUS_OPEN) //The square under consideration for branching is open.
						{
							currentSquare.branchTo(nextSquareToBranchTo);
							nextSquareToBranchTo.setStatus(globals.STATUS_CLOSED);
							currentSquare = nextSquareToBranchTo;

							if(fieldForBacktracking.openSquaresRemain() == false) //The previously open square was the last one on the field. We found a path that eliminates all the open squares!
							{
								//Work backwards from the final square to extract and return the path.
								var calculatedForayPath = [currentSquare.getCoordinates()];								
								while(typeof (currentSquare = currentSquare.squaresBranchedFromStack.pop()) != "undefined") //While there is an earlier square in the path... 
								{
									//Add that earlier square's coordinates to the beginning of the calculatedForayPath array.
									calculatedForayPath.unshift(currentSquare.getCoordinates());
								}
								//Remove the starting square from the path.
								calculatedForayPath.shift();

								return calculatedForayPath; //gg no re
							} 							
						}
						else if(nextSquareToBranchTo.getStatus() == globals.STATUS_CLOSED) //The square under consideration for branching is closed.
						{
							if(closedSquaresOnPathSoFar < allowedClosedSquares) //We are still accepting closed squares. We can branch in this direction.
							{
								currentSquare.branchTo(nextSquareToBranchTo);
								currentSquare = nextSquareToBranchTo;
								closedSquaresOnPathSoFar++;								 
							}							
						}						
					}					
				}
				else //The current square has no more directions to consider.
				{
					if(typeof actionOfCurrentSquare != "undefined") //The current square was branched to by another square. Let's backtrack to that square.
					{
						if(currentSquare.getStatus() == globals.STATUS_CLOSED) //The square we are backtracking from is closed even without our being there.
						{
							//By backtracking from this square, we take a redundant step off our path.
							closedSquaresOnPathSoFar--;
						}

						//The square that most recently branched to the current square becomes the new current square. 
						currentSquare = actionOfCurrentSquare;
					}
					else //The current square was the initial square. We're out of options for this number of allowed closed squares.
					{
						triedAllPossiblePaths = true;
					}
				}
				
			}	
		}

		//Failed to find a path with as few closed squares as required.
		return undefined;	
	}
}

//Outputting your path to the console:
var calculatedPath = game.calculateForayPath(allowedRedundantSteps);
if(typeof calculatedPath != "undefined")
{
	console.log("=======Your Path========")
	for(let coordinates of calculatedPath)
	{
		console.log(coordinates);
	}
}
else
{
	console.log("Failed to find a path with fewer than " + (allowedRedundantSteps + 1) + " redundant steps. Try increasing the allowed redundant steps.")
}
