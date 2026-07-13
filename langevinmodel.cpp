#include "langevinmodel.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QDebug>
#include <random>
#include <algorithm>

// ---- Физические константы и единицы (используем MeV, fm, fm/c) ----
// hbar*c в MeV*fm
static constexpr double HBARC = 197.3269804; // MeV*fm
// 1 amu в MeV/c^2
static constexpr double AMU_MeV = 931.49410242; // MeV

LangevinModel::LangevinModel(QObject *parent)
    : QObject(parent), normalDist(0.0, 1.0)
{
    std::random_device rd;
    generator.seed(rd());
}

double LangevinModel::generateRandomAngle(int projectileA, int projectileZ,
                                         int targetA, int targetZ,
                                         double E_lab)
{
    double b = generateImpactParameter(projectileA, projectileZ, targetA, targetZ, E_lab);
    return calculateScatteringAngle(projectileA, projectileZ, targetA, targetZ, E_lab, b);
}

double LangevinModel::generateImpactParameter(int projectileA, int projectileZ,
                                            int targetA, int targetZ, double E_lab)
{
    double b_max = calculateMaxImpactParameter(projectileA, projectileZ, targetA, targetZ, E_lab);
    // Равномерный в площади: b = b_max * sqrt(u)
    double u = QRandomGenerator::global()->generateDouble();
    return b_max * std::sqrt(std::max(0.0, u));
}

double LangevinModel::calculateCMEnergy(int projectileA, int targetA, double E_lab)
{
    // E_cm = E_lab * A_target / (A_proj + A_target)
    return E_lab * static_cast<double>(targetA) / (static_cast<double>(projectileA) + static_cast<double>(targetA));
}

double LangevinModel::calculateScatteringAngle(int projectileA, int projectileZ,
                                             int targetA, int targetZ,
                                             double E_lab, double impactParameter)
{
    double E_cm = calculateCMEnergy(projectileA, targetA, E_lab);
    DynamicsResult result = solveLangevinEquations(projectileA, projectileZ,
                                                  targetA, targetZ,
                                                  E_cm, impactParameter);
    return result.angle;
}

QVector<double> LangevinModel::simulateAngularDistribution(int projectileA, int projectileZ,
                                                         int targetA, int targetZ,
                                                         double E_lab, int nTrajectories)
{
    QVector<double> angles;
    angles.reserve(nTrajectories);
    for (int i = 0; i < nTrajectories; ++i) {
        double b = generateImpactParameter(projectileA, projectileZ, targetA, targetZ, E_lab);
        double angle = calculateScatteringAngle(projectileA, projectileZ,
                                              targetA, targetZ, E_lab, b);
        angles.append(angle);
    }
    return angles;
}

double LangevinModel::calculateGrazingImpactParameter(int projectileA, int projectileZ,
                                                    int targetA, int targetZ, double E_lab)
{
    double E_cm = calculateCMEnergy(projectileA, targetA, E_lab);
    double V_coulomb = calculateCoulombBarrier(projectileZ, targetZ, projectileA, targetA);
    double R_contact = calculateContactDistance(projectileA, targetA);

    if (E_cm <= V_coulomb) return 0.0;
    double factor = std::sqrt(std::max(0.0, 1.0 - V_coulomb / E_cm));
    return R_contact * factor;
}

double LangevinModel::calculateMaxImpactParameter(int projectileA, int projectileZ,
                                                int targetA, int targetZ, double E_lab)
{
    double b_grazing = calculateGrazingImpactParameter(projectileA, projectileZ,
                                                      targetA, targetZ, E_lab);
    // Добавляем запас (fm). Можно уменьшить/увеличить при подстройке.
    return b_grazing + 4.0;
}

double LangevinModel::calculateCrossSection(double b_max)
{
    // π b_max^2 (fm^2)
    return M_PI * b_max * b_max;
}

double LangevinModel::calculateGrazingAngle(int projectileA, int projectileZ,
                                          int targetA, int targetZ, double E_lab)
{
    double E_cm = calculateCMEnergy(projectileA, targetA, E_lab);
    double R_contact = calculateContactDistance(projectileA, targetA);
    double V_coulomb = calculateCoulombBarrier(projectileZ, targetZ, projectileA, targetA);

    if (E_cm <= V_coulomb) return M_PI;
    return 2.0 * std::atan(V_coulomb / (E_cm - V_coulomb));
}

