#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

using namespace std;

class PIDController
{
    public:
        PIDController(double process_init, double *k);
        virtual ~PIDController();
        double GetOutput(double setpoint, double process);
        void SetOutputRange(double upper, double lower);
        void reinitialize(double process_init);
    protected:

    private:
        double integrator;
        double err;
        double last_process_var;
        double kP;
        double kI;
        double kD;
        double high, low;

        double coerce(double val, double upper, double lower);
};

#endif // PIDCONTROLLER_H
