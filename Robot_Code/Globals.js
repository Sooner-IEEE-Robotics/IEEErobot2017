//This file is a registry for any global variables we will need.

//Import Square object
var s = require("./Square.js");

//Debug flag
var DEBUG_MODE = false;

//Game board
var BOARD = [];

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

//Bundle all global variables into package for user
module.exports = {
	DEBUG_MODE: DEBUG_MODE,
	BOARD: BOARD,
	FuturePath: FuturePath
}