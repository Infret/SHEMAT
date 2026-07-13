// Реализация алгоритма моделирования углового распределения продукции MNT
// (основано на алгоритме из присланной статьи). Написано для Qt (Core).
// Файл содержит класс AngularDistributionModel и пример использования в main().
#include <QtGlobal>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <algorithm>


#include <QtGlobal>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <algorithm>


AngularDistributionModel::AngularDistributionModel(QObject *parent)
: QObject(parent), Z1(0), Z2(0), A1(0), A2(0), Ecm(1.0), Rint(1.2), Vint(0.0),
beta(1.0), delta(0.1), D_theta(1.0), l_max(300), hbarc(197.3269804)
{
}


double AngularDistributionModel::reducedMass() const {
return (A1 * A2) / (A1 + A2);
}


double AngularDistributionModel::estimate_l_gr() const {
double mu = reducedMass();
double Eeff = qMax(0.0, Ecm - Vint);
if (Eeff <= 0.0) return 0.0;
return 0.22 * Rint * qSqrt(mu * Eeff);
}


double AngularDistributionModel::ThetaC(double l) const {
double mu = reducedMass();
double p = qSqrt(2.0 * mu * qMax(1e-12, Ecm));
double b = (p > 0.0) ? (l / p) : 1e6;
double kC = (Z1 * Z2 * 1.44) / (2.0 * Ecm * qMax(1e-12, b));
return 2.0 * qAtan(kC);
}


double AngularDistributionModel::ThetaN(double l) const {
double lgr = estimate_l_gr();
if (lgr <= 0.0) return 0.0;
double x = l / lgr;
double ThetaC_gr = ThetaC(lgr);
double exponent = 0.0;
if (x > 0.0) exponent = (delta / beta) * x;
double f = qPow(x, exponent);
if (!std::isfinite(f)) f = 0.0;
return beta * ThetaC_gr * f;
}


double AngularDistributionModel::ThetaMean(double l) const {
return ThetaC(l) - ThetaN(l);
}


double AngularDistributionModel::tauInteraction(double l) const {
double mu = reducedMass();
double I_tot = mu * Rint * Rint;
double ThetaC_val = ThetaC(l);
double Theta_val = ThetaMean(l);
double numerator = qAbs(ThetaC_val - Theta_val) * I_tot;
double denom = qMax(1e-12, hbarc * l);
return numerator / denom;
}


double AngularDistributionModel::sigma2Theta(double l) const {
double tau = tauInteraction(l);
double s2 = 2.0 * D_theta * tau;
s2 = qMax(s2, 1e-8);
s2 = qMin(s2, 1e4);
return s2;
}


double AngularDistributionModel::gaussianContribution(double theta, double l) const {
double mean = ThetaMean(l);
double s2 = sigma2Theta(l);
double s = qSqrt(s2);
double diff = theta - mean;
}
