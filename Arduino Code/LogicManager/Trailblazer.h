#ifndef TRAILBLAZER_H
#define TRAILBLAZER_H

#include "Square.h"
#include <Vector.h>
//#include <Vector>
//#include <algorithm>
//#include <ctime>

class Trailblazer
{
	
	public:
		Trailblazer();
		virtual ~Trailblazer();
		Vector<Square> calculateForayPath(int row, int col);
		void setSquare(int key);
		
	protected:
		
		
	private:
		//The board
		Vector< Vector<Square> > board;
		
		//Enumeration for Directions
		enum directions {DIRECTION_NORTH = 0, DIRECTION_EAST = 1, DIRECTION_SOUTH = 2, DIRECTION_WEST = 3, DIRECTION_NONE = 10, ERROR_NOSUCHSQUARE = 11};
		
		//Redundancy Control
		int allowedRedundantSteps;
		int initialMsBeforeIncreasingRedundantSteps;
		int additionalMsBeforeIncreasingRedundantSteps;
		
		//Timing
		//clock_t startTime;
		
		//Square Operations
		Vector< Vector<Square> > squares;
		Square currentSquare;
		int actionOfCurrentSquare;
		Square nextSquareToBranchTo;
		int closedSquaresOnPathSoFar;
		bool triedAllPossiblePaths;
		bool searchedForTooLong;
		int msBeforeIncreasingAllowedClosedSquares;
		
		//Timing
		void beginClock();
		double getElapsedTime();
		
		//Board Functions
		void initializeBoard();
		
		//Square Functions
		Square getMimickedSquare(int row, int col);
		Square getSquareRelatively(int row, int column, int direction);
		bool openSquaresRemain();
		void resetSquares();
	
};

#endif
