#ifndef NEUTRONEVAPORATION_H
#define NEUTRONEVAPORATION_H

#include <QWidget>
#include <random>
#include <vector>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QPair>
#include "bindingenergydata.h"

class NeutronEvaporation : public QWidget
{
    Q_OBJECT

public:
    explicit NeutronEvaporation(double Z, double A, double U, double E, QWidget *parent = nullptr);
    ~NeutronEvaporation();

    void Evaporate_particles(double& Z, double& A, double U, double& E, int numProtons, int numNeutrons, int numAlphas,
                             double& x, double& y, double& theta, double& phi, double& x_prime, double& y_prime,
                             int& breake);//, bool &spAsked, bool &snAsked);

private:
    double probabilityDensity(double E, double C, double T);
    double Z;
    double A;
    double U;
    double E;
    int brk;
    int failureEvaporation;
    double selectedEnergy;


    //Импульсы
    double P_initial;
    double P_evaporated;
    double P_x;
    double P_y;
    double P_z;
    double P_x_evaporated;
    double P_y_evaporated;
    double P_z_evaporated;

    //углы
    double theta_evaporated;
    double phi_evaporated;

    //other
    double R0;
    double C;
    double T;
    double Zp;
    double Ap;
    double Sn;
    QMap<QPair<int,int>, double> neutronBindingCache;
    QMap<QPair<int,int>, double> protonBindingCache;


//    double x_prime;
//    double y_prime;
};

#endif // NEUTRONEVAPORATION_H
