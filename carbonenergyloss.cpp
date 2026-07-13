#include "carbonenergyloss.h"


const double CARBON_DENSITY_KG_M3 = 2250.0;

CarbonEnergyLoss::CarbonEnergyLoss(double E, int Z, int A, double thickness_ugcm2, QWidget *parent) :
    QWidget(parent), E(E), Z(Z), A(A)
{
    double surface_density_kg_m2 = thickness_ugcm2 * 1e-5;
    m_thickness_m = surface_density_kg_m2 / CARBON_DENSITY_KG_M3;
}

CarbonEnergyLoss::~CarbonEnergyLoss()
{
}

void CarbonEnergyLoss::CarbonCalc(double& E, double& Z, double& A, double& theta, double& phi, double& x_prime, double& y_prime)
{

    //ПАРАМЕТРЫ УГЛЕРОДНОЙ ФОЛЬГИ-МИШЕНИ
    // Параметры углеродной мишени (фиксированные)
       At = 12;
       Zt = 6;
       mt = At * 1.6726219e-27 * 6.022e23;
       density = CARBON_DENSITY_KG_M3; // плотность в кг/м³
       l = m_thickness_m; // используем переданную толщину

      // Константы
       Mp = A * 1.66e-27;  // Масса частицы в кг
       Mt = At * 1.66e-27; // Масса мишени в кг
       a0 = 5.291772105e-11; // Боровский радиус (м)
       e = 1.44e-12;
       Na = 6.02e23;        // Число Авогадро

      // Вычисляем концентрацию атомов и длину экранирования
      N = density * Na / mt;
      a = CalculateScreeningLength(a0, Zt, Z);

      double totalProjectionX = 0;
      alpha1_previous = theta;
      alpha2_previous = phi;
      int scatterCount = 0;

    // Основной цикл рассеяния
    while (totalProjectionX < l)
    {
        // Вычисляем основные параметры рассеяния
        double Ec = CalculateCenterOfMassEnergy(E, Mp, Mt);
        double eps = CalculateEpsilon(Zt, Z, Mt, Mp, E);
        double L = CalculateMeanFreePath(N, a);
        double P = PParameter(N, L);
        double B = P / a;



        if (L > (l - totalProjectionX)) {
               break;
           }

        // Настройка параметров для уравнения Ньютона
        double tolerance = 1e-11;
        int maxIterations = 10;
        double r0 = P;

        NewtonSolver solver(E, P, tolerance, maxIterations, a, Zt, Z);
        double r_solution = solver.solve(r0);

        if (r_solution == -1) {
            qDebug() << "Solution not found.";
            break;
        }

        r0 = r_solution;

        // Расчет угла рассеяния
        double thetaSC = ScatteringAngle(solver, eps, B, r0, Ec, a);

        // Преобразование углов и расчет для theta
        double Rn2 = QRandomGenerator::global()->generateDouble();
        double psi1 = std::atan((std::sin(thetaSC) / (std::cos(thetaSC) + (Mp / Mt))));
        double phi = 2 * M_PI * Rn2;
        double alpha1 = std::acos(std::cos(alpha1_previous) * std::cos(psi1) + std::sin(alpha1_previous) * std::sin(psi1) * std::cos(phi));


        // Потеря энергии
        const double c = 299792458;//скорость света м/сек
        double Mp_KeV = A * 931.5 * 1000;
        double Mt_KeV = At * 931.5 * 1000;

        double Tn = 4.0 * Mt_KeV * Mp_KeV / std::pow((Mt_KeV + Mp_KeV), 2.0) * E * std::pow(std::sin(thetaSC / 2.0), 2.0);

//        double V = c  * std::sqrt(1.0 - std::pow((Mp_KeV / (E + Mp_KeV)), 2.0));


        double L_Ang = L * 1e10;                     // L в метрах -> Å
        double N_Ang = N * 1e-24 / 1000000.0;                    // N в см⁻³ -> Å⁻³

        double Se = electronic_stopping_power(Z, A, Zt, E * 1000); // теперь E в эВ
        double Te = L_Ang * N_Ang * Se / 1000.0;      // кэВ


//        double Te = 4.3e-9 * std::pow((Zt + Z), 5.0/3.0) * V / std::pow((1 + 0.31 * P * std::pow((Z + Zt), 1.0 / 3.0)), 5.0);
        double T = Tn + Te;
//        qDebug()<<"Vcarb--------"<<V;
//        qDebug()<<"TnCarbon-------"<<Tn;
//        qDebug()<<"TеCarbon-------"<<Te;
        //qDebug()<<"delta E carbon = "<<T;


        E -= T;
        if (E <= 1) {
            qDebug() << "Частица потеряла всю энергию после" << scatterCount << "рассеяний.";
            break;
        }

        totalProjectionX += L * std::cos(alpha1);
        alpha1_previous = alpha1;
        theta = alpha1;



        // Вычисляем новые углы x' и y'
        x_prime = std::atan2(std::sin(theta) * std::cos(phi), std::cos(theta));
        y_prime = std::atan2(std::sin(theta) * std::sin(phi), std::cos(theta));


//         qDebug() << "theta :" << theta;
//         qDebug() << "phi :" << phi;
        scatterCount++;
    }
//qDebug()<<theta*180/M_PI;
//qDebug()<<"Энергии на рассения В ФОЛЬГЕ суммарно - "<< Ed-E;
//qDebug()<<"E"<<E;
}
// Вспомогательные функции
double CarbonEnergyLoss::PParameter(double N, double L)
{
    double Rn1 = QRandomGenerator::global()->generateDouble();
    return std::pow(-std::log(Rn1) / (M_PI * N * L), 1.0 / 2.0);
}

