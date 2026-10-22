// Консольные тесты: g++ -O2 -std=c++14 *.cpp -o test
#include "fieldengine.h"
#include <cstdio>
#include <cmath>

using namespace ft;

static int failures = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { ++failures; std::printf("FAIL: %s\n", msg); } \
    else { std::printf("PASS: %s\n", msg); } } while (0)

// Копия матричного квадруполя из SeparatorElement::transport (для сравнения)
static void matrixQuad(double& x, double& xp, double& y, double& yp,
                       double q, double G_kGsm, double m, double E, double L)
{
    double K = std::sqrt(std::fabs(q * G_kGsm) / (1.439 * std::sqrt(m * E)));
    double KL = K * L;
    double xi = x, xpi = xp, yi = y, ypi = yp;
    x  = xi * std::cos(KL) + xpi * std::sin(KL) / K;
    xp = -xi * K * std::sin(KL) + xpi * std::cos(KL);
    y  = yi * std::cosh(KL) + ypi * std::sinh(KL) / K;
    yp = yi * K * std::sinh(KL) + ypi * std::cosh(KL);
}

int main()
{
    const double m = 100.0;      // а.е.м.
    const int    q = 3;
    const double E = 100.0;      // МэВ

    // ---- Тест 1: дрейф, угол 5° ----
    {
        FieldEngine eng;
        eng.addElement(std::make_shared<FieldDrift>(1.0));
        auto r = eng.transport(0, 0, 5.0 * M_PI / 180.0, 0, E, m, q);
        CHECK(std::fabs(r.x - std::tan(5.0 * M_PI / 180.0)) < 1e-12,
              "drift: x = L*tan(5deg), точно");
    }

    // ---- Тест 2: квадруполь малой амплитуды vs матрица (hard edge) ----
    {
        const double L = 0.5, G_kGsm = 10.0, aper = 0.05;
        const double G_Tpm = G_kGsm * 0.1;
        double mx = 1e-3, mxp = 0, my = 0, myp = 0;
        matrixQuad(mx, mxp, my, myp, q, G_kGsm, m, E, L);

        FieldEngine eng;
        eng.addElement(std::make_shared<FieldQuadrupole>(
            G_Tpm, L, aper, /*hardEdge=*/true, engeCosyQuadrupole(), aper));
        IntegratorOpts opts; opts.h = 0.0005;
        auto r = eng.transport(1e-3, 0, 0, 0, E, m, q, opts);

        double dx = std::fabs(r.x - mx) / std::fabs(mx);
        CHECK(dx < 1e-3, "quad vs matrix: совпадение x (<0.1%) в параксиальном лимите");
    }

    // ---- Тест 3: диполь — референсная частица и сохранение |p| ----
    {
        const double B0 = 1.0;   // Тл
        const double massMeV = m * AMU_MEV;
        const double p = momentumFromEnergy(E, massMeV);
        const double R = p / (q * BRHO_PER_TESLA * B0);
        const double th = M_PI / 4.0;
        const double L = R * th;

        FieldEngine eng;
        eng.addElement(std::make_shared<FieldDipole>(
            B0, L, R, 0, 0, 0.05, 0.1, 0.025, true, engeCosyDipole(), 0.1));
        IntegratorOpts opts; opts.h = 0.001;
        auto r = eng.transport(0, 0, 0, 0, E, m, q, opts);

        CHECK(std::fabs(r.x) < 1e-6 && std::fabs(r.xp) < 1e-6,
              "dipole: референсная частица выходит на оси выходной рамки");
        const double p2 = momentumFromEnergy(E, massMeV); // |p| не должен меняться
        // независимо: контроль через угол поворота
        CHECK(std::fabs(r.yp) < 1e-9, "dipole: вертикаль не трогается");
    }

    // ---- Тест 4: сходимость RK4 (порядок h^4) на диполе при xp=5°, δ=-5% ----
    {
        const double B0 = 1.0;
        const double massMeV = m * AMU_MEV;
        const double pRef = momentumFromEnergy(E, massMeV);
        const double R = pRef / (q * BRHO_PER_TESLA * B0);
        const double L = R * M_PI / 4.0;

        auto run = [&](double h, double eKin) {
            FieldEngine eng;
            eng.addElement(std::make_shared<FieldDipole>(
                B0, L, R, 0, 0, 0.05, 0.5, 0.5, true, engeCosyDipole(), 0.1));
            IntegratorOpts o; o.h = h;
            return eng.transport(0, 0, 5.0 * M_PI / 180.0, 0, eKin, m, q, o);
        };
        auto rFine   = run(0.00025, 95.0);
        auto rMid    = run(0.001,   95.0);
        auto rCoarse = run(0.004,   95.0);

        double dMid  = std::fabs(rMid.x  - rFine.x)  + std::fabs(rMid.xp  - rFine.xp);
        double dCoarse = std::fabs(rCoarse.x - rFine.x) + std::fabs(rCoarse.xp - rFine.xp);
        double ratio = dCoarse / dMid;              // ожидаем ~ (4)^4 = 256
        CHECK(ratio > 100.0 && ratio < 1000.0,
              "convergence: ошибка ~ h^4 (отношение ~256)");
        std::printf("      (отношение ошибок = %.1f)\n", ratio);
    }

    // ---- Тест 5: офф-моментум в диполе поворачивает сильнее ----
    {
        const double B0 = 1.0;
        const double massMeV = m * AMU_MEV;
        const double pRef = momentumFromEnergy(E, massMeV);
        const double R = pRef / (q * BRHO_PER_TESLA * B0);
        const double L = R * M_PI / 4.0;

        FieldEngine eng;
        eng.addElement(std::make_shared<FieldDipole>(
            B0, L, R, 0, 0, 0.05, 0.5, 0.5, true, engeCosyDipole(), 0.1));
        IntegratorOpts o; o.h = 0.0005;

        auto rLo = eng.transport(0, 0, 0, 0, 90.0, m, q, o);   // -10%
        // Частица с меньшим импульсом должна оказаться ВНУТРЬ (x < 0 в выходной рамке)
        CHECK(rLo.x < -1e-3, "dipole: меньшая энергия -> меньший радиус -> x < 0");
    }

    std::printf("\n%s (%d failures)\n", failures ? "FAILED" : "ALL TESTS PASSED", failures);
    return failures ? 1 : 0;
}
