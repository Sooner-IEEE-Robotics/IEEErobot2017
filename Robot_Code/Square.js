//Square Status
var explored = false;
var mainTunnel = false;
var deadEnd = false;
var obstacle = false;
var start = false;
var empty = true;
var futurePath = true;

//Square coordinates
var row = 0;
var col = 0;

var Square = function(row, col)
{
	this.row = row;
	this.col = col;
};

//Set values of the square below
Square.prototype.setExplored = function(x)
{
	this.explored = x;
}

Square.prototype.setMainTunnel = function(x)
{
	this.mainTunnel = x;
}

Square.prototype.setDeadEnd = function(x)
{
	this.deadEnd = x;
}

Square.prototype.setObstacle = function(x)
{
	this.obstacle = x;
}

Square.prototype.setStart = function(x)
{
	this.start = x;
}

Square.prototype.setEmpty = function(x)
{
	this.empty = x;
}
//Set values of the square above

//Get various statuses of the square below
Square.prototype.isExplored = function()
{
	return this.explored;
}

Square.prototype.isMainTunnel = function()
{
	return this.mainTunnel;
}

Square.prototype.isDeadEnd = function()
{
	return this.deadEnd;
}

Square.prototype.isObstacle = function()
{
	return this.obstacle;
}

Square.prototype.isStart = function()
{
	return this.start;
}

Square.prototype.isEmpty = function()
{
	return this.empty;
}
//Get various statuses of the square above

module.exports = Square;