#include "Square.h"

Square::Square()
{
  this->row = 0;
  this->col = 0;
  this->isA7 = false;
}

Square::Square(int row, int col, bool isA7)
{
	this->row = row;
	this->col = col;
	this->isA7 = isA7;
}

//*********************SETTERS BLOCK********************//
void Square::setExplored(bool b)
{
	this->isExplored = b;
}

void Square::setMainTunnel(bool b)
{
	this->isMainTunnel = b;
}

void Square::setDeadEnd(bool b)
{
	this->isDeadEnd = b;
}

void Square::setObstacle(bool b)
{
	this->isObstacle = b;
}

void Square::setEmpty(bool b)
{
	this->_isEmpty = b;
}

void Square::setOccupied(bool b)
{
	this->isOccupied = b;
}
//*********************END SETTERS BLOCK********************//

//*********************GETTERS BLOCK********************//
bool Square::getExplored()
{
	return this->isExplored;
}

bool Square::getMainTunnel()
{
	return this->isMainTunnel;
}

bool Square::getDeadEnd()
{
	return this->isDeadEnd;
}

bool Square::getObstacle()
{
	return this->isObstacle;
}

bool Square::getEmpty()
{
	return this->_isEmpty;
}

bool Square::getOccupied()
{
	return this->isOccupied;
}
//*********************END GETTERS BLOCK********************//

//*********************BACKTRACKING BLOCK********************//
void Square::setupSquare()
{
	this->status = (!this->isExplored && !this->isObstacle) || this->isOccupied;
	
	this->originalStatus = (!this->isExplored && !this->isObstacle) || this->isOccupied;
}

bool Square::getStatus()
{
	return this->status;
}

void Square::setStatus(bool b)
{
	this->status = b;
}

int Square::getRow()
{
	return this->row;
	
}

int Square::getCol()
{
	return this->col;
	
}

void Square::addAllDirectionsToStack()
{
	this->directionsToConsiderStack.push(0); //West
	this->directionsToConsiderStack.push(1); //South
	this->directionsToConsiderStack.push(2); //East
	this->directionsToConsiderStack.push(3); //North
}

void Square::addSquareToStack(Square &s)
{
	this->squaresBranchedFromStack.push(s);
}

int Square::getNextAction()
{
	if((this->directionsToConsiderStack.count() - 1) > 0)
	{
		return this->directionsToConsiderStack.pop();
	}
	else
	{
		this->directionsToConsiderStack.pop();
		
		if(this->squaresBranchedFromStack.count() == 1)
		{
			this->setStatus(this->originalStatus);
		}
		
		return this->squaresBranchedFromStack.count()-100;
	}
}

Square Square::getPreviousSquareOnBranch()
{
	return this->squaresBranchedFromStack.pop();
}

void Square::branchTo(Square s)
{
	s.addSquareToStack(*this);
	s.addAllDirectionsToStack();
}
//*********************END BACKTRACKING BLOCK********************//


//********************INFERENCE BLOCK*************************//

//********************END INFERENCE BLOCK********************//
Square::~Square()
{
	
}

