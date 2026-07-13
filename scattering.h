#ifndef SCATTERING_H
#define SCATTERING_H

#include "newtonsolver.h"
#include <QWidget>
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>
#include <QVector3D>

class Scattering : public QWidget
{
    Q_OBJECT

public:
    explicit Scattering(double E, double Z, double A, double Zt, double At, double density, double mt,
                        double l, bool Targetflag, double tiltBeta,  QWidget *parent = nullptr);
    ~Scattering();

    void ScatteringCalc(double& E, double& Z, double& A, double& Zt, double& At, double& density, double& mt, double& l,
                        double& theta, double& phi, double& x_prime, double& y_prime, bool Targetflag,
                        double tiltBeta);

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
    double ls; // расстояние от места появления до конца мишени
    double Targetflag;

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
    double tiltBeta;
//    bool l_is_along_beam ;



    // Вспомогательные функции расчета
    double electronic_stopping_power(double Z1, double A1, double Z2, double E);
    double PParameter(double N, double L);
    double CalculateScreeningLength(double a0, double Zt, double Z);
    double CalculateCenterOfMassEnergy(double E, double Mp, double Mt);
    double CalculateEpsilon(double Zt, double Z, double Mt, double Mp, double E);
    double CalculateMeanFreePath(double N, double a);
    double ScatteringAngle(NewtonSolver& solver, double eps, double B, double r0, double Ec, double a);
};

#endif // SCATTERING_H

