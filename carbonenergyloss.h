#ifndef CARBONENERGYLOSS_H
#define CARBONENERGYLOSS_H

#include "newtonsolver.h"
#include <QWidget>
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>

class CarbonEnergyLoss : public QWidget
{
    Q_OBJECT

public:
    explicit CarbonEnergyLoss(double E, int Z, int A, double thickness_ugcm2, QWidget *parent = nullptr);
    ~CarbonEnergyLoss();

     void CarbonCalc(double& E, double& Z, double& A, double& theta, double& phi, double& x_prime, double& y_prime);
private:
    // Члены класса для хранения параметров
    double E;
    double Z;
    double A;
    double Zt;
    double At;
    double density;
    double mt;
    double l;

    double m_thickness_m;


    double Mp;  // Масса частицы (нобелий 250) в кг
    double Mt; // Масса мишени (свинец) в кг
    double a0; // Бор радиус в метрах
    double e;
    double Na;        // Число Авогадро
    double N;
    double a;

    double totalProjectionX;
    double alpha1_previous;
    double alpha2_previous;
    int scatterCount;



    // Вспомогательные функции расчета
    double PParameter(double N, double L);
    double CalculateScreeningLength(double a0, double Zt, double Z);
    double CalculateCenterOfMassEnergy(double E, double Mp, double Mt);
    double CalculateEpsilon(double Zt, double Z, double Mt, double Mp, double E);
    double CalculateMeanFreePath(double N, double a);
    double ScatteringAngle(NewtonSolver& solver, double eps, double B, double r0, double Ec, double a);
    double electronic_stopping_power(double Z1, double A1, double Z2, double E);
};

#endif // CARBONENERGYLOSS_H
