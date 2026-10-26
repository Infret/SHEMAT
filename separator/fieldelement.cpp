// fieldelement.cpp — реализация локальных моделей полей
#include "fieldelement.h"
#include <cmath>

namespace ft {

Frame FieldElement::exitFrame(const Frame& entry) const {
    Frame f = entry;
    f.origin = entry.origin + entry.ez * designLength();
    return f;
}

// Общая рамка выхода для изгибающих элементов (считается в рамке «изгиб к -x»)
static Frame bentExitFrame(const Frame& e, double R, double th) {
    Frame f;
    f.ex = rotateAround(e.ex, e.ey, -th);
    f.ey = e.ey;
    f.ez = rotateAround(e.ez, e.ey, -th);
    f.origin = e.origin + e.ez * (std::sin(th) * R) - e.ex * ((1.0 - std::cos(th)) * R);
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
        FieldSample f;
        // компоненты поля — в ЛОКАЛЬНОЙ рамке, переводим в глобальные
        f.B = toGlobalDir(entry, Vec3(m_G * rl.y * S, m_G * rl.x * S, m_G * rl.x * rl.y * dSdz));
        return f;
    };
}

bool FieldQuadrupole::apertureLocal(const Vec3& r) const {
    return (r.x * r.x + r.y * r.y) <= m_ap * m_ap;
}

// ============================================================
// Диполь
// ============================================================
FieldDipole::FieldDipole(double B0, double R, double angle, int bendSign,
                         double e1, double e2, double xMin, double xMax, double yHalf,
                         bool hard, const EngeCoeffs& ec, double lambda, const std::string& nm)
    : m_B0(std::fabs(B0)), m_R(std::fabs(R)), m_L(std::fabs(R) * angle), m_e1(e1), m_e2(e2),
      m_xMin(xMin), m_xMax(xMax), m_yAp(yHalf), m_lambda(lambda),
      m_flip(bendSign < 0), m_hard(hard), m_ec(ec), m_name(nm) {}

FieldFn FieldDipole::makeField(const Frame& entry) const {
    const Frame fr = eff(entry);
    const double th = m_L / m_R;
    const Vec3 exitPoint(-(1.0 - std::cos(th)) * m_R, 0.0, std::sin(th) * m_R);
    const Vec3 exd(std::cos(th), 0.0, std::sin(th));
    const Vec3 ezd(-std::sin(th), 0.0, std::cos(th));
    // внешняя нормаль выходной грани: s2 > 0 СНАРУЖИ магнита
    const Vec3 n2 = ezd * std::cos(m_e2) + exd * std::sin(m_e2);
    const double ds1dx = std::sin(m_e1), ds1dz = -std::cos(m_e1);
    const double ds2dx = n2.x, ds2dz = n2.z;

    return [this, fr, ds1dx, ds1dz, ds2dx, ds2dz, exitPoint, n2](const Vec3& r) {
        const Vec3 rl = toLocal(fr, r);
        const double s1 = rl.x * ds1dx + rl.z * ds1dz;
        const double s2 = (rl - exitPoint).dot(n2);

        double D1, D2, dD1, dD2;
        if (m_hard) {
            D1 = (s1 > 0.0) ? 0.0 : 1.0;
            D2 = (s2 > 0.0) ? 0.0 : 1.0;
            dD1 = dD2 = 0.0;
        } else {
            D1 = engeAndDeriv(s1 / m_lambda, m_ec, dD1);
            D2 = engeAndDeriv(s2 / m_lambda, m_ec, dD2);
            dD1 /= m_lambda; dD2 /= m_lambda;
        }
        const double By    = m_B0 * D1 * D2;
        const double dBydx = m_B0 * (dD1 * D2 * ds1dx + D1 * dD2 * ds2dx);
        const double dBydz = m_B0 * (dD1 * D2 * ds1dz + D1 * dD2 * ds2dz);

        FieldSample f;
        f.B = toGlobalDir(fr, Vec3(dBydx * rl.y, By, dBydz * rl.y));
        return f;
    };
}

bool FieldDipole::apertureLocal(const Vec3& r) const {
    return r.x >= m_xMin && r.x <= m_xMax && std::fabs(r.y) <= m_yAp;
}

// Центр кривизны в (-R,0,0) рамки «изгиб к -x». Возвращает x относительно дизайн-орбиты
// в ориентации ВХОДНОЙ рамки (для изгиба к +x знак меняется).
Vec3 FieldDipole::designLocal(const Vec3& rl) const {
    const double xp = m_flip ? -rl.x : rl.x;
    const double rho = std::sqrt((xp + m_R) * (xp + m_R) + rl.z * rl.z);
    const double dev = rho - m_R;
    return Vec3(m_flip ? -dev : dev, rl.y, rl.z);
}

