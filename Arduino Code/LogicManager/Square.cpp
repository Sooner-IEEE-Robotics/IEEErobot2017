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
//*********************END BACKTRACKING BLOCK********************//
Square::~Square()
{
	
}

