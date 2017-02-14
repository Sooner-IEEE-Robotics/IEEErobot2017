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



Square::~Square()
{
	
}