double LangevinModel::calculateReducedMass(int A1, int A2)
{
    // Возвращаем приведённую массу в MeV (т.е. энерговек в MeV, эквивалент mass*c^2)
    double m1 = static_cast<double>(A1) * AMU_MeV;
    double m2 = static_cast<double>(A2) * AMU_MeV;
    return (m1 * m2) / (m1 + m2); // MeV (в 'энергетическом' представлении)
}

double LangevinModel::calculateCoulombBarrier(int Z1, int Z2, int A1, int A2)
{
    // V_C = Z1*Z2*1.44 / (R1 + R2) [MeV], R ~ 1.2*A^(1/3) fm
    double R1 = 1.2 * std::pow(static_cast<double>(A1), 1.0/3.0);
    double R2 = 1.2 * std::pow(static_cast<double>(A2), 1.0/3.0);
    return (static_cast<double>(Z1) * static_cast<double>(Z2) * 1.44) / (R1 + R2);
}

double LangevinModel::calculateContactDistance(int A1, int A2)
{
    double R1 = 1.2 * std::pow(static_cast<double>(A1), 1.0/3.0);
    double R2 = 1.2 * std::pow(static_cast<double>(A2), 1.0/3.0);
    return R1 + R2; // fm
}

double LangevinModel::woodSaxonFormFactor(double r, double R_contact)
{
    return 1.0 / (1.0 + std::exp((r - R_contact - DELTA_RF) / A_F));
}

double LangevinModel::oneBodyFriction(double r, double R_contact)
{
    if (r > R_contact + 2.0) return 0.0;
    // Возвращаем величину трения в условных единицах (MeV/fm) — настраивается
    return GAMMA_R0 * 0.1;
}

double LangevinModel::phenomenologicalFriction(double r, double R_contact)
{
    double F_r = woodSaxonFormFactor(r, R_contact);
    return GAMMA_R0 * F_r;
}

double LangevinModel::potentialEnergy(double r_fm, int Z1, int Z2, int A1, int A2)
{
    // r_fm — расстояние в fm. Возвращаемое значение в MeV.
    double R_contact = calculateContactDistance(A1, A2);
    // Кулоновский член (простая форма)
    double V_coulomb = (static_cast<double>(Z1) * static_cast<double>(Z2) * 1.44) / std::max(0.0001, r_fm);
    // Простейший ядерный потенциал (Woods-Saxon-like)
    double V0 = 50.0; // MeV, глубина потенциала — эмпирическая
    double a = 0.63;  // fm
    double V_nuclear = -V0 / (1.0 + std::exp((r_fm - R_contact) / a));
    return V_coulomb + V_nuclear;
}

