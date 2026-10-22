#include "scattering.h"

Scattering::Scattering(double E, double Z, double A, double Zt, double At, double density, double mt, double l, bool Targetflag, double tiltBeta, QWidget *parent) :
    QWidget(parent), E(E), Z(Z), A(A), Zt(Zt), At(At), density(density), mt(mt), l(l), tiltBeta(tiltBeta), Targetflag(Targetflag)
{
}

Scattering::~Scattering()
{
}


// void Scattering::ScatteringCalc(double& E, double& Z, double& A, double& Zt, double& At,
// double& density, double& mt, double& l, double& theta, double& phi,
// double& x_prime, double& y_prime, bool Targetflag,
// double tiltBeta = 0.0, bool l_is_along_beam = false);


void Scattering::ScatteringCalc(double& E, double& Z, double& A, double& Zt, double& At,
                                double& density, double& mt, double& l, double& theta, double& phi,
                                double& x_prime, double& y_prime, bool Targetflag,
                                double tiltBeta)
{
    // ---- Сохраняем начальные углы в лабораторной системе ----
    double xp_initial = x_prime;
    double yp_initial = y_prime;

    // ---- Правильное восстановление единичного вектора направления ----
    // x' = tan(theta) * cos(phi), y' = tan(theta) * sin(phi)
    double tanTheta = std::sqrt(xp_initial * xp_initial + yp_initial * yp_initial);
    double theta0 = std::atan(tanTheta);               // полярный угол в лабораторной системе
    double phi0 = std::atan2(yp_initial, xp_initial);  // азимутальный угол
    if (std::isnan(phi0)) phi0 = 0.0;

    // Единичный вектор исходного направления
    double vx0 = std::sin(theta0) * std::cos(phi0);
    double vy0 = std::sin(theta0) * std::sin(phi0);
    double vz0 = std::cos(theta0);


    // ---- Решение о повороте ----
    bool useRotation = (theta0 > 1e-6);

    // ---- Константы ----
    a0 = 5.291772105e-11; // Бор радиус в метрах
    Na = 6.02e23;        // Число Авогадро

    // ---- Вычисляем массы ----
     Mp = A * 1.66e-27;  // Масса частицы  в кг
     Mt = At * 1.66e-27; // Масса частицы мишени  в кг

    // ---- Вычисляем концентрацию атомов и длину экранирования ----
     N = density * Na / mt;
//    qDebug()<<"концентрация:"<<N;
    a = CalculateScreeningLength(a0, Zt, Z);
    double a_Angstr = a * 1e10;

    // ---- Переменные для цикла ----
    totalProjectionX = 0;
    scatterCount = 0;

    // ---- Определение эффективной толщины (ls) с учётом Targetflag ----
    double u = QRandomGenerator::global()->generateDouble();
    if (Targetflag){
        ls = l * u; // расстояние от места появления до конца мишени
//        qDebug()<<"Место появления ядра случайное";
    }
    else {
        ls = l;
//        qDebug()<<"Расчет происходит для полной длинны мишени";
    }



    // ---- Корректировка толщины с учётом угла падения и наклона мишени ----
    // Угол наклона мишени (tiltBeta) задаётся в градусах, переводим в радианы
    double tiltBetaRad = tiltBeta * M_PI / 180.0;
    double totalAngle = theta0 + tiltBetaRad;  // эффективный угол падения

    // ---- Корректировка толщины с учётом угла падения ----
    double effectiveLength = ls;
    if (useRotation) {
        double cosTilt = std::cos(totalAngle);
        if (cosTilt < 1e-12) {
            // Угол почти 90° – частица не проходит через мишень
            x_prime = xp_initial;
            y_prime = yp_initial;
            theta = theta0;
            phi = phi0;
            return;
        }
        effectiveLength = ls / cosTilt;
    }

    // ---- Переход в систему пучка (обнуляем углы) ----
    alpha1_previous = 0.0;
    theta = 0.0;
    phi = 0.0;

//    // ---- Отладочный вывод ----
//    qDebug() << "Scattering: theta0=" << theta0 << "phi0=" << phi0
//             << "ls=" << ls << "effectiveLength=" << effectiveLength
//             << "useRotation=" << useRotation;

    //qDebug()<<"ls :"<<ls;

    // ---- Основной цикл рассеяния (в системе пучка) ----
    while (totalProjectionX < effectiveLength)
    {
        // ---- Вычисляем основные параметры рассеяния ----
        a = CalculateScreeningLength(a0, Zt, Z);
        double Ec = CalculateCenterOfMassEnergy(E, Mp, Mt);
        double eps = CalculateEpsilon(Zt, Z, Mt, Mp, E);
        double L = CalculateMeanFreePath(N, a);
        if (L > (effectiveLength - totalProjectionX)) {
            break;
        }

        double P = PParameter(N, L);
        double B = P / a;

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

        // ---- Расчет угла рассеяния ----
        double thetaSC = ScatteringAngle(solver, eps, B, r0, Ec, a);
        if (std::isnan(thetaSC) || std::isinf(thetaSC)) {
            qDebug() << "Invalid thetaSC, breaking";
            break;
        }

        // ---- Преобразование углов и расчет для theta (в системе пучка) ----
        double Rn2 = QRandomGenerator::global()->generateDouble();
        double psi1 = std::atan((std::sin(thetaSC) / (std::cos(thetaSC) + (Mp / Mt))));
        if (std::isnan(psi1) || std::isinf(psi1)) {
            qDebug() << "Invalid psi1, breaking";
            break;
        }
        double phi_scatter = 2 * M_PI * Rn2;
        double alpha1 = std::acos(std::cos(alpha1_previous) * std::cos(psi1) + std::sin(alpha1_previous) * std::sin(psi1) * std::cos(phi_scatter));
        if (std::isnan(alpha1) || std::isinf(alpha1)) {
            qDebug() << "Invalid alpha1, breaking";
            break;
        }

//        if (std::isnan(thetaSC) || std::isnan(psi1) || std::isnan(phi) || std::isnan(alpha1)) {
//            qDebug() << "NaN detected in variables. Resetting values and exiting.";
//            E = 0;
//            theta = 0;
//            phi = 0;
//            x_prime = 0;
//            y_prime = 0;
//            break;
//        }

        // ---- Потеря энергии ----
        const double c = 299792458; // скорость света м/сек
        double Mp_KeV = A * 931.5 * 1000;
        double Mt_KeV = At * 931.5 * 1000;
        double Tn = 4.0 * Mt_KeV * Mp_KeV / std::pow((Mt_KeV + Mp_KeV), 2.0) * E * std::pow(std::sin(thetaSC / 2.0), 2.0);

///FIRSOV ENERGY LOSSS FORMYLAS
//        double V = c  * std::sqrt(1.0 - std::pow((Mp_KeV / (E + Mp_KeV)), 2.0));
//        double Te = 4.3e-9 * std::pow(Zt + Z, 5.0/3.0) * V /
//                std::pow(1.0 + 0.31 * P * 1e10 * std::pow(Z + Zt, 1.0/3.0), 5.0);

///OEN and ROBINSON ENERGY LOSS FORMULAS
//        double k = 1.212 * std::pow(Z, 7.0/6.0) * Zt /
//                   ( std::pow( std::pow(Z, 2.0/3.0) + std::pow(Zt, 2.0/3.0), 1.5 ) *
//                     std::sqrt(A) );
//        double Te = 0.045 * k * std::sqrt(E) / (M_PI * a * a) *
//                    std::exp(-0.3 * r0 / a) *
//                    (std::sqrt(1000.0) / (1e20 * 1000.0));

/// БЕТТЕ БЛОХ подход
        double L_Ang = L * 1e10;                     // L в метрах -> Å
        double N_Ang = N * 1e-24 / 1000000.0;        // N в см⁻³ -> Å⁻³
        double Se = electronic_stopping_power(Z, A, Zt, E * 1000); // теперь E в эВ
        double Te = L_Ang * N_Ang * Se / 1000.0;      // кэВ

//        qDebug() << "L_Ang =" << L_Ang << "Å";
//        qDebug() << "N_Ang =" << N_Ang << "Å⁻³";
//        qDebug() << "Se =" << Se << "эВ·Å²";
//        qDebug() << "Te =" << Te << "кэВ";
//        qDebug() << "Tn =" << Tn << "кэВ";
//        qDebug() << "mt =" << mt;

        double T = Tn + Te;

//        qDebug()<<Mp_KeV<<r0<<Mt_KeV<<E<<a<<V<<c<<k<<At<<A;

        E -= T;
        if (E <= 0) {
            qDebug() << "Частица потеряла всю энергию после" << scatterCount << "рассеяний.";
            break;
        }

        // ---- Накопление пройденного пути (в системе пучка) ----
        // В системе пучка частица всегда летит вперёд, поэтому cos(alpha1) >= 0.
        // Если alpha1 > pi/2, то частица развернулась назад — выходим.
        if (std::cos(alpha1) < 0) {
            break;
        }
        totalProjectionX += L * std::cos(alpha1);

//        if (tiltBeta == 0){
//            totalProjectionX += L * std::cos(alpha1);
////            qDebug()<<"Проекция на толщину мишени под прямым углом:"<<totalProjectionX;
//        }
//        else {
//            totalProjectionX += L * std::cos(alpha1) * std::cos(tiltBeta * M_PI / 180);
////            qDebug()<<"angle"<<tiltBeta;
////            qDebug()<<"cos(tiltBeta)"<<std::cos(tiltBeta * M_PI / 180);
////            qDebug()<<"Проекция на толщину мишени под углом"<<totalProjectionX;
//        }

        alpha1_previous = alpha1;
        theta = alpha1;          // угол в системе пучка
        phi = phi_scatter;       // азимут в системе пучка


        scatterCount++;
        qDebug() << "theta :" << theta<<"рассеение :"<<scatterCount;
        qDebug() << "phi :" << phi<<"рассеение :"<<scatterCount;


    }

//            qDebug()<<theta*180/M_PI;
//qDebug()<<"E после мишени - "<<E;
//qDebug()<<"Энергии на рассения суммарно- "<< Ed-E;

    // ---- Результат в системе пучка ----
    double theta_beam = theta;
    double phi_beam = phi;

    // ---- Преобразование обратно в лабораторную систему ----
    if (useRotation && !std::isnan(theta_beam) && !std::isinf(theta_beam)) {
        // Вектор направления в системе пучка
        double vx_b = std::sin(theta_beam) * std::cos(phi_beam);
        double vy_b = std::sin(theta_beam) * std::sin(phi_beam);
        double vz_b = std::cos(theta_beam);

        // 1. Поворот на -theta0 вокруг оси Y (возвращаем исходное направление)
        double vx_tmp = vx_b * std::cos(theta0) + vz_b * std::sin(theta0);
        double vy_tmp = vy_b;
        double vz_tmp = -vx_b * std::sin(theta0) + vz_b * std::cos(theta0);

        // 2. Поворот на phi0 вокруг оси Z
        double vx_lab = vx_tmp * std::cos(phi0) - vy_tmp * std::sin(phi0);
        double vy_lab = vx_tmp * std::sin(phi0) + vy_tmp * std::cos(phi0);
        double vz_lab = vz_tmp;

        // Нормализация (на случай накопления ошибок)
        double norm = std::sqrt(vx_lab * vx_lab + vy_lab * vy_lab + vz_lab * vz_lab);
        if (norm > 0) {
            vx_lab /= norm;
            vy_lab /= norm;
            vz_lab /= norm;
        }

        // Вычисляем финальные углы в лабораторной системе
        // x' = tan(theta) * cos(phi) = vx / vz, y' = vy / vz
        if (std::abs(vz_lab) > 1e-12) {
            x_prime = vx_lab / vz_lab;
            y_prime = vy_lab / vz_lab;
        } else {
            // Если vz_lab ≈ 0, частица летит перпендикулярно оси
            x_prime = (vx_lab > 0) ? 1e9 : -1e9;
            y_prime = (vy_lab > 0) ? 1e9 : -1e9;
        }

        // Обновляем theta и phi (для внешнего использования, если нужно)
        theta = std::acos(vz_lab);
        phi = std::atan2(vy_lab, vx_lab);
        if (std::isnan(phi)) phi = 0.0;

        // qDebug() << "Scattering: after rotation xp=" << x_prime << "yp=" << y_prime;
    } else {
        // Если преобразование не применялось, вычисляем из theta_beam, phi_beam
        if (!std::isnan(theta_beam) && !std::isinf(theta_beam)) {
            double vx = std::sin(theta_beam) * std::cos(phi_beam);
            double vy = std::sin(theta_beam) * std::sin(phi_beam);
            double vz = std::cos(theta_beam);
            if (std::abs(vz) > 1e-12) {
                x_prime = vx / vz;
                y_prime = vy / vz;
            } else {
                x_prime = (vx > 0) ? 1e9 : -1e9;
                y_prime = (vy > 0) ? 1e9 : -1e9;
            }
            theta = theta_beam;
            phi = phi_beam;
        }
    }
}


