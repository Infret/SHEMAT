// fieldelement.cpp — реализация локальных моделей полей
#include "fieldelement.h"

namespace ft {

// Общий выход прямых элементов: сдвиг рамки вдоль дизайн-оси
Frame FieldElement::exitFrame(const Frame& entry) const {
    Frame f = entry;
    f.origin = entry.origin + entry.ez * designLength();
    return f;
}

// ============================================================
// Квадруполь
// ============================================================
FieldQuadrupole::FieldQuadrupole(double G, double L, double ap, bool hard,
                                 const EngeCoeffs& ec, double lambda, const std::string& nm)
    : m_G(G), m_L(L), m_ap(ap), m_lambda(lambda), m_hard(hard), m_ec(ec), m_name(nm) {}

FieldFn FieldQuadrupole::makeField(const Frame& entry) const {
    return [this, entry](const Vec3& r) {
        const Vec3 rl = toLocal(entry, r);
        double S, dSdz;   // профиль силы вдоль z и его производная
        if (m_hard) {
            S = (rl.z >= 0.0 && rl.z <= m_L) ? 1.0 : 0.0;
            dSdz = 0.0;
        } else {
            double dIn, dOut;
            const double Din  = engeAndDeriv(-rl.z / m_lambda, m_ec, dIn);   // входной край
            const double Dout = engeAndDeriv((rl.z - m_L) / m_lambda, m_ec, dOut);
            S = Din * Dout;
            dSdz = (-dIn) * Dout + Din * dOut;      // d/dz: dIn берётся при t=-z/λ
            dSdz /= m_lambda;
        }
        FieldSample f;
        f.B = Vec3(m_G * rl.y * S, m_G * rl.x * S, m_G * rl.x * rl.y * dSdz);
        return f;
    };
}

bool FieldQuadrupole::apertureLocal(const Vec3& r) const {
    return (r.x * r.x + r.y * r.y) <= m_ap * m_ap;
}

// ============================================================
// Диполь
// ============================================================
FieldDipole::FieldDipole(double B0, double L, double R, double e1, double e2,
                         double gap, double xAp, double yAp, bool hard,
                         const EngeCoeffs& ec, double lambda, const std::string& nm)
    : m_B0(B0), m_L(L), m_R(R), m_e1(e1), m_e2(e2),
      m_xAp(xAp), m_yAp(yAp), m_lambda(lambda), m_hard(hard), m_ec(ec), m_name(nm)
{
    (void)gap; // зазор заложен в lambda (по умолчанию 2*gap — см. fieldengine)
}

FieldFn FieldDipole::makeField(const Frame& entry) const {
    // Геометрия в локальных координатах входной рамки
    const double th = m_L / m_R;                 // дизайн-угол
    const Vec3 exitPoint = Vec3(-(1.0 - std::cos(th)) * m_R, 0.0, std::sin(th) * m_R);
    // Нормали к граням (единичные, направленные наружу):
    //   входная: n1 =  ez*cos(e1) - ex*sin(e1)   -> s1 = -(rl·n1)
    //   выходная: n2 = -ez_d*cos(e2) + ex_d*sin(e2) -> s2 = (rl - exitPoint)·n2
    const Vec3 exd(std::cos(th), 0.0, std::sin(th));
    const Vec3 ezd(-std::sin(th), 0.0, std::cos(th));
    const Vec3 n2 = ezd * std::cos(m_e2) + exd * std::sin(m_e2);
    const double ds1dx = std::sin(m_e1), ds1dz = -std::cos(m_e1);
    const double ds2dx = n2.x, ds2dz = n2.z;

    return [this, entry, ds1dx, ds1dz, ds2dx, ds2dz, exitPoint, n2](const Vec3& r) {
        const Vec3 rl = toLocal(entry, r);
        const double s1 = rl.x * ds1dx + rl.z * ds1dz;          // = -(rl·n1)
        const Vec3 d = rl - exitPoint;
        const double s2 = d.dot(n2);

        double D1, D2, dD1, dD2;
        if (m_hard) {
            D1 = (s1 > 0.0) ? 0.0 : 1.0;   // внутри: s1 < 0
            D2 = (s2 > 0.0) ? 0.0 : 1.0;
            dD1 = dD2 = 0.0;
        } else {
            D1 = engeAndDeriv(s1 / m_lambda, m_ec, dD1);
            D2 = engeAndDeriv(s2 / m_lambda, m_ec, dD2);
            dD1 /= m_lambda; dD2 /= m_lambda;
        }

        const double By  = m_B0 * D1 * D2;
        const double dBydx = m_B0 * (dD1 * D2 * ds1dx + D1 * dD2 * ds2dx);
        const double dBydz = m_B0 * (dD1 * D2 * ds1dz + D1 * dD2 * ds2dz);

        FieldSample f;   // Максвелл (ведущий порядок по y): Bx = y*dBy/dx, Bz = y*dBy/dz
        f.B = Vec3(dBydx * rl.y, By, dBydz * rl.y);
        return f;
    };
}

bool FieldDipole::apertureLocal(const Vec3& r) const {
    return std::abs(r.x) <= m_xAp && std::abs(r.y) <= m_yAp;
}

Frame FieldDipole::exitFrame(const Frame& entry) const {
    // Изгиб к -ex: поворот базиса вокруг ey на угол -th, начало координат — по дуге
    const double th = m_L / m_R;
    Frame f;
    f.ex = rotateAround(entry.ex, entry.ey, -th);
    f.ey = entry.ey;
    f.ez = rotateAround(entry.ez, entry.ey, -th);
    f.origin = entry.origin + entry.ez * (std::sin(th) * m_R)
                             - entry.ex * ((1.0 - std::cos(th)) * m_R);
    return f;
}

// ============================================================
// Электростатический дефлектор
// ============================================================
FieldElectrostaticDeflector::FieldElectrostaticDeflector(double E0, double L, double R,
        double xAp, double yAp, bool hard, const EngeCoeffs& ec, double lambda,
        const std::string& nm)
    : m_E0(E0), m_L(L), m_R(R), m_xAp(xAp), m_yAp(yAp),
      m_lambda(lambda), m_hard(hard), m_ec(ec), m_name(nm) {}

FieldFn FieldElectrostaticDeflector::makeField(const Frame& entry) const {
    return [this, entry](const Vec3& r) {
        const Vec3 rl = toLocal(entry, r);
        double S, dSdz;
        if (m_hard) {
            S = (rl.z >= 0.0 && rl.z <= m_L) ? 1.0 : 0.0;
            dSdz = 0.0;
        } else {
            double dIn, dOut;
            const double Din  = engeAndDeriv(-rl.z / m_lambda, m_ec, dIn);
            const double Dout = engeAndDeriv((rl.z - m_L) / m_lambda, m_ec, dOut);
            S = Din * Dout;
            dSdz = (-dIn) * Dout + Din * dOut;
            dSdz /= m_lambda;
        }
        FieldSample f;   // Максвелл (ведущий порядок): Ez = x*dEx/dz
        f.E = Vec3(m_E0 * S, 0.0, m_E0 * rl.x * dSdz);
        return f;
    };
}

bool FieldElectrostaticDeflector::apertureLocal(const Vec3& r) const {
    return std::abs(r.x) <= m_xAp && std::abs(r.y) <= m_yAp;
}

Frame FieldElectrostaticDeflector::exitFrame(const Frame& entry) const {
    const double th = m_L / m_R;
    Frame f;
    f.ex = rotateAround(entry.ex, entry.ey, -th);
    f.ey = entry.ey;
    f.ez = rotateAround(entry.ez, entry.ey, -th);
    f.origin = entry.origin + entry.ez * (std::sin(th) * m_R)
                             - entry.ex * ((1.0 - std::cos(th)) * m_R);
    return f;
}

} // namespace ft
