//=======================::::USE BELOW FOR TESTING::::===========================

//Build a valid field: 0 represents empty squares, 1 represents main tunnel squares, 2 represents dead end squares, and 3 represents obstacles (unknown tunnel type).
var fieldForTesting =
[
	[0, 0, 0, 1, 0, 0, 0],
	[0, 0, 0, 1, 0, 2, 3],
	[0, 0, 0, 3, 3, 2, 0],
	[0, 0, 0, 1, 3, 0, 0],
	[0, 2, 3, 1, 3, 0, 0],
	[0, 0, 0, 0, 1, 0, 0],
	[0, 0, 0, 0, 1, 0, 0]
]

//That's all you have to do! The program will draw your board in the console, replacing the obstacle squares with the inferred tunnel types. 
//The inferred types will be surrounded by brackets. For example, an inferred main tunnel segment will look like this: [1].

//=======================::::USE ABOVE FOR TESTING::::===========================

var InferenceMaker = require('../InferenceMaker.js');
var Square = require('../Square.js')

var fieldOfSquares = [];

for(var i = 0; i < 7; i++)
{
	fieldOfSquares.push(new Array(7));
}

for(var i = 0; i < 7; i++)
{
	for(var j = 0; j < 7; j++)
	{
		fieldOfSquares[i][j] = new Square(i, j);

		if(fieldForTesting[i][j] == 1)
		{
			fieldOfSquares[i][j].setMainTunnel(true);
		}
		else if(fieldForTesting[i][j] == 2)
		{
			fieldOfSquares[i][j].setDeadEnd(true);
		}
		else if(fieldForTesting[i][j] == 3)
		{
			fieldOfSquares[i][j].setObstacle(true);
		}		
	}
}

if(!(new InferenceMaker()).inferTunnelLayout(fieldOfSquares)) //The inference was unsuccessful.
{
	console.log("No valid tunnel layout could be inferred. Are you sure there is a valid layout beneath your obstacles?");
	return;
}

//The inference was successful. We draw the post-inference board:
for(var i = 0; i < 7; i++)
{
	for(var j = 0; j < 7; j++)
	{
		if(fieldOfSquares[i][j].isObstacle())
		{
			process.stdout.write('[');
		}
		else
		{
			process.stdout.write(' ');
		}

		if(fieldOfSquares[i][j].isEmpty())
		{
			process.stdout.write('0');			
		}
		else if(fieldOfSquares[i][j].isMainTunnel())
		{
			process.stdout.write('1');		
		}
		else if(fieldOfSquares[i][j].isDeadEnd())
		{
			process.stdout.write('2');				
		}

		if(fieldOfSquares[i][j].isObstacle())
		{
			process.stdout.write('] ')
		}
		else
		{
			process.stdout.write('  ');
		}
		
	}
	process.stdout.write('\n');
}

