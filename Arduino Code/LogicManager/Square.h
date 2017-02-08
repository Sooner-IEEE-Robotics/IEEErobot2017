#ifndef SQUARE_H
#define SQUARE_H

class Square
{
	
	public:
		Square(int row, int col, bool isA7);
		virtual ~Square();
		
		//Status
		void setExplored(bool b);
		void setMainTunnel(bool b);
		void setDeadEnd(bool b);
		void setObstacle(bool b);
		void setEmpty(bool b);
		void setOccupied(bool b);
		
		//Backtracking
		void setupSquare();
		bool getStatus();
		int[] getCoordinates();
		void addAllDirectionsToStack();
		void addSquareToStack();
		Square getNextAction();
		void branchTo();
		
		//Inference
		void setAdjacentToMainTunnelEndpoint();
		bool isAdjacentToMainTunnelEndpoint();
		bool isTunnel();
		void setPartOfContiguousTunnelLayout();
		bool isPartOfContiguousTunnelLayout();
		
	protected:
		
	private:
		enum directions {DIRECTION_NORTH = 0, DIRECTION_EAST = 1, DIRECTION_SOUTH = 2, DIRECTION_WEST = 3, DIRECTION_NONE = 10, ERROR_NOSUCHSQUARE = 11};
		
		//Is this the current square?
		bool isCurrent;
		
		//Coordinates
		int row;
		int col;
		
		//Status bools
		bool isExplored;
		bool isMainTunnel;
		bool isDeadEnd;
		bool isObstacle;
		bool isA7;
		bool status, originalStatus;
		
		//Backtracking Functionality
		Vector<Square> squaresBranchedFromStack;
		Vector<Square> directionsToConsiderStack;
};

#endif