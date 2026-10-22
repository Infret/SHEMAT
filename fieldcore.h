// fieldcore.h — состояние частицы, уравнения движения, Enge, рамки (header-only)
#pragma once
#include "vec3.h"
#include <cmath>

namespace ft {

// ============================================================
// Физические константы (система единиц: м, Тл, В/м, МэВ/c)
// ============================================================
constexpr double BRHO_PER_TESLA = 299.792458;  // p[МэВ/c] = BRHO * z * Bρ[Тл·м]
constexpr double AMU_MEV        = 931.49410242; // энергия покоя 1 а.е.м., МэВ

// Значение поля в точке: B в Тл, E в В/м
struct FieldSample {
    Vec3 B, E;
    FieldSample() : B(0, 0, 0), E(0, 0, 0) {}
};

// Состояние частицы в ГЛОБАЛЬНОЙ системе координат
struct TrackState {
    Vec3   r;        // позиция, м
    Vec3   p;        // вектор импульса, МэВ/c (полный, трёхкомпонентный)
    double massMeV;  // энергия покоя mc^2, МэВ
    int    charge;   // зарядовое состояние z (в единицах e)
};

inline double betaGamma(const TrackState& s) { return s.p.length() / s.massMeV; }
inline double betaOf(const TrackState& s) {
    const double bg = betaGamma(s);
    return bg / std::sqrt(1.0 + bg * bg);
}

// Импульс из кинетической энергии (релятивистски точно)
inline double momentumFromEnergy(double eKinMeV, double massMeV) {
    return std::sqrt(eKinMeV * (eKinMeV + 2.0 * massMeV));
}

// ============================================================
// Правые части уравнений движения (параметризация по длине пути s)
//
//   dr/ds = u = p/|p|
//   dp/ds = z * 299.792458 * (u x B)   — магнит: поворачивает, |p| не меняет
//         + z * E / (beta * 1e6)       — электрическое: меняет |p|
//
// Обе константы — пересчёт единиц: магнитная из p = 299.79*z*Bρ,
// электрическая из dp/ds = qE/v.
// ============================================================
inline void equationsOfMotion(const TrackState& st, const FieldSample& f,
                              Vec3& drds, Vec3& dpds)
{
    const double pmag = st.p.length();
    const Vec3 u = st.p / pmag;
    drds = u;
    dpds = u.cross(f.B) * (BRHO_PER_TESLA * st.charge)
         + f.E * (static_cast<double>(st.charge) / (betaOf(st) * 1.0e6));
}

// ============================================================
// Функция Энге — краевое поле
//   D(s) = 1 / (1 + exp(P(s/λ))),  P(t) = a1 + a2 t + a3 t^2 + ... + a6 t^5
// s — расстояние от эффективной границы поля, положительное СНАРУЖИ.
// Глубоко внутри (s -> -inf) D -> 1; снаружи -> 0.
// ============================================================
struct EngeCoeffs {
    double a[6];
    int n = 6;
};

// Значение D(t), t = s/λ
inline double enge(double t, const EngeCoeffs& c) {
    double P = c.a[0];
    double tp = t;
    for (int i = 1; i < c.n; ++i) { P += c.a[i] * tp; tp *= t; }
    if (P > 40.0)  return 0.0;          // защита от переполнения exp
    if (P < -40.0) return 1.0;
    return 1.0 / (1.0 + std::exp(P));
}

// Значение и производная dD/dt (аналитическая)
inline double engeAndDeriv(double t, const EngeCoeffs& c, double& dDdt) {
    double P = c.a[0], dPdt = 0.0;
    double tp = t;
    for (int i = 1; i < c.n; ++i) {
        dPdt += i * c.a[i] * tp;        // производная многочлена
        P    += c.a[i] * tp;
        tp   *= t;
    }
    if (P > 40.0)  { dDdt = 0.0; return 0.0; }
    if (P < -40.0) { dDdt = 0.0; return 1.0; }
    const double D = 1.0 / (1.0 + std::exp(P));
    dDdt = -D * (1.0 - D) * dPdt;
    return D;
}

// Дефолты COSY INFINITY (Berz et al., PRST-AB 3, 124001 (2000)).
// Рекомендуется сверить с GICOSYFF.DAT / заменить фитом по реальному магниту.
inline EngeCoeffs engeCosyQuadrupole() {
    EngeCoeffs c; // масштаб λ = апертура (радиус) квадруполя
    c.a[0] = 0.296471; c.a[1] = 4.533219; c.a[2] = -2.270982;
    c.a[3] = 1.068627; c.a[4] = -0.036391; c.a[5] = 0.022261;
    return c;
}
inline EngeCoeffs engeCosyDipole() {
    EngeCoeffs c; // масштаб λ = 2*зазор
    c.a[0] = 0.251417; c.a[1] = 1.368965; c.a[2] = -0.042608;
    c.a[3] = 0.681353; c.a[4] = 0.554711; c.a[5] = -2.33769;
    return c;
}

// ============================================================
// Рамка элемента: начало + ортонормированный базис
//   ez — дизайн-ось (направление полёта центральной частицы)
//   ex — в плоскости изгиба (от центра кривизны при диполе)
//   ey — вертикаль
// ============================================================
struct Frame {
    Vec3 origin;
    Vec3 ex, ey, ez;
};

// Рамка по началу и направлению оси z (ex выбирается ближайшим к hintX)
inline Frame makeFrame(const Vec3& origin, const Vec3& dirZ, const Vec3& hintX) {
    Frame f; f.origin = origin;
    f.ez = dirZ.normalized();
    f.ey = f.ez.cross(hintX).normalized();
    f.ex = f.ey.cross(f.ez).normalized();
    return f;
}

// Поворот вектора вокруг единичной оси (формула Родригеса)
inline Vec3 rotateAround(const Vec3& v, const Vec3& axisUnit, double angle) {
    const double c = std::cos(angle), s = std::sin(angle);
    return v * c + axisUnit.cross(v) * s + axisUnit * (axisUnit.dot(v) * (1.0 - c));
}

inline Vec3 toLocal(const Frame& f, const Vec3& r) {
    const Vec3 d = r - f.origin;
    return Vec3(d.dot(f.ex), d.dot(f.ey), d.dot(f.ez));
}
inline Vec3 toGlobalDir(const Frame& f, const Vec3& v) {
    return f.ex * v.x + f.ey * v.y + f.ez * v.z;
}
inline Vec3 toGlobalPoint(const Frame& f, const Vec3& v) {
    return f.origin + toGlobalDir(f, v);
}

}

// namespace ft
