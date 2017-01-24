#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <vector>
#include <algorithm>
using namespace std;

class PIDController
{
    public:
        PIDController(float process_init, float *k);
        virtual ~PIDController();
        float GetOutput(float setpoint, float process);
        void SetOutputRange(float upper, float lower);

    protected:

    private:
        float integrator;
        float err;
        float last_process_var;
        float kP;
        float kI;
        float kD;
        float high, low;

        float coerce(float val, float upper, float lower);
};

#endif // PIDCONTROLLER_H
