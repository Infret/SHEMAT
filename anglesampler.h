#ifndef ANGLESAMPLER_H
#define ANGLESAMPLER_H


#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>


class AngularDistributionModel : public QObject {
Q_OBJECT
public:
explicit AngularDistributionModel(QObject *parent = nullptr);


// Параметры реакции
int Z1, Z2;
double A1, A2;
double Ecm;
double Rint;
double Vint;


// Параметры модели
double beta;
double delta;
double D_theta;


// Численные параметры
int l_max;


// Основные методы
double reducedMass() const;
double estimate_l_gr() const;
double ThetaC(double l) const;
double ThetaN(double l) const;
double ThetaMean(double l) const;
double tauInteraction(double l) const;
double sigma2Theta(double l) const;
double gaussianContribution(double theta, double l) const;
double weightForL(double l) const;


QVector<QPair<double,double>> computeDSigmaDTheta(const QVector<double>& thetaGrid,
int nL = 400) const;


QVector<double> monteCarloEvents(int nEvents, double lmin = 0.0, double lmax_in = -1.0) const;


static bool saveToCSV(const QString &filePath, const QVector<QPair<double,double>>& data);


private:
const double hbarc; // MeV*fm
};

#endif // anglesampler_H
