#include "newtonsolver.h"
#include <QDebug>





double NewtonSolver::V(double r) {
//    double R = r / a;
    double Fi = 0.35 * exp(-0.3 * r/a) + 0.55 * exp(-1.2 * r/a) + 0.1 * exp(-6 * r/a);

    //qDebug()<<"Fi"<<Fi;
    //qDebug()<<"V"<<Zt * Zp * 1.44e-12 / r * Fi;
    return Zt * Zp * 1.44e-12 / r * Fi;
}

double NewtonSolver::V_derivative(double r) {

    double Fi = 0.35 * exp(-0.3 * r/a) + 0.55 * exp(-1.2 * r/a) + 0.1 * exp(-6 * r/a);
//  qDebug()<<"Fi"<<Fi;
   //qDebug()<<"Производная V"<<Zt * Zp * 1.44e-12  * (-Fi / (r * r) - (1 / (r*a)) * (0.105 * exp(-0.3 * r/a) + 0.55 * exp(-1.2 * r/a) + 0.6 * exp(-6 * r/a)));

    return Zt * Zp * 1.44e-12  * (-Fi / (r * r) - (1 / (r*a)) * (0.105 * exp(-0.3 * r/a) + 0.66 * exp(-1.2 * r/a) + 0.6 * exp(-6 * r/a)));
}

double NewtonSolver::solve(double r0) {


    for (int i = 0; i < maxIterations; ++i) {
        // Compute function and its derivative
        double f_r0 = 1 - V(r0) / Ec - (P * P) / (r0 * r0);
        double f_prime_r0 = -V_derivative(r0) / Ec + 2 * (P * P) / (r0 * r0 * r0);
        //qDebug()<<"f_r0"<<f_r0;
          //qDebug()<<"f_prime_r0"<<f_prime_r0;


        // Check for division by zero
                if (f_prime_r0 == 0) {
                    qDebug() << "Derivative is zero!";
                    return -1;
                }
        // Newton's method correction
        double r_new = r0 - f_r0 / f_prime_r0;

//        qDebug() << "Iteration:" << i << " r0:" << r0 << " r_new:" << r_new;

        // Check for convergence
        if (std::abs(r_new - r0) < tolerance) {
            return r_new;  // Return the found solution
        }

        r0 = r_new;  // Update r0 for the next iteration
    }

    return -1;  // Return -1 if solution not found
}
