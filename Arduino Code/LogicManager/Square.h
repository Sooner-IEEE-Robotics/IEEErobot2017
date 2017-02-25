#ifndef SQUARE_H
#define SQUARE_H

#include <StackList.h>

class Square
{
	
	public:
    Square();
		Square(int row, int col, bool isA7);
		virtual ~Square();
		
		//Status
		void setExplored(bool b);
		void setMainTunnel(bool b);
		void setDeadEnd(bool b);
		void setObstacle(bool b);
		void setEmpty(bool b);
		void setOccupied(bool b);
		
		//Getters
		bool getExplored();
		bool getMainTunnel();
		bool getDeadEnd();
		bool getObstacle();
		bool getEmpty();
		bool getOccupied();
		
		//Backtracking
		void setupSquare();
		bool getStatus();
		void setStatus(bool b);
		int getRow();
		int getCol();
		void addAllDirectionsToStack();
		void addSquareToStack(Square &s);
		int getNextAction();
		Square getPreviousSquareOnBranch();
		void branchTo(Square s);
		
		//Inference
		void setAdjacentToMainTunnelEndpoint();
		bool isAdjacentToMainTunnelEndpoint();
		bool isTunnel();
		void setPartOfContiguousTunnelLayout();
		bool isPartOfContiguousTunnelLayout();

		//Backtracking Functionality
		StackList<Square> squaresBranchedFromStack;
		StackList<int> directionsToConsiderStack;    
		
	protected:
		
	private:
		enum directions {DIRECTION_NORTH = 0, DIRECTION_EAST = 1, DIRECTION_SOUTH = 2, DIRECTION_WEST = 3, DIRECTION_NONE = 10, ERROR_NOSUCHSQUARE = 11};
		
		//Is this the current square?
		bool isCurrent;
		
		//Coordinates
		int row;
		int col;
		
		//Status bools
		bool isExplored = false;
		bool isMainTunnel = false;
		bool isDeadEnd = false;
		bool isObstacle = false;
		bool isA7 = false;
		bool _isEmpty = false;
		bool isOccupied = false;
		bool status, originalStatus;
		
   
};

#endif
