var blaze = require('../TrailBlazer.js');
var square = require('../Square.js');

//Set the field up for testing. false means it is open, true means there is an obstacle or that the space is forbidden or already explored
var closed =
[
	[false, false, false, false, false, false, false],
    [false, false, false, false, false, false, false],
    [true, false, false, false, false, false, false],
    [false, false, false, false, false, false, false],
    [true, false, false, false, true, false, true],
    [true, true, true, true, true, true, true],
    [true, true, true, true, true, true, true]


];

var board = [];

for(var row = 0; row<7; ++row)
{
	var r = [];
	
	for(var col = 0; col<7; ++col)
	{
		var sq = new square(row,col);
		sq.setExplored(closed[row][col]);
		r.push(sq);
	}
	
	board.push(r);
}

board[5][1].setObstacle(true);

var plan = new blaze();
var queue = plan.calculateForayPath(board, 5, 2);

console.log(queue);

var trail = 
[
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0],
[0,0,0,0,0,0,0]
]
console.log("=======Your Path========")
var i = 1;
queue.forEach(function(coordinates)
{
	trail[coordinates[0]][coordinates[1]] = i;
	++i;
});

console.log(trail);