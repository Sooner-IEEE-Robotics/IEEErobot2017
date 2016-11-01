var bone = require('bonescript');
var lib = require('../lib/index.js');
//Test PWM Stuff

var motor = new lib.PWM("P8_13");
motor.setOutput(1.0);

console.log("Running Motor...")
//bone.analogWrite('P8_13', 0.7, 2000, printJSON);