Frame FieldDipole::exitFrame(const Frame& entry) const {
    const Frame f = bentExitFrame(eff(entry), m_R, m_L / m_R);
    return m_flip ? flipFrame(f) : f;
}

Frame FieldDipole::entryFace(const Frame& entry) const {
    const Frame e = eff(entry);
    Frame f;
    f.origin = e.origin;
    f.ez = (e.ex * (-std::sin(m_e1)) + e.ez * std::cos(m_e1)).normalized();   // внутрь магнита
    f.ey = e.ey;
    f.ex = f.ey.cross(f.ez);
    return f;
}

Frame FieldDipole::exitFace(const Frame& entry) const {
    const Frame e = eff(entry);
    const double th = m_L / m_R;
    const Vec3 exitPoint(-(1.0 - std::cos(th)) * m_R, 0.0, std::sin(th) * m_R);
    const Vec3 exd(std::cos(th), 0.0, std::sin(th));
    const Vec3 ezd(-std::sin(th), 0.0, std::cos(th));
    const Vec3 n2 = ezd * std::cos(m_e2) + exd * std::sin(m_e2);           // наружу
    Frame f;
    f.origin = toGlobalPoint(e, exitPoint);
    f.ez = toGlobalDir(e, n2).normalized();
    f.ey = e.ey;
    f.ex = f.ey.cross(f.ez);
    return f;
}

// Вертикальная фокусировка на наклонной грани (жёсткий край): dp_y = -q c B tan(e) y
void FieldDipole::faceKick(TrackState& st, const Frame& entry, bool entrance) const {
    if (!m_hard) return;   // для мягких краёв это даёт Bz (Максвелл)
    const double e = entrance ? m_e1 : m_e2;
    if (e == 0.0) return;
    const double y = (st.r - entry.origin).dot(entry.ey);
    st.p += entry.ey * (-BRHO_PER_TESLA * st.charge * m_B0 * std::tan(e) * y);
}

// ============================================================
// Электростатический дефлектор
// ============================================================
FieldElectrostaticDeflector::FieldElectrostaticDeflector(double E0, double R, double angle, int bendSign,
        double xAp, double yAp, bool hard, const EngeCoeffs& ec, double lambda, const std::string& nm)
    : m_E0(std::fabs(E0)), m_R(std::fabs(R)), m_L(std::fabs(R) * angle), m_xAp(xAp), m_yAp(yAp),
      m_lambda(lambda), m_flip(bendSign < 0), m_hard(hard), m_ec(ec), m_name(nm) {}

FieldFn FieldElectrostaticDeflector::makeField(const Frame& entry) const {
    const Frame fr = eff(entry);
    return [this, fr](const Vec3& r) {
        const Vec3 rl = toLocal(fr, r);
        FieldSample f;
        if (m_hard) {
            // E = E0 * (R/rho), радиально К ЦЕНТРУ кривизны (-R,0,0). Границы — радиальные плоскости.
            const double X = rl.x + m_R;
            const double rho2 = X * X + rl.z * rl.z;
            const double phi = std::atan2(rl.z, X);
            if (rho2 > 1e-12 && phi >= 0.0 && phi <= m_L / m_R) {
                const double k = -m_E0 * m_R / rho2;
                f.E = toGlobalDir(fr, Vec3(k * X, 0.0, k * rl.z));
            }
            return f;
        }
        // мягкие края: упрощённая модель (однородное поле к центру)
        double dIn, dOut;
        const double Din  = engeAndDeriv(-rl.z / m_lambda, m_ec, dIn);
        const double Dout = engeAndDeriv((rl.z - m_L) / m_lambda, m_ec, dOut);
        const double S = Din * Dout;
        double dSdz = (-dIn) * Dout + Din * dOut;
        dSdz /= m_lambda;
        f.E = toGlobalDir(fr, Vec3(-m_E0 * S, 0.0, -m_E0 * rl.x * dSdz));
        return f;
    };
}

bool FieldElectrostaticDeflector::apertureLocal(const Vec3& r) const {
    return std::fabs(r.x) <= m_xAp && std::fabs(r.y) <= m_yAp;
}

Vec3 FieldElectrostaticDeflector::designLocal(const Vec3& rl) const {
    const double xp = m_flip ? -rl.x : rl.x;
    const double rho = std::sqrt((xp + m_R) * (xp + m_R) + rl.z * rl.z);
    const double dev = rho - m_R;
    return Vec3(m_flip ? -dev : dev, rl.y, rl.z);
}

Frame FieldElectrostaticDeflector::exitFrame(const Frame& entry) const {
    const Frame f = bentExitFrame(eff(entry), m_R, m_L / m_R);
    return m_flip ? flipFrame(f) : f;
}

} // namespace ft
