#include "Trailblazer.h"

Trailblazer::Trailblazer()
{
	allowedRedundantSteps = 10;
	
	closedSquaresOnPathSoFar = 0;
	
	//initialize the main board
	masterBoard = new Square*[7];
	for(int i = 0; i < 7; ++i)
	{
		masterBoard[i] = new Square[7];
	}
	
	//initialize the board for cache exploration.
	startBoard = new Square*[7];
	for(int i = 0; i < 7; ++i)
	{
		startBoard[i] = new Square[7];
	}
	
	//Fill the boards with squares
	for(int r = 0; r < 7; ++r)
	{
		for(int c = 0; c < 7; ++c)
		{
			if(r == 6 && c == 0)
			{
				masterBoard[r][c] = Square(r,c,true);
				startBoard[r][c] = Square(r,c,true);
				
				startBoard[r][c].setOccupied(true);
				masterBoard[r][c].setOccupied(true);
				
				startBoard[r][c].setExplored(true);
				masterBoard[r][c].setExplored(true);
			}
			else if(!(r==0 || r==6 || c==0 || c==6))//If the Square is not on the edges, set to explored at the start.
			{
				startBoard[r][c] = Square(r,c,false);
				startBoard[r][c].setExplored(true);
			}
			else
			{
				masterBoard[r][c] = Square(r,c,false);
				startBoard[r][c] = Square(r,c,false);
			}
		}
	}
}


void Trailblazer::initializeBoard(bool isStartBoard)
{
	
	tempBoard = new Square*[7];
	for(int i = 0; i < 7; ++i)
	{
		tempBoard[i] = new Square[7];
	}
	
	if(isStartBoard)
	{
		for(int r = 0; r < 7; ++r)
		{
			for(int c = 0; c < 7; ++c)
			{
				tempBoard[r][c] = startBoard[r][c];
				tempBoard[r][c].setupSquare();
			}
		}
	}
	else
	{
		for(int r = 0; r < 7; ++r)
		{
			for(int c = 0; c < 7; ++c)
			{
				tempBoard[r][c] = masterBoard[r][c];
				tempBoard[r][c].setupSquare();
			}
		}
	}
}

//Returns the Square object in the specified direction from the given coordinates. 
//Returns globals.ERROR_NOSUCHSQUARE if relative square is out of bounds.
Square* Trailblazer::getSquareRelatively(int row, int column, int direction)
{
	if(direction == 0)
	{
		return &tempBoard[row - 1][column]; //NORTH
	} 
	else if(direction == 1)
	{
		return &tempBoard[row][column + 1]; //EAST
	}
	else if(direction == 2)
	{
		return &tempBoard[row + 1][column]; //SOUTH
	}
	else if(direction == 3)
	{
		return &tempBoard[row][column - 1]; //WEST
	}
	
	return nullptr; //NOSUCHSQUARE (not 11 for type reasons)
}


//Returns true if there are open squares left. Otherwise returns false.
bool Trailblazer::openSquaresRemain()
{
	for(int r = 0; r < 7; ++r)
	{
		for(int c = 0; c < 7; ++c)
		{
			if(tempBoard[r][c].getStatus())
			{
				return true;
			}
		}
	}
	
	return false;
}

