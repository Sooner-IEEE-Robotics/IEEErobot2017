//Enables us to use let-of loops.
"use strict";

var Square = require('./Square.js')

//Has some helpful functions that are used in the inferTunnelLayout method.
var Combinatorics = require('js-combinatorics');

//Global variables to be used throughout InferenceMaker.
var globals = 
{
	DIRECTION_NORTH:0, DIRECTION_EAST:1, DIRECTION_SOUTH:2, DIRECTION_WEST:3, ERROR_OUTOFBOUNDS:11, TUNNELTYPE_MAIN:12, TUNNELTYPE_DEADEND:13
};

//Constructor to make Node easy.
var InferenceMaker = function()
{

}

//Returns the Square object at the given coordinates on the given board.
//Returns globals.ERROR_OUTOFBOUNDS if relative square is out of bounds.
InferenceMaker.prototype.getSquare = function(board, row, column)
{
	if(row > board.length - 1 || row < 0 || column > board[0].length - 1 || column < 0) //The coordinates do not refer to a square on the field.
	{
		return globals.ERROR_OUTOFBOUNDS;
	}
	
	return board[row][column];
}

//Returns the Square object in the specified direction from the given coordinates. 
//Returns globals.ERROR_OUTOFBOUNDS if relative square is out of bounds.
InferenceMaker.prototype.getSquareRelatively = function(board, row, column, direction)
{
	if(direction == globals.DIRECTION_NORTH)
	{
		return this.getSquare(board, row - 1, column);
	} 
	else if(direction == globals.DIRECTION_EAST)
	{
		return this.getSquare(board, row, column + 1);
	}
	else if(direction == globals.DIRECTION_SOUTH)
	{
		return this.getSquare(board, row + 1, column);
	}
	else if(direction == globals.DIRECTION_WEST)
	{
		return this.getSquare(board, row, column - 1);
	}
}

//Returns an array of all the Square objects adjacent to the square at the specified row and column on the given board.
InferenceMaker.prototype.getNeighboringSquares = function(board, row, column)
{
	var neighboringSquares = [];

	var currentSquare;

	if((currentSquare = this.getSquareRelatively(board, row, column, globals.DIRECTION_NORTH)) != globals.ERROR_OUTOFBOUNDS)//There is a square to the north.
	{
		neighboringSquares.push(currentSquare);
	}
	if((currentSquare = this.getSquareRelatively(board, row, column, globals.DIRECTION_EAST)) != globals.ERROR_OUTOFBOUNDS)//There is a square to the north.
	{
		neighboringSquares.push(currentSquare);
	}
	if((currentSquare = this.getSquareRelatively(board, row, column, globals.DIRECTION_SOUTH)) != globals.ERROR_OUTOFBOUNDS)//There is a square to the north.
	{
		neighboringSquares.push(currentSquare);
	}
	if((currentSquare = this.getSquareRelatively(board, row, column, globals.DIRECTION_WEST)) != globals.ERROR_OUTOFBOUNDS)//There is a square to the north.
	{
		neighboringSquares.push(currentSquare);
	}

	return neighboringSquares;
}

//Returns an array of all the locations adjacent to the square at the specified row and column on the given board.
//Locations are given as arrays of length 2, with elements representing the vertical and horizontal coordinates of the location, respectively.
InferenceMaker.prototype.getNeighboringLocations = function(board, row, column)
{
	//Returns an array of the four locations adjacent to the given one, filtering out those that are not within the bounds of the board.
	//(i.e. filtering out those that are undefined)
	return [
		[row - 1, column],
		[row, column + 1],
		[row + 1, column],
		[row, column - 1]
	].filter(function(location) {return location[0] < board.length && location[0] > -1 && location[1] < board[0].length && location[1] > -1});
}

