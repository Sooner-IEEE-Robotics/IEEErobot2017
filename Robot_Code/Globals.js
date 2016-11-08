//This file is a registry for any global variables we will need.

//Import Square object
var s = require("./Square.js");

//Debug flag
var DEBUG_MODE = false;

//Game board. 
var BOARD = [];

var CACHE_BOARD = [];

//Path Queue
var FuturePath = [];

//Current Coordinates
var currentRow = 6;
var currentColumn = 0;

//Current Direction
var d = 
{
	DIRECTION_NORTH:0, DIRECTION_EAST:1, DIRECTION_SOUTH:2, DIRECTION_WEST:3
};
var direction = d.DIRECTION_EAST;

//Coordinates in encoder units (inches)
var currentY = 0;
var currentX = 0;

//Gyro Angle
var heading = 0;

//matrix to define the desired path used to navigate the walls to find the caches. Note: true means that the space is closed as it has already been explored
var cacheBoard =
[
	[true, false, false, false, false, false, true],
    [false, false, true, true, true, false, false],
    [false, true, true, true, true, true, false],
    [false, true, true, true, true, true, false],
    [false, true, true, true, true, true, false],
    [false, false, true, true, true, false, false],
    [true, false, false, false, false, false, true]
];

//The number of caches we have found at the start
var cachesFound = 0;

//Bundle all global variables into package for user
module.exports = {
	DEBUG_MODE: DEBUG_MODE,
	BOARD: BOARD,
	CACHE_BOARD: CACHE_BOARD,
	FuturePath: FuturePath,
	cacheBoard: cacheBoard,
	currentX: currentX,
	currentY: currentY,
	direction: direction,
	heading: heading,
	currentRow: currentRow,
	currentColumn: currentColumn,
	cachesFound: cachesFound,
}