QueueList<int>* Trailblazer::generateDirections(int orientation)
{	
	int rDiff;
	int cDiff;
	
	QueueList<int> directions;
	
	//1 SEC PAUSE: 0
	//FORWARD: 1
	//LEFT: 2
	//RIGHT: 3
	//UTURN: 4
	//CACHE : 5 //N/A, this will be done on the fly.
	//0.5 SEC PAUSE: 6
	//STOP: 7
	while(rowDiffs.count() > 0)
	{
		rDiff = this->rowDiffs.pop();
		cDiff = this->colDiffs.pop();
		
		if(rDiff > 0) // We are moving down (board reference)(SOUTH)
		{
			if(orientation == 0)//if north
			{
				directions.push(4); //UTURN
				directions.push(1); //FORWARD
				directions.push(0); //PAUSE
			}
			else if(orientation == 1)//if east
			{
				directions.push(3);	//RIGHT
				directions.push(1);	//FORWARD
				directions.push(0);	//PAUSE
			}
			else if(orientation == 2)//South
			{
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
			else//West
			{
				directions.push(2); //Left
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
		}
		else if(rDiff < 0) //We are moving up (board reference)(NORTH)
		{
			if(orientation == 2)//if south
			{
				directions.push(4); //UTURN
				directions.push(1); //FORWARD
				directions.push(0); //PAUSE
			}
			else if(orientation == 3)//if west
			{
				directions.push(3);	//RIGHT
				directions.push(1);	//FORWARD
				directions.push(0);	//PAUSE
			}
			else if(orientation == 0)//north
			{
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
			else//East
			{
				directions.push(2); //Left
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
		}
		else if(cDiff > 0)//We are moving right (board reference)(EAST)
		{
			if(orientation == 3)//west
			{
				directions.push(4); //UTURN
				directions.push(1); //FORWARD
				directions.push(0); //PAUSE
			}
			else if(orientation == 0)//north
			{
				directions.push(3);	//RIGHT
				directions.push(1);	//FORWARD
				directions.push(0);	//PAUSE
			}
			else if(orientation == 1)//east
			{
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
			else//south
			{
				directions.push(2); //Left
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
		}
		else if(cDiff < 0) //We are moving left (board reference)(WEST)
		{
			if(orientation == 1)//east
			{
				directions.push(4); //UTURN
				directions.push(1); //FORWARD
				directions.push(0); //PAUSE
			}
			else if(orientation == 2)//south
			{
				directions.push(3);	//RIGHT
				directions.push(1);	//FORWARD
				directions.push(0);	//PAUSE
			}
			else if(orientation == 3)//west
			{
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
			else//north
			{
				directions.push(2); //Left
				directions.push(1); //Forward
				directions.push(0); //Pause
			}
		}
	}
	
	directions.push(7);
	
	return &directions;
}

void Trailblazer::deleteTempBoard()
{
	for(int i = 0; i < 7; ++i)
	{
		delete [] tempBoard[i];
	}
	delete [] tempBoard;
}

QueueList<int>* Trailblazer::calculateForayPath(int row, int col, bool isStartBoard, int orientation)
{
	//Fill our mimickedField with SquaresForBacktracking having the same statuses as the squares in the actual field.
	initializeBoard(isStartBoard);
	
	//Get our hands on the occupied square so we can treat it as closed in our mimickedField.
	int occupiedSquareCoords[2] = {row, col};
	
	//**********************BACKTRACKING PROCESS BEGINS***************************//
	for(int allowedClosedSquares = 0; allowedClosedSquares <= allowedRedundantSteps; ++allowedClosedSquares)
	{
		//We're just starting with this number of closed squares, so we haven't tried all the paths or searched too long yet.
		this->triedAllPossiblePaths = false;
		
		//For each number of allowed closed squares, we start our search at the occupied square.
		this->currentSquare = tempBoard[occupiedSquareCoords[0]][occupiedSquareCoords[1]];
		
		//Since nothing will branch to the initial square, we need to tell it to consider all of the directions.
		this->currentSquare.addAllDirectionsToStack();
		
		//Search while there are still paths left to search for.
		while(!triedAllPossiblePaths)
		{
			this->actionOfCurrentSquare = this->currentSquare.getNextAction();
			if(this->actionOfCurrentSquare == 0 || this->actionOfCurrentSquare == 1 || this->actionOfCurrentSquare == 2 || this->actionOfCurrentSquare == 3)
			{
				this->nextSquareToBranchTo = *(this->getSquareRelatively(this->currentSquare.getRow(), this->currentSquare.getCol(), this->actionOfCurrentSquare));
				
				//If the next Square to branch to actually is on the board
				if(&(this->nextSquareToBranchTo) != nullptr)
				{
					if(this->nextSquareToBranchTo.getStatus() == true)//The square under consideration for branching is open.
					{
						this->currentSquare.branchTo(this->nextSquareToBranchTo);
						this->nextSquareToBranchTo.setStatus(false);
						this->currentSquare = this->nextSquareToBranchTo;
						
						if(this->openSquaresRemain() == false)
						{
							int currentRow = this->currentSquare.getRow();
							int currentCol = this->currentSquare.getCol();
							int nextRow, nextCol;
							Square prevSquare;
							
							//TODO: Make the queue of directions
							while(this->currentSquare.squaresBranchedFromStack.count() > 0)
							{
								prevSquare = currentSquare.squaresBranchedFromStack.pop();
								
								nextRow = prevSquare.getRow();
								nextCol = prevSquare.getCol();
								
								this->rowDiffs.push(currentRow - nextRow);
								this->colDiffs.push(currentCol - nextCol);
								
								currentRow = nextRow;
								currentCol = nextCol;
							}
							
							deleteTempBoard();
							return this->generateDirections(orientation);
						}
					}
					else if(this->nextSquareToBranchTo.getStatus() == false && this->nextSquareToBranchTo.getObstacle() == false)//The square under consideration for branching is closed, but it is not an obstacle.
					{
						if(this->closedSquaresOnPathSoFar < allowedClosedSquares)
						{
							this->currentSquare.branchTo(this->nextSquareToBranchTo);
							this->currentSquare = this->nextSquareToBranchTo;
							this->closedSquaresOnPathSoFar++;
						}
					}
				}
			}
			else
			{
				if(this->actionOfCurrentSquare > -100)
				{
					if(this->currentSquare.getStatus() == false)
					{
						this->closedSquaresOnPathSoFar--;
					}
					
					this->currentSquare = this->currentSquare.getPreviousSquareOnBranch();
				}
				else//The current square was the initial square. We're out of options for this number of allowed closed squares.
				{
					this->triedAllPossiblePaths = true;
				}
			}
		}
	}
	
	deleteTempBoard();
	return nullptr;
	
}

Trailblazer::~Trailblazer()
{
	for(int i = 0; i < 7; ++i)
	{
		delete [] masterBoard[i];
	}
	delete [] masterBoard;
	
	for(int i = 0; i < 7; ++i)
	{
		delete [] startBoard[i];
	}
	delete [] startBoard;
	
	for(int i = 0; i < 7; ++i)
	{
		delete [] tempBoard[i];
	}
	delete [] tempBoard;
}

