var bone = require('bonescript');

bone.pinMode("P8_12",bone.INPUT);
/*
bone.digitalRead("P8_28",readDigital);

function readDigital(x)
{
	console.log(x.value);
}*/
while(true)
{
	bone.digitalRead("P8_28",function(x)
	{
		console.log(x.value);
	});
}