double CarbonEnergyLoss::CalculateScreeningLength(double a0, double Zt, double Z)
{
    return 0.8853 * a0 / (std::pow(Zt, 0.23) + std::pow(Z, 0.23));
}

double CarbonEnergyLoss::CalculateCenterOfMassEnergy(double E, double Mp, double Mt)
{
    return E / (1 + Mp / Mt);
}

double CarbonEnergyLoss::CalculateEpsilon(double Zt, double Z, double Mt, double Mp, double E)
{
    return (32.53 * E * Mt) / ((Zt * Z) * (Mt + Mp) * (std::pow(Zt, 0.23) + std::pow(Z, 0.23)));
}

double CarbonEnergyLoss::CalculateMeanFreePath(double N, double a)
{
    double Rn3 = QRandomGenerator::global()->generateDouble();
    return 1 / (N * M_PI * (2 * a) * (2 * a)) * std::log(1 / (1 - Rn3));
}

double CarbonEnergyLoss::ScatteringAngle(NewtonSolver& solver, double eps, double B, double r0, double Ec, double a)
{
    double R0 = r0 / a;
    double rc = -2 * ((Ec - solver.V(r0)) / solver.V_derivative(r0));
    double Rc = rc / a;

    // Константы для расчета
    double C1 = 0.6743, C2 = 0.009611, C3 = 0.005175, C4 = 10.0, C5 = 6.314;

    // Расчет параметров угла рассеяния
    double x1 = 1 + C1 * std::pow(eps, -0.5);
    double x2 = (C2 + std::sqrt(eps)) / (C3 + std::sqrt(eps));
    double x3 = (C4 + eps) / (C5 + eps);
    double A = 2 * x1 * eps * std::pow(B, x2);
    double G = x3 / (std::sqrt(1 + A * A) - A);

    double delta = A * (R0 - B) / (1 + G);
    double argument = (B + Rc + delta) / (R0 + Rc);
    return 2 * std::acos(argument);
}
///// БЕТТЕ БЛОХ подход
double CarbonEnergyLoss::electronic_stopping_power(double Z1, double A1, double Z2, double E) {
    // ------------------------------------------------------------
    // 1. Линдхард–Шарфф
    // ------------------------------------------------------------
    double k = 1.212 * std::pow(Z1, 7.0/6.0) * Z2 /
               ( std::pow( std::pow(Z1, 2.0/3.0) + std::pow(Z2, 2.0/3.0), 1.5 ) *
                 std::sqrt(A1) );                    // эВ^(1/2)·Å²
    double SL = k * std::sqrt(E);                     // эВ·Å²   (E в эВ)

    // ------------------------------------------------------------
    // 2. Бете–Блох с модификацией
    // ------------------------------------------------------------
    double I0 = (Z2 < 13) ? (12.0 + 7.0 * Z2) : (9.76 + 58.5 * std::pow(Z2, -1.19)); // эВ
    double M1c2 = A1 * 931.5e6;                        // эВ (энергия покоя)

    // Скорость (нерелятивистская)
    double v2_c2 = 2.0 * E / M1c2;                     // (v/c)²
    double c = 299792458.0;                            // м/с
    double v2 = v2_c2 * c * c;                         // v² в м²/с²

    const double m_e = 9.1093837e-31;                  // кг
    const double eV_to_J = 1.60217662e-19;             // Дж/эВ
    double m_e_v2_eV = m_e * v2 / eV_to_J;             // эВ

    double eps_B = 2.0 * m_e_v2_eV / (Z2 * I0);        // безразмерный

    double C = (Z1 < 3) ? (100.0 * Z1 / Z2) : 5.0;
    const double e2 = 14.4;                             // эВ·Å
    double pre = 8.0 * M_PI * std::pow(Z1, 2) * std::pow(e2, 2) / (I0 * eps_B); // эВ·Å²
    double SB = pre * std::log(eps_B + 1.0 + C / eps_B); // эВ·Å²

    // ------------------------------------------------------------
    // 3. Интерполяция (формула 32)
    // ------------------------------------------------------------
    return 1.0 / (1.0 / SL + 1.0 / SB);                 // эВ·Å²
}
////////////////////////////////////////////////////////
