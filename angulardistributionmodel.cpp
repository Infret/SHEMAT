#include "angulardistributionmodel.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <QDebug>

angulardistributionmodel::angulardistributionmodel(QObject* parent)
    : QObject(parent)
{
    std::random_device rd;
    m_rng.seed(rd());
}

angulardistributionmodel::angulardistributionmodel(const reactionparameters& params, QObject* parent)
    : QObject(parent)
{
    std::random_device rd;
    m_rng.seed(rd());

    setparameters(params);
}

void angulardistributionmodel::setparameters(double e_lab, double z1, double a1, double z2, double a2)
{
    if (e_lab <= 0 || z1 <= 0 || a1 <= 0 || z2 <= 0 || a2 <= 0) {
        qDebug() << "ERROR: Invalid parameters for angular distribution model:";
        qDebug() << "e_lab:" << e_lab << "z1:" << z1 << "a1:" << a1 << "z2:" << z2 << "a2:" << a2;
        return;
    }

    m_e_lab = e_lab;
    m_z1 = z1;
    m_a1 = a1;
    m_z2 = z2;
    m_a2 = a2;

    Initialize_Calculation();
}

void angulardistributionmodel::setparameters(const reactionparameters& params)
{
    setparameters(params.e_lab, params.z1, params.a1, params.z2, params.a2);
}

void angulardistributionmodel::Initialize_Calculation()
{
    qDebug() << "=== initializing angulardistributionmodel ===";
    qDebug() << "input parameters: e_lab =" << m_e_lab << "MeV, z1 =" << m_z1 << "a1 =" << m_a1 << "z2 =" << m_z2 << "a2 =" << m_a2;

    if (m_a1 <= 0 || m_a2 <= 0 || m_e_lab <= 0) {
        qDebug() << "Invalid basic parameters";
        return;
    }

    // 1) приведённая масса в а.е.м.
    m_reducedmass = (m_a1 * m_a2) / (m_a1 + m_a2); // в единицах массы нуклона (u)
    qDebug() << "reduced mass: " << m_reducedmass << "u";

    // 2) радиус взаимодействия (эмпирически)
    // Можно улучшить: использовать деформации/включить +2 fm для поверхности при необходимости
    m_interactionradius = 1.2 * (std::pow(m_a1, 1.0/3.0) + std::pow(m_a2, 1.0/3.0));
    qDebug() << "interaction radius: " << m_interactionradius << "fm";

    // 3) энергия в системе центра масс
    m_e_cm = Calculate_CM_Energy();
    qDebug() << "cm energy: " << m_e_cm << "MeV";

    // 4) eta'
    m_etaprime = Calculate_Etaprime();
    qDebug() << "eta prime: " << m_etaprime;

    // 5) параметры модели (по статье: либо конкретные для Xe+Bi либо резервные приближ.)
    Calculate_Parameters(m_etaprime);
    qDebug() << "model parameters: beta =" << m_beta << "delta =" << m_delta << "d_theta =" << m_dtheta;
}

// ----------------- Основные функции -----------------

double angulardistributionmodel::Generate_Scattering_Angle()
{
    // 1) генерируем b по правилу вероятности ∝ b (равномерное распределение по сечению)
    impactparameter = Generate_Impact_Parameter();
    // 2) восстанавливаем L из b -- теперь L действительно используется в расчётах
    angularmomentum = Calculate_Angular_Momentum_From_Impact_Parameter(impactparameter);

    // Кулоновское отклонение вычисляем по L (точно: сначала считаем b=L/k внутри функции)
    double theta_c = Coulomb_Deflection(angularmomentum);

    // Ядерное отклонение (параметрическое)
    double theta_n = Nuclear_Deflection(angularmomentum);

    // Средний итоговый угол
    double theta_mean = theta_c - theta_n; // Θ(l)

    // Время взаимодействия (в "единицах", согласованных с моделью)
    double tau_int = Interaction_Time(angularmomentum, theta_c, theta_mean);

    // Дисперсия угла
    double sigma_sq = 2.0 * m_dtheta * tau_int;
    double sigma_theta = std::sqrt(std::max(sigma_sq, 1e-12));

    // Генерация гауссова шума
    std::normal_distribution<double> dist(theta_mean, sigma_theta);
    double generated_angle = dist(m_rng);
    generated_angle = foldAngleToZeroPi(generated_angle);


    if (!std::isfinite(generated_angle)) generated_angle = theta_mean;

    // Нормируем в диапазон [0, pi]
    // Угол задан в радианах; приводим к [0, pi]
    while (generated_angle < 0.0) generated_angle = -generated_angle;
    if (generated_angle > M_PI) generated_angle = std::fmod(generated_angle, M_PI);

    qDebug() << "generated: L =" << angularmomentum << "ħ, b =" << impactparameter << "fm";
    qDebug() << "scattering: theta_c =" << theta_c << "rad, theta_n =" << theta_n << "rad";
    qDebug() << "result: theta_mean =" << theta_mean << "rad, sigma =" << sigma_theta << "rad, final =" << generated_angle << "rad";

    return generated_angle;
}

