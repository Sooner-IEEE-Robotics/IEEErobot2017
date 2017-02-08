#include "PIDController.h"

PIDController::PIDController(double process_init, double *k)
{
    last_process_var = process_init;
    err = 0;
    integrator = 0;

    kP = k[0];
    kI = k[1];
    kD = k[2];

    low = -1;
    high = 1;
}

void PIDController::reinitialize(double process_init)
{
    last_process_var = process_init;

    err = 0;
    integrator = 0;
}

double PIDController::GetOutput(double setpoint, double process)
{
    err = setpoint - process;
    double P = kP * err;

    integrator += err;
    integrator = coerce(integrator, high, low);
    double I = kI * integrator;

    double delta = process - last_process_var;
    double D = kD * delta;

    double output = coerce(P + I + D, high, low);

    last_process_var = process;

    return output;
}

double PIDController::coerce(double val, double upper, double lower)
{
    if(val > upper)
    {
        return upper;
    }
    else if(val < lower)
    {
        return lower;
    }
    return val;
}

void PIDController::SetOutputRange(double upper, double lower)
{
	high = upper;
	low = lower;
}

PIDController::~PIDController()
{
    //dtor
}