// -------- Основная функция решения уравнений Ланжевена (обновлённая) -----------
LangevinModel::DynamicsResult LangevinModel::solveLangevinEquations(int projectileA, int projectileZ,
                                                                   int targetA, int targetZ,
                                                                   double E_cm, double b_fm)
{
    DynamicsResult result;
    result.angle = M_PI; // по умолчанию (на случай ошибок)
    result.finalEnergy = 0.0;
    result.reactionTime = 0.0;

    // --- Единицы: MeV, fm, время в fm/c ---
    // reduced mass (MeV)
    double mu = calculateReducedMass(projectileA, targetA); // MeV

    // Начальное расстояние (стандарт в статье ~50 fm)
    double r = 50.0; // fm
    double R_contact = calculateContactDistance(projectileA, targetA); // fm

    // Потенциал в начальной точке (MeV)
    double V0 = potentialEnergy(r, projectileZ, targetZ, projectileA, targetA);
    double K_rad = E_cm - V0; // MeV (радиальная доступная энергия)
    if (K_rad <= 0.0) {
        // недостаточно энергии для продвижения к контактной конфигурации
        result.angle = M_PI;
        result.finalEnergy = 0.0;
        result.reactionTime = 0.0;
        return result;
    }

    // --- Angular momentum (в натуральных единицах): L = b * p_total (MeV * fm)
    // Сначала общая кинетическая энергия -> p_total
    double p_total = std::sqrt(std::max(0.0, 2.0 * mu * K_rad)); // MeV (в "momentum units" MeV)
    // Учет прицельного параметра (b в fm)
    double L = b_fm * p_total; // MeV * fm

    // Начальный радиальный импульс (MeV)
    // p_r^2 = p_total^2 - (L/r)^2
    double L_over_r = L / r; // MeV
    double p_r = 0.0;
    double temp = p_total * p_total - L_over_r * L_over_r;
    if (temp > 0.0) p_r = std::sqrt(temp);
    else p_r = 0.0;

    // Угол (радианы)
    double theta = 0.0;

    // Параметры интегрирования
    double dt = 0.05; // fm/c — шаг времени (уменьшите если нестабильность сохраняется)
    const int max_steps = 1000000; // запас
    double t_total = 0.0;

    // Трение и температура (надо подбирать/перевести из статьи; здесь - рабочие примерные значения)
    // ВАЖНО: значения γ в статье приведены в других единицах; ниже — начальные "безопасные" величины в MeV/fm
    double gamma_rad = 0.01;    // [MeV/fm] — радиальное демпфирование (влияет в уравнении для dp/dt)
    double gamma_theta = 0.001; // демпфирование углового момента
    // Эффективная температура (MeV) — можно связать с возбуждением: T = sqrt(E*/a)
    double T_eff = 0.5; // MeV (примерное небольшое значение; при больших энергиях увеличить)

    // Предкомпьютируем шумовую константу для Δp
    // Для дискретного шага Δp_random = sqrt(2 * gamma * mu * T) * sqrt(dt)
    auto rand_normal = [&]( ) {
        return normalDist(generator);
    };

    // Вспомогательные лямбда: V(r) и dV/dr (MeV и MeV/fm)
    auto V_mev = [&](double r_fm_local)->double {
        return potentialEnergy(r_fm_local, projectileZ, targetZ, projectileA, targetA);
    };
    auto dVdr = [&](double r_fm_local)->double {
        double h = 0.01; // fm (малый шаг для численной производной)
        double Vp = V_mev(r_fm_local + h);
        double Vm = V_mev(std::max(1e-6, r_fm_local - h));
        return (Vp - Vm) / (2.0 * h); // MeV/fm
    };

    // Интегрирование (Euler–Maruyama)
    for (int step = 0; step < max_steps; ++step) {
        // защита от нереалистичного r
        if (!std::isfinite(r) || !std::isfinite(p_r) || !std::isfinite(theta) || !std::isfinite(L)) {
            qDebug() << "Numerical instability in Langevin solve: step=" << step
                     << " r=" << r << " p_r=" << p_r << " theta=" << theta << " L=" << L;
            result.angle = M_PI;
            result.reactionTime = t_total;
            result.finalEnergy = 0.0;
            return result;
        }

        // Силы:
        double dV = dVdr(r); // MeV/fm (force radially)
        // центробежный вклад в виде силы (MeV/fm): L^2/(mu * r^3)
        double centrifugal = (L * L) / (mu * r * r * r); // MeV/fm

        // dr/dt = p_r / mu  (fm / (fm/c) => в единицах c=1)
        double dr_dt = p_r / mu;

        // deterministic dp_r/dt = -dV + centrifugal - gamma * dr_dt
        double dp_r_det = -dV + centrifugal - gamma_rad * dr_dt; // MeV/fm

        // стохастический вклад в Δp_r (MeV)
        double sigma_p = std::sqrt(2.0 * gamma_rad * mu * T_eff); // MeV / sqrt(fm/c)
        double delta_p_random = sigma_p * std::sqrt(dt) * rand_normal(); // MeV

        // обновление p_r (Euler–Maruyama)
        p_r += dp_r_det * dt + delta_p_random;

        // обновление радиуса
        r += dr_dt * dt;

        // уголовая скорость dtheta/dt = L / (mu * r^2)
        double dtheta_dt = 0.0;
        if (r > 1e-8) dtheta_dt = L / (mu * r * r);

        // демпфирование углового момента (простая модель)
        double dL_dt = -gamma_theta * dtheta_dt;

        // обновление L и theta
        L += dL_dt * dt;
        theta += dtheta_dt * dt;

        t_total += dt;

        // стоп-условия:
        if (r > 4.0 * R_contact && step > 50) {
            // продукты разошлись — считаем угол рассеяния
            break;
        }
        if (r < 0.5 * R_contact) {
            // поглощение/слияние -> считаем угол "задним" (по соглашению 180°)
            theta = M_PI;
            break;
        }
    }

    // финальные значения
    // приводим theta к диапазону [0, pi]
    if (!std::isfinite(theta)) theta = M_PI;
    if (theta < 0.0) theta = std::abs(theta);
    if (theta > M_PI) theta = std::fmod(theta, M_PI);

    result.angle = std::abs(theta); // radians
    // Оценка финальной энергии: E_cm - V(r_final) (примерная)
    double V_final = V_mev(r);
    result.finalEnergy = E_cm - V_final;
    result.reactionTime = t_total;

    return result;
}