//Takes a 7x7 array of Square objects.
//Returns a 5x5 array of the 25 centermost Square objects in the 7x7, which are unchanged, 
//except that the Squares adjacent to MT endpoints become main tunnel Squares with an isAdjacentToMainTunnelEndpoint property set to true.
InferenceMaker.prototype.reduceBoard = function(board)
{
	//Prepare the 5x5 board.
	var reducedBoard = [];
	for(var i = 0; i < 5; i++)
	{
		reducedBoard.push(new Array(5));
	}

	//Loop through the 25 centermost Square objects in the 7x7 board and place them into the 5x5 board.
	for(var i = 1; i < 6; i++)
	{
		for(var j = 1; j < 6; j++)
		{
			reducedBoard[i - 1][j - 1] = board[i][j];

			if((i == 1 && this.getSquareRelatively(board, i, j, globals.DIRECTION_NORTH).isMainTunnel()) ||
				(i == 5 && this.getSquareRelatively(board, i, j, globals.DIRECTION_SOUTH).isMainTunnel()) ||
				(j == 1 && this.getSquareRelatively(board, i, j, globals.DIRECTION_WEST).isMainTunnel()) ||
				(j == 5 && this.getSquareRelatively(board, i, j, globals.DIRECTION_EAST).isMainTunnel())) //The current square is directly inward from a MT endpoint.
			{
				//The current square must be a main tunnel segment. If it was hidden by an obstacle, this is a useful inference.
				reducedBoard[i - 1][j - 1].setMainTunnel(true);

				//The square remembers that it is adjacent to a main tunnel endpoint.
				reducedBoard[i - 1][j - 1].setAdjacentToMainTunnelEndpoint(true);
								
			}
		}
	}

	return reducedBoard;
}

//Takes a board that has been reduced with the reduceBoard method.
//Returns true if all the tunnel segments on the board form a contiguous whole.
//Returns false if not.
InferenceMaker.prototype.reducedBoardHasContiguousTunnelLayout = function(reducedBoard)
{
	//Gives all Squares on the board the property isPartOfContiguousTunnelLayout = false.
	for(let row of reducedBoard)
	{
		for(let square of row)
		{
			square.setPartOfContiguousTunnelLayout(false);
		}
	}

	var tunnelSegmentCoordinates;

	//Identifies the location of a tunnel segment on the board.
	for(var i = 0; i < 5; i++)
	{
		for(var j = 0; j < 5; j++)
		{
			if(reducedBoard[i][j].isTunnel())
			{
				tunnelSegmentCoordinates = [i, j];
				break;
			}
		}
	}

	//We get a reference to the current object so we can call InferenceMaker's methods in the subsequent function.
	var referenceToInferenceMakerObject = this;

	//Takes the coordinates of a tunnel segment on a reduced board. Gives that segment and all contiguously connected tunnel segments the property isPartOfContiguousTunnelLayout = true. 
	//This function is called immediately with the location identified above.
	var addTunnelSegmentsToContiguousLayout = function(reducedBoard, row, column) 
	{
		reducedBoard[row][column].setPartOfContiguousTunnelLayout(true);

		var currentNeighboringSquare;

		//Call this function again on each neighboring tunnel segment that has not yet been marked as part of the contiguous layout. 
		for(let neighboringLocation of referenceToInferenceMakerObject.getNeighboringLocations(reducedBoard, row, column))
		{
			currentNeighboringSquare = reducedBoard[neighboringLocation[0]][neighboringLocation[1]];

			if(currentNeighboringSquare.isTunnel() && !currentNeighboringSquare.isPartOfContiguousTunnelLayout())
			{
				addTunnelSegmentsToContiguousLayout(reducedBoard, neighboringLocation[0], neighboringLocation[1]);
			}			
		}
	}	

	addTunnelSegmentsToContiguousLayout(reducedBoard, tunnelSegmentCoordinates[0], tunnelSegmentCoordinates[1]);

	//If any tunnel segments were not added to the contiguous layout, the layout must not be contiguous.
	for(let row of reducedBoard)
	{
		for(let square of row)
		{
			if(square.isTunnel() && !square.isPartOfContiguousTunnelLayout())
			{
				return false;
			}
		}
	}

	return true;
}

