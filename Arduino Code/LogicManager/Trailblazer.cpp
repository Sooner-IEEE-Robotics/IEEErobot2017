#include "Trailblazer.h"

Trailblazer::Trailblazer()
{
	allowedRedundantSteps = 10;
	initialMsBeforeIncreasingRedundantSteps = 5000;
	additionalMsBeforeIncreasingRedundantSteps = 7500;
	
	closedSquaresOnPathSoFar = 0;
	
	for(int r = 0; r < 7; ++r)
	{
		Vector<Square> row;
		for(int c = 0; c < 7; ++c)
		{
			if(r == 6 && c == 0)
			{
				row.push_back(Square(r,c,true));
			}
			else
			{
				row.push_back(Square(r,c,false));
			}
		}
	}
}

void Trailblazer::beginClock()
{
//	startTime = clock();
}

double Trailblazer::getElapsedTime()
{
//	return ((clock() - startTime)/(double)CLOCKS_PER_SEC) * 1000;
}

void Trailblazer::initializeBoard()
{
	
}

Vector<Square> Trailblazer::calculateForayPath(int row, int col)
{
	initializeBoard();
	
	int occupiedSquareCoords[2] = {row, col};
	
	for(int i = 0; i < 7; ++i)
	{
		for(int j = 0; j < 7; ++j)
		{
			board[i][j].setupSquare();
			squares[i][j] = board[i][j];
		}
	}
	
	
}

Trailblazer::~Trailblazer()
{
	
}