// ----------------- Вспомогательные -----------------

double angulardistributionmodel::foldAngleToZeroPi(double a)
{
    // приводим к [0, 2π)
    double twoPi = 2.0 * M_PI;
    a = std::fmod(a, twoPi);
    if (a < 0.0) a += twoPi;
    // отражаем часть (π, 2π) обратно в (π,0)
    if (a > M_PI) a = twoPi - a;
    return a; // теперь в [0, π]
}

// Генерация impact parameter с распределением по площади: P(b) db ∝ b db
double angulardistributionmodel::Generate_Impact_Parameter()
{
    double b_max = Get_Maximum_Impact_Parameter();
    // Чтобы получить распределение пропорциональное b, генерируем u~U(0,1) и b = sqrt(u) * b_max
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double u = dist(m_rng);
    double b = std::sqrt(u) * b_max;
    return b;
}

double angulardistributionmodel::Get_Maximum_Impact_Parameter() const
{
//    double l_gr = Grazing_Angular_Momentum();
//    double b_gr = Calculate_Impact_Parameter_From_Angular_Momentum(l_gr);
    double fallback = m_interactionradius * 0.6; //ЭМПЕРИЧЕСКИЙ ПОДОБРАННО! :)
//    if (b_gr > 1e-12) return std::min(fallback, b_gr);
    return fallback;
}

// L (в ħ) из b: k = sqrt(2 μ(MeV) E_cm(MeV)) / (ħc) -> fm^-1, L = k * b
double angulardistributionmodel::Calculate_Angular_Momentum_From_Impact_Parameter(double b) const
{
    if (b <= 0.0) return 0.0;
    double reducedmass_mev = m_reducedmass * 931.494; // MeV/c^2
    if (reducedmass_mev <= 0.0 || m_e_cm <= 0.0) return 0.0;

    double k = std::sqrt(2.0 * reducedmass_mev * m_e_cm) / m_hbarc; // fm^-1
    if (k <= 0.0) return 0.0;
    double L = k * b;
    return L;
}

double angulardistributionmodel::Calculate_Impact_Parameter_From_Angular_Momentum(double L) const
{
    if (L <= 0.0) return 0.0;
    double reducedmass_mev = m_reducedmass * 931.494;
    if (reducedmass_mev <= 0.0 || m_e_cm <= 0.0) return 0.0;

    double k = std::sqrt(2.0 * reducedmass_mev * m_e_cm) / m_hbarc;
    if (k <= 0.0) return 0.0;
    return L / k;
}

// ----------------- Ключевые физические методы -----------------

// Coulomb deflection как функция L (внутри вычисляется b = L/k)
double angulardistributionmodel::Coulomb_Deflection(double L) const
{
    if (L <= 0.0) return 0.0;

    double b = Calculate_Impact_Parameter_From_Angular_Momentum(L);
    if (b <= 0.0) return 0.0;

    // Rutherford-like выражение: Θ_C = 2 arctan( Z1 Z2 e^2 / (2 E_cm b) )
    double argument = (m_z1 * m_z2 * m_e_square) / (2.0 * m_e_cm * b);
    return 2.0 * std::atan(argument);
}

// Coulomb deflection специально для grazing (используем L_gr непосредственно)
double angulardistributionmodel::Coulomb_Deflection_Grazing(double L) const
{
    return Coulomb_Deflection(L);
}