// Вспомогательные функции
double Scattering::PParameter(double N, double L)
{
    double Rn1 = QRandomGenerator::global()->generateDouble();
    return std::pow(-std::log(Rn1) / (M_PI * N * L), 1.0 / 2.0);
}

double Scattering::CalculateScreeningLength(double a0, double Zt, double Z)
{
    return 0.8853 * a0 / (std::pow(Zt, 0.23) + std::pow(Z, 0.23));
}

double Scattering::CalculateCenterOfMassEnergy(double E, double Mp, double Mt)
{
    return E / (1 + Mp / Mt);
}

double Scattering::CalculateEpsilon(double Zt, double Z, double Mt, double Mp, double E)
{
    return (32.53 * E * Mt) / ((Zt * Z) * (Mt + Mp) * (std::pow(Zt, 0.23) + std::pow(Z, 0.23)));
}

double Scattering::CalculateMeanFreePath(double N, double a)
{
    double Rn3 = QRandomGenerator::global()->generateDouble();
    return 1 / (N * M_PI * (2 * a) * (2 * a)) * std::log(1 / (1 - Rn3));
}

double Scattering::ScatteringAngle(NewtonSolver& solver, double eps, double B, double r0, double Ec, double a)
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
double Scattering::electronic_stopping_power(double Z1, double A1, double Z2, double E) {
    // ------------------------------------------------------------
    // 1. Линдхард–Шарфф (формула 29)
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
