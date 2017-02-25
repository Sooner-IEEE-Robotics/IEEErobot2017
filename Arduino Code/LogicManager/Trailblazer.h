#ifndef TRAILBLAZER_H
#define TRAILBLAZER_H

#include "Square.h"
#include <QueueList.h>
#include <StackList.h>
//#include <Vector>
//#include <algorithm>
//#include <ctime>

class Trailblazer
{
	
	public:
		Trailblazer();
		virtual ~Trailblazer();
		QueueList<int>* calculateForayPath(int row, int col, bool isStartBoard, int orientation);
		QueueList<int>* generateDirections(int orientation);
		
	protected:
		
		
	private:
		//The board
		Square **masterBoard;
		Square **startBoard;
		
		//Enumeration for Directions. I never actually use this, but keep it for reference to what the numbers mean
		enum directions {DIRECTION_NORTH = 0, DIRECTION_EAST = 1, DIRECTION_SOUTH = 2, DIRECTION_WEST = 3, DIRECTION_NONE = 10, ERROR_NOSUCHSQUARE = 11};
		
		//Redundancy Control
		int allowedRedundantSteps;
		
		//Square Operations
		Square **tempBoard;
		Square currentSquare;
		int actionOfCurrentSquare;
		Square nextSquareToBranchTo;
		int closedSquaresOnPathSoFar;
		bool triedAllPossiblePaths;

		//Board Functions
		void initializeBoard(bool isStartBoard);
		
		//Square Functions
		Square getMimickedSquare(int row, int col);
		Square* getSquareRelatively(int row, int column, int direction);
		bool openSquaresRemain();
		void resetSquares();
		
		//The list of moves required to backtrace what motions are necessary.
		StackList<int> rowDiffs, colDiffs;
	
		//Memory management at its finest
		void deleteTempBoard();
};

#endif