// Nuclear deflection Θ_N(l) по параметризации статьи
double angulardistributionmodel::Nuclear_Deflection(double L) const
{
    double l_gr = Grazing_Angular_Momentum();
    if (l_gr <= 1e-12) return 0.0;

    double theta_c_gr = Coulomb_Deflection_Grazing(l_gr);

    double ratio = L / l_gr;
    if (ratio < 0.0) ratio = 0.0;

    // Защита от невалидных степеней
    double powarg = (m_delta / m_beta);
    if (powarg <= 0.0) powarg = 1e-12;

    double result = m_beta * theta_c_gr * ratio * std::pow(powarg, ratio);
    return result;
}

// Interaction time: статья использует tau ~ (I_tot / lħ) * (Θ_C(l) - Θ(l))
// Здесь возвращаем tau в безразмерных единицах (согласованных с D_theta)
double angulardistributionmodel::Interaction_Time(double L, double theta_c, double theta_total) const
{
    if (L <= 1e-12) return 0.0;

    double A_red = (m_a1 * m_a2) / (m_a1 + m_a2);
    double I_tot = A_red * m_interactionradius * m_interactionradius; // условные ед.

    double delta = theta_c - theta_total;
    if (delta < 0.0) delta = 0.0;

    // Учтём ħ (в удобной для вашего кода форме) — используем m_hbarc как ~197.326 MeV·fm
    // Это просто привести масштаб τ к разумному значению; при желании заменить на более аккуратную единичную систему.
    const double hbarc = m_hbarc; // если у вас задан m_hbarc = 197.326
    double denom = std::max(L * hbarc, 1e-12);
    double tau = (I_tot / denom) * delta;

    // Защита от слишком больших tau: (опционально) лимитируем
    const double tau_max = 1e3;
    if (tau > tau_max) tau = tau_max;

    return tau;
}

double angulardistributionmodel::Grazing_Angular_Momentum() const
{
    double v_barrier = Interaction_Potential(m_interactionradius);
    double e_kin = m_e_cm - v_barrier;

    if (e_kin <= 0.0) {
        // если энергия ниже барьера — даём маленькое положительное значение, чтобы избежать NAN
        e_kin = 1e-6;
    }

    double A_red = (m_a1 * m_a2) / (m_a1 + m_a2);
    double l_gr = 0.22 * m_interactionradius * std::sqrt(std::max(0.0, A_red * e_kin));
    return l_gr;
}

double angulardistributionmodel::Interaction_Potential(double radius) const
{
    // Кулоновский потенциал
    double coulomb = (m_z1 * m_z2 * m_e_square) / radius;

    // Простая модель ядерного потенциала (Woods-Saxon-like)
    double nuclear_depth = -50.0; // MeV (можно подогнать)
    double diffuseness = 0.63;    // fm

    double nuclear = nuclear_depth / (1.0 + std::exp((radius - m_interactionradius) / diffuseness));
    return coulomb + nuclear;
}

// ----------------- Прочее: параметры и отладочный вывод -----------------
 
void angulardistributionmodel::Calculate_Parameters(double /*eta_prime*/)
{
    // Вычислим eta' уже есть в m_etaprime
    double eta = m_etaprime;

    // Используем эмпирические формулы из статьи (eq.10)
    double beta = 4.7 + 81.2 * std::exp( - (eta - 160.8) / 126.5 );
    double delta = 0.029 + 0.127 * std::exp( - (eta - 176.3) / 364.0 );
    double dtheta_deg2 = 21.0 - 0.0178 * eta; // это Dθ в (deg^2) по статье



    // Внутри кода мы работаем в радианах, поэтому конвертируем дисперсию:
    m_beta = beta;
    m_delta = delta;
    m_dtheta = dtheta_deg2;


//    m_beta = 5;
//    m_delta = 0.05;
//    m_dtheta = 11;
}

double angulardistributionmodel::Calculate_CM_Energy() const
{
    // E_cm = E_lab * A_target / (A_proj + A_target)
    return m_e_lab * (m_a2 / (m_a1 + m_a2));
}

double angulardistributionmodel::Calculate_Etaprime() const
{
    double reducedmass_mev = m_reducedmass * 931.494;

       // β = v/c
       double beta = std::sqrt(2.0 * m_e_cm / reducedmass_mev);
       if (beta <= 1e-12) return 0.0;

       // η = Z1 Z2 e² / (ħc * β)
       double eta = (m_z1 * m_z2 * m_e_square) / (m_hbarc * beta);


    return eta;
}

