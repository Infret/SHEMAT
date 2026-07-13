#ifndef LANGEVINMODEL_H
#define LANGEVINMODEL_H

#include <QObject>
#include <QVector>
#include <random>
#include <cmath>

class LangevinModel : public QObject
{
    Q_OBJECT

public:
    explicit LangevinModel(QObject *parent = nullptr);

    // Основные методы для расчета угла рассеяния
    double calculateScatteringAngle(int projectileA, int projectileZ,
                                   int targetA, int targetZ,
                                   double E_lab, double impactParameter);

    // Метод для множественного моделирования
    QVector<double> simulateAngularDistribution(int projectileA, int projectileZ,
                                              int targetA, int targetZ,
                                              double E_lab,
                                              int nTrajectories = 1000);

    // Методы для работы с прицельным параметром
    double calculateMaxImpactParameter(int projectileA, int projectileZ,
                                     int targetA, int targetZ, double E_lab);
    double generateImpactParameter(int projectileA, int projectileZ,
                                 int targetA, int targetZ, double E_lab);
    double calculateCrossSection(double b_max);
    double calculateGrazingImpactParameter(int projectileA, int projectileZ,
                                         int targetA, int targetZ, double E_lab);

    // Вспомогательные методы
    double calculateCMEnergy(int projectileA, int targetA, double E_lab);
    double calculateGrazingAngle(int projectileA, int projectileZ,
                                int targetA, int targetZ, double E_lab);

    double generateRandomAngle(int projectileA, int projectileZ,
                                  int targetA, int targetZ,
                                  double E_lab);

private:
    // Параметры из статьи Karpov2017
    const double GAMMA_R0 = 30.0e-22;    // MeV·s/fm²
    const double DELTA_RF = 2.5;         // fm
    const double A_F = 0.2;              // fm
    const double LAMBDA_A0 = 1.0e22;     // s⁻¹
    const double TAU_DA = 1.0e-22;       // s
    const double TAU_EPSILON = 1.0e-21;  // s

    // Константы
    const double HC = 197.3269718;       // MeV·fm
    const double ALPHA = 1.0 / 137.035999;
    const double MASS_NUCLEON = 931.5;   // MeV/c²

    // Случайный генератор
    std::mt19937 generator;
    std::normal_distribution<double> normalDist;

    struct DynamicsResult {
        double angle;        // угол рассеяния в радианах
        double finalEnergy;  // конечная энергия в МэВ
        double reactionTime; // время реакции в секундах
    };

    // Вспомогательные методы
    double calculateReducedMass(int A1, int A2);
    double calculateCoulombBarrier(int Z1, int Z2, int A1, int A2);
    double calculateContactDistance(int A1, int A2);
    double woodSaxonFormFactor(double r, double R_contact);
    double oneBodyFriction(double r, double R_contact);
    double phenomenologicalFriction(double r, double R_contact);
    double potentialEnergy(double r, int Z1, int Z2, int A1, int A2);

    DynamicsResult solveLangevinEquations(int projectileA, int projectileZ,
                                         int targetA, int targetZ,
                                         double E_cm, double b);
};

#endif // LANGEVINMODEL_H
