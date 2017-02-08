#include <map>
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <ctime>

#include "PIDController.h"

using namespace std;

#define pb push_back

float constants[3] = {0.875, .0014, 0.0004};

PIDController pidController(0, constants);

float number;

int main()
{
	number = 0;

	clock_t start;
	start = clock();

	double deltaTime = 0.0;
	double lastTime = 0.0;
	//cout<<"Random Number: "<<number<<endl;

	pidController.SetOutputRange(0.8, -0.8);

    while((clock() - start)/(double)CLOCKS_PER_SEC < 5.0)
    {
        float output = pidController.GetOutput(5,number);

        deltaTime = ((clock() - start)/(double)CLOCKS_PER_SEC) - lastTime;
        number += output * deltaTime * 2.5;

        cout<<(clock()-start)/(double)CLOCKS_PER_SEC<<"\t"<<number<<endl;

        lastTime = (clock() - start)/(double)CLOCKS_PER_SEC;

        for(int i = 0; i < 10000000; i++)
        {

        }
    }

    return 0;
}