//Takes a board that has been reduced with the reduceBoard method.
//Returns true if two conditions are met: 
//	1) Each tunnel segment has a type and number of neighbors that is legal for that type of segment
//	2) The number of dead end segments with a main tunnel neighbor is equal to the number of dead end segments with exactly one neighbor
//Otherwise returns false.
InferenceMaker.prototype.allSquaresOnReducedBoardHaveAcceptableNeighbors = function(reducedBoard)
{
	//Initialize variables for the loop.
	var currentSquareNeighbors;
	var currentSquare;
	var numberOfDeadEndSegmentsWithAMainTunnelNeighbor = 0;
	var numberOfDeadEndSegmentsWithExactlyOneNeighbor = 0;
	
	//We loop through each of the squares on the board. If a square is a tunnel, we check that its number of neighbors and type of neighbors is relatively legal.
	//Meanwhile, we keep track of the number of dead end segments with a main tunnel neighbor and the number of dead end segments with exactly one neighbor.
	//After the loop, we will make sure these numbers are equal. This will catch any remaining illegalities in neighbor type and number and also help to prevent rings and squares.
	for(var i = 0; i < 5; i++)
	{
		for(var j = 0; j < 5; j++)
		{
			currentSquare = reducedBoard[i][j];

			if(currentSquare.isTunnel()) //The current square is a tunnel segment. We will have to evaluate its neighbors.
			{
				//Get an array of the current square's neigboring tunnel segments
				currentSquareNeighbors = this.getNeighboringSquares(reducedBoard, i, j).filter(function(neighboringSquare) {return neighboringSquare.isTunnel()});

				if(currentSquare.isMainTunnel()) //The current square is a main tunnel segment.
				{
					if(currentSquare.isAdjacentToMainTunnelEndpoint()) //The current square is adjacent to a main tunnel endpoint. 
					{
						//We confirm that the current square has exactly one main tunnel neighbor on the reduced board.
						if(currentSquareNeighbors.filter(function(neighboringSquare) {return neighboringSquare.isMainTunnel()}).length != 1) 
						{
							return false;
						}
					}
					else //The current square is a main tunnel segment but not adjacent to a main tunnel endpoint.
					{
						//We confirm that the current square has exactly two main tunnel neighbors on the reduced board.
						if(currentSquareNeighbors.filter(function(neighboringSquare) {return neighboringSquare.isMainTunnel()}).length != 2)
						{
							return false;
						}

					}
				}
				else //The current square is a dead end segment.
				{
					//We confirm that current square has either one or two tunnel neighbors.
					if(currentSquareNeighbors.length != 1 && currentSquareNeighbors.length != 2)
					{
						return false;
					}

					//We update our count if the square has exactly one tunnel neighbor.
					if(currentSquareNeighbors.length == 1)
					{
						numberOfDeadEndSegmentsWithExactlyOneNeighbor++;
					}

					//We update our count if the square has a main tunnel neighbor.
					if(currentSquareNeighbors.some(function(neighboringSquare) {return neighboringSquare.isMainTunnel();}))
					{
						numberOfDeadEndSegmentsWithAMainTunnelNeighbor++;
					}
				}
			}
			
		}
	}

	//We ensure that the number of dead end segments with a main tunnel neighbor is equal to the number of dead end segments with exactly one neighbor.
	//This will catch any remaining illegalities in neighbor type and number and also help to prevent rings and squares.
	if(numberOfDeadEndSegmentsWithAMainTunnelNeighbor != numberOfDeadEndSegmentsWithExactlyOneNeighbor)
	{
		return false;
	}

	return true;
}

//Takes a board that has been reduced with the reduceBoard method.
//Returns true if the square's tunnel types constitute a valid configuration of tunnel segments. Otherwise returns false.
//To determine whether the layout is valid, this method employs three checks:
//	1) That each tunnel segment has a type and number of neighbors that is legal for that segment
//	2) That the number of dead end segments with a main tunnel neighbor is equal to the number of dead end segments with exactly one neighbor
//	3) That the tunnel segments are contiguous
//Together, these checks are exhaustive.
InferenceMaker.prototype.reducedBoardHasValidTunnelLayout = function(reducedBoard)
{
	if(this.allSquaresOnReducedBoardHaveAcceptableNeighbors(reducedBoard) && this.reducedBoardHasContiguousTunnelLayout(reducedBoard))
	{
		return true;
	}	
	
	return false;
}

