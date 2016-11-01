
//The program allows for more and more redundant steps as it fails to find optimal paths. Decide when you want it to just give up:
var allowedRedundantSteps = 3;

//With a lot of open squares on the board, looking for a path with only a few redundant squares can take an absurd amount of time. 
//After searching for this many milliseconds, the program will decide to speed up the calculation but accept another redundant square on the path. 
var initialMillisecondsBeforeIncreasingRedundantSteps = 5000;

//Every time the program increases the number of redundant steps it will aim for, it increases the milliseconds it is willing to wait by this much. 
//If this is zero, it waits the same amount no matter how many redundant steps it is allowing.
var additionalMillisecondsBeforeIncreasingRedundantSteps = 7500;

//Constructor to make Node easy
var TrailBlazer = function(){};



