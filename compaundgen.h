#ifndef COMPAUNDGEN_H
#define COMPAUNDGEN_H

#include <QWidget>
#include <random>
#include <QDebug>
#include <QRandomGenerator>


class CompaundGen : public QWidget
{
    Q_OBJECT

public:
    explicit CompaundGen(double mean, double variance, double a, double b, QWidget *parent = nullptr);
    ~CompaundGen();
    std::default_random_engine generator;
    void GenerateCoordinates(double a, double b, double& E, double& x, double& y, double& x_prime, double& y_prime);


private:
    double mean;
    double variance;
    double a;
    double b;
    double x_prime;
    double y_prime;
    double E;


    double generateParticleEnergy();
};

#endif // COMPAUNDGEN_H

