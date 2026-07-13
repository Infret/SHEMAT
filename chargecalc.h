#ifndef CHARGECALC_H
#define CHARGECALC_H

#include <QWidget>
#include <QDebug>
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>
#include <map>


class ChargeCalc : public QWidget
{
    Q_OBJECT


public:
   explicit ChargeCalc(double Z, double Mass, double Q, double E, QWidget *parent = nullptr);
    ~ChargeCalc();

    void CalculateQ(double& Z, double& Mass, double& Q, double& E);

private:
    double Mass;
    int Q;
    double E;
    double Z;
    double V;
    double X;
    double mean;



};

#endif // CHARGECALC_H
