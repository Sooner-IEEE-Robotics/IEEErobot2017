void arcadeDrive(float y_power, float turn_power)
{
	float right, left;
	
	if(y_power > 0)
	{
		if(turn_power > 0)
		{
			right = y_power - turn_power;
			left = max(y_power, turn_power);
		}
		else
		{
			right = max(y_power, -turn_power);
			left = y_power + turn_power;
		}
	}
	else
	{
		if(turn_power > 0)
		{
			right = (-1) * max(-y_power, turn_power);
			left = y_power + turn_power;
		}
		else
		{
			right = y_power - turn_power;
			left = (-1) * max(-turn_power, -y_power);
		}
	}
	
	//Output to motors
}