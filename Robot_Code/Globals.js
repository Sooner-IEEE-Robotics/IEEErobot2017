//This file is a registry for any global variables we will need.

//Import Square object
var s = require("./Square.js");

//Debug flag
var DEBUG_MODE = false;

//Game board
var BOARD = [];

//Path Queue
var FuturePath = [];

//Bundle all global variables into package for user
module.exports = {
	DEBUG_MODE: DEBUG_MODE,
	BOARD: BOARD,
	FuturePath: FuturePath
}