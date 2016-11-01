var bone = require('bonescript');
//var lib = require('./../lib/index.js');
//Test PWM Stuff

//var motor = new lib.PWM("P8_13");

console.log("Running Motor...")
bone.analogWrite('P8_13', 0.7, 2000, printJSON);
function printJSON(x) { console.log(JSON.stringify(x)); }
//motor.setOutput(255);