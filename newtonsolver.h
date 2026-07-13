#ifndef NEWTONSOLVER_H
#define NEWTONSOLVER_H

#include <cmath>

class NewtonSolver
{
public:
    NewtonSolver(double Ec, double P, double tolerance, int maxIterations, double a, double Zt, double Zp)
        : Ec(Ec), P(P), tolerance(tolerance), maxIterations(maxIterations), a(a), Zt(Zt), Zp(Zp) {}

    double V(double r);
    double V_derivative(double r);
    double solve(double r0);

private:
    double Ec;
    double P;
    double tolerance;
    int maxIterations;
    double a;
    double Zt;
    double Zp;
};

#endif // NEWTONSOLVER_H

