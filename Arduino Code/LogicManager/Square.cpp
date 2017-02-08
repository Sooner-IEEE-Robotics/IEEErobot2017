#include "Square.h"

Square::Square(int row, int col, bool isA7)
{
	this.row = row;
	this.col = col;
	this.isA7 = isA7;
	
	this.isEmpty = true;
}



Square::~Square()
{
	
}