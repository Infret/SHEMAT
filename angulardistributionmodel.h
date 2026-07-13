#ifndef ANGULARDISTRIBUTIONMODEL_H
#define ANGULARDISTRIBUTIONMODEL_H

#include <QObject>
#include <random>

class angulardistributionmodel : public QObject
{
    Q_OBJECT

public:
    struct reactionparameters {
        double z1 = 0;
        double z2 = 0;
        double a1 = 0;
        double a2 = 0;
        double e_lab = 0;
    };



    explicit angulardistributionmodel(QObject* parent = nullptr);
    explicit angulardistributionmodel(const reactionparameters& params, QObject* parent = nullptr);

    // Основной метод генерации угла рассеяния
    double Generate_Scattering_Angle();

    // Метод с заданным угловым моментом
//    double Generate_Scattering_Angle_With_Momentum(double angularmomentum);

    // Установка параметров
    void setparameters(double e_lab, double z1, double a1, double z2, double a2);
    void setparameters(const reactionparameters& params);

    // Получение параметров
    double getbeta() const { return m_beta; }
    double getdelta() const { return m_delta; }
    double getdtheta() const { return m_dtheta; }
    double getetaprime() const { return m_etaprime; }
    double getcmenergy() const { return m_e_cm; }
    double getlabenergy() const { return m_e_lab; }
//    double getlastangularmomentum() const { return m_lastangularmomentum; }
//    double getlastimpactparameter() const { return m_lastimpactparameter; }

    static double degreestoradians(double degrees) { return degrees * M_PI / 180.0; }
    static double radianstodegrees(double radians) { return radians * 180.0 / M_PI; }

    static double foldAngleToZeroPi(double a) ;

    ///вывод проверка
     void debugPrintAngularDistribution(int numSamples = 10000);

private:
    double angularmomentum;
    double impactparameter;
    double m_z1 = 0, m_z2 = 0;
    double m_a1 = 0, m_a2 = 0;
    double m_e_lab = 0;
    double m_e_cm = 0;

    double m_beta = 0;
    double m_delta = 0;
    double m_dtheta = 0;
    double m_etaprime = 0;
    double m_reducedmass = 0;
    double m_interactionradius = 0;

//    double m_lastangularmomentum = 0;
//    double m_lastimpactparameter = 0;

    const double m_e_square = 1.44;
    const double m_hbarc = 197.3;

    std::mt19937 m_rng;

    void Initialize_Calculation();
    double Calculate_CM_Energy() const;
    double Calculate_Etaprime() const;
    void Calculate_Parameters(double eta_prime);

    double Coulomb_Deflection(double angularmomentum) const;
    double Coulomb_Deflection_Grazing(double angularmomentum) const;
    double Nuclear_Deflection(double angularmomentum) const;

    double Interaction_Time(double angularmomentum, double coulombangle, double totaldeflection) const;
    double Grazing_Angular_Momentum() const;
    double Interaction_Potential(double radius) const;

    double Generate_Angular_Momentum(double impactparameter);
    double Calculate_Angular_Momentum_From_Impact_Parameter(double impactparameter) const;
    double Generate_Impact_Parameter();
    double Get_Maximum_Impact_Parameter() const;
    double Calculate_Impact_Parameter_From_Angular_Momentum(double angularmomentum) const;


};

#endif // ANGULARDISTRIBUTIONMODEL_H