// Отладочный вывод (аналогично твоему)
void angulardistributionmodel::debugPrintAngularDistribution(int numSamples)
{
    qDebug() << "=== ANGULAR DISTRIBUTION DEBUG OUTPUT (fixed) ===";
    qDebug() << "System: Z1=" << m_z1 << "A1=" << m_a1 << "Z2=" << m_z2 << "A2=" << m_a2;
    qDebug() << "E_cm:" << m_e_cm << "MeV, E_lab:" << m_e_lab << "MeV";
    qDebug() << "Parameters: m_beta=" << m_beta << "m_delta=" << m_delta << "m_dtheta=" << m_dtheta;
    qDebug() << "Generating" << numSamples << "samples...";

    QVector<double> angles;
    QVector<double> Ls;
    angles.reserve(numSamples);
    Ls.reserve(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        double angle = Generate_Scattering_Angle();
        angles.append(angle);
        Ls.append(angularmomentum);
    }

    auto rad2deg = [](double rad) { return rad * 180.0 / M_PI; };
    auto deg2rad = [](double deg) { return deg * M_PI / 180.0; };

    // диапазон 10..160°
    const double degMin = 10.0;
    const double degMax = 160.0;

    // шаг 0.5°
    const double binStep = 0.5;
    const int binCount = static_cast<int>((degMax - degMin) / binStep) + 1;

    QVector<int> histogram(binCount, 0);

    // заполняем гистограмму
    for (double angle : angles) {
        double d = rad2deg(angle);

        if (d < degMin || d > degMax)
            continue;

        int bin = static_cast<int>((d - degMin) / binStep);
        bin = std::clamp(bin, 0, binCount - 1);
        histogram[bin]++;
    }

    // вычисление dσ/dΩ
    QVector<double> dsigma(binCount, 0.0);
    double maxval = 0.0;

    for (int i = 0; i < binCount; ++i) {

        double center_deg = degMin + i * binStep + 0.5 * binStep;
        double center_rad = deg2rad(center_deg);

        double solidAngle = 2.0 * M_PI * std::sin(center_rad) * deg2rad(binStep);

        double val = (numSamples > 0 && solidAngle > 1e-12)
                     ? (histogram[i] / (solidAngle * numSamples))
                     : 0.0;

        dsigma[i] = val;
        if (val > maxval) maxval = val;
    }
    // После заполнения dsigma[] сделаем простое скользящее среднее (окно 3)
    QVector<double> smoothed(dsigma.size(), 0.0);
    for (int i = 0; i < (int)dsigma.size(); ++i) {
        double sum = dsigma[i];
        int cnt = 1;
        if (i > 0) { sum += dsigma[i-1]; ++cnt; }
        if (i+1 < (int)dsigma.size()) { sum += dsigma[i+1]; ++cnt; }
        smoothed[i] = sum / cnt;
    }
    dsigma = smoothed;

    // вывод для Origin
    qDebug() << "\n--- DATA FOR ORIGIN (10°..160°, step=0.5°) ---";
    qDebug() << "Angle(deg)\tDensity";

    for (int i = 0; i < binCount; ++i) {
        double center_deg = degMin + i * binStep + 0.5 * binStep;
        double rel = (maxval > 0.0) ? dsigma[i] / maxval : 0.0;

        qDebug().noquote() << QString("%1 %2")
                              .arg(center_deg, 0, 'f', 2)
                              .arg(rel, 0, 'f', 6);
    }

    qDebug() << "--- END DATA ---";

    // статистика
    double minA = rad2deg(*std::min_element(angles.begin(), angles.end()));
    double maxA = rad2deg(*std::max_element(angles.begin(), angles.end()));
    double meanA = rad2deg(std::accumulate(angles.begin(), angles.end(), 0.0) / angles.size());

    double minL = *std::min_element(Ls.begin(), Ls.end());
    double maxL = *std::max_element(Ls.begin(), Ls.end());
    double meanL = std::accumulate(Ls.begin(), Ls.end(), 0.0) / Ls.size();

    qDebug() << "Angles - Min:" << minA << "deg, Max:" << maxA << "deg, Mean:" << meanA << "deg";
    qDebug() << "Angular momentum - Min:" << minL << "ħ, Max:" << maxL << "ħ, Mean:" << meanL << "ħ";
    qDebug() << "Grazing L:" << Grazing_Angular_Momentum() << "ħ";

    qDebug() << "=== END ===";
}