//Takes a board that has been reduced with the reduceBoard method.
//Returns an array of all the obstacle Squares whose tunnel types are still unknown.
InferenceMaker.prototype.getCandidateObstaclesFromReducedBoard = function(reducedBoard)
{	
	var candidateObstacleLocations = [];
	
	//Loop through all the squares on the board and add the candidate obstacle locations to our array.
	for(var i = 0; i < 5; i++)
	{
		for(var j = 0; j < 5; j++)
		{
			//We already know that squares adjacent to main tunnel endpoints are main tunnel segments, so we don't consider obstacles on those squares to be candidate obstacles.
			if(reducedBoard[i][j].isObstacle() && !reducedBoard[i][j].isAdjacentToMainTunnelEndpoint())
			{
				candidateObstacleLocations.push(reducedBoard[i][j]);
			}
		}
	}

	return candidateObstacleLocations;
}

//Takes a 7x7 array of Square objects, which has come from a complete and successful navigation of the board and which has not been altered since.
//Alters the array so that the obstacle Squares know their correct tunnel types. 
//Returns true if correct tunnel types were found and the alteration was performed. 
//Also returns true if tunnel types were correct already and no alteration was needed.
//Returns false if no arrangement of tunnel types made for a valid layout.
InferenceMaker.prototype.inferTunnelLayout = function(board)
{
	//Reduce the 7x7 board to a 5x5 so it's easier to work with.
	//Occasionally, the reduction process will get some trivial inferences out of the way.
	var reducedBoard = this.reduceBoard(board);

	//Maybe the obstacle Squares already know their correct tunnel types and we won't have to venture into that dubious mess of code several lines down.
	if(this.reducedBoardHasValidTunnelLayout(reducedBoard))
	{
		return true;
	}

	//No such luck. Let's get the obstacles whose tunnel types are unknown so we can start guessing different tunnel types for them.
	var candidateObstacles = this.getCandidateObstaclesFromReducedBoard(reducedBoard);

	//Initialize variables for subsequent loops.
	var combinationOfCandidateObstaclesGenerator;
	var currentCombinationOfCandidateObstacles;
	var arrangementOfTunnelTypesGenerator;
	var currentArrangementOfTunnelTypes;

	//Gradually increase the number of candidate obstacles whose tunnel types we are guessing.
	for(var numberOfCandidateObstacles = 1; numberOfCandidateObstacles <= candidateObstacles.length; numberOfCandidateObstacles++) 
	{
		combinationOfCandidateObstaclesGenerator = Combinatorics.combination(candidateObstacles, numberOfCandidateObstacles);

		//Cycle through each combination of the current number of candidate obstacles.
		while(currentCombinationOfCandidateObstacles = combinationOfCandidateObstaclesGenerator.next()) 
		{
			arrangementOfTunnelTypesGenerator = Combinatorics.baseN([globals.TUNNELTYPE_MAIN, globals.TUNNELTYPE_DEADEND], numberOfCandidateObstacles);

			//Cycle through each arrangment of possible tunnel types for the current combination of candidate obstacles.
			while(currentArrangementOfTunnelTypes = arrangementOfTunnelTypesGenerator.next()) 
			{
				//Apply the current arrangment of tunnel types to the current combination of candidate obstacles. 
				for(var i = 0; i < numberOfCandidateObstacles; i++)
				{
					if(currentArrangementOfTunnelTypes[i] == globals.TUNNELTYPE_MAIN)
					{
						currentCombinationOfCandidateObstacles[i].setMainTunnel(true);
					}
					else
					{
						currentCombinationOfCandidateObstacles[i].setDeadEnd(true);
					}
				}

				if(this.reducedBoardHasValidTunnelLayout(reducedBoard)) //The current arrangement of tunnel types makes for a legal network of tunnels. Our work here is done!
				{
					//Modifications made to the reduced board affected the 7x7 board as well because the reduced board referred to the same Square objects.
					//Thus, the tunnel types we just gave to obstacle Squares on the reduced board have been inferred on the original board as well. We're done.
					return true;
				}
			}

			//We're leaving this combination of candidate obstacles behind. Reset them to non-tunnel status so our failed guesses don't confuse future results.
			for(let obstacleSquare of currentCombinationOfCandidateObstacles)
			{
				obstacleSquare.setEmpty(true);
			}
		}
		
	}

	//We tried every combination of candidate obstacles and came up dry.
	return false;		
}

module.exports = InferenceMaker;