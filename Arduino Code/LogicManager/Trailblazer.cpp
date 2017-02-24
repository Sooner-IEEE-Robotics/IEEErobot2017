#include "Trailblazer.h"

Trailblazer::Trailblazer()
{
	allowedRedundantSteps = 10;
	
	closedSquaresOnPathSoFar = 0;
	
	initializeBoards();
	
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


void Trailblazer::initializeBoards()
{
	//initialize the 2 boards
	masterBoard = new Square*[7];
	for(int i = 0; i < 7; ++i)
	{
		masterBoard[i] = new Square[7];
	}
	
	startBoard = new Square*[7];
	for(int i = 0; i < 7; ++i)
	{
		startBoard[i] = new Square[7];
	}
}

QueueList<int> Trailblazer::calculateForayPath(int row, int col)
{
	//initializeBoard();
	
	int occupiedSquareCoords[2] = {row, col};
	
	for(int i = 0; i < 7; ++i)
	{
		for(int j = 0; j < 7; ++j)
		{
			//board[i][j].setupSquare();
			//squares[i][j] = board[i][j];
		}
	}
	
	
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
}

