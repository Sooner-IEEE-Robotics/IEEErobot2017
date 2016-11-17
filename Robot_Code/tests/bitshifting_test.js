var count = 0;

var i = 0, j = 0, k = 0;

for(;count<8;count++)
{
	i = (count>>2) & 1;
	j = (count>>1) & 1;
	k = count & 1;
	
	console.log(i + " " + j + " " + k);
}