#include "separatorelement.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

namespace {
const double AMU_MEV_  = 931.49410242;   // МэВ
const double BRHO_PER_T = 299.792458;    // p[МэВ/c] = 299.79 * q * Bρ[Тл·м]

// импульс, МэВ/c, и полная энергия, МэВ
inline void momentumOf(const Particle& p, double& pMeV, double& eTot)
{
    const double mc2 = p.mass * AMU_MEV_;
    pMeV = std::sqrt(p.energy * (p.energy + 2.0 * mc2));
    eTot = p.energy + mc2;
}
}

SeparatorElement::SeparatorElement(ElementType type)
    : m_type(type), m_len(0), m_aperture(1e6), m_gradient(0), m_gap(0),
      m_voltage(0), m_yAperture(0.15), m_B(0), m_R(1.0), m_angle(0),
      m_e1(0), m_e2(0), m_xmin(-0.1), m_xmax(0.1), m_xAperture(0.1),
      m_yApertureDip(0.05), m_radius(0), m_correction(0.05), m_fringe(0.2),
      m_name("")
{}

void SeparatorElement::setParameters(double p1, double p2, double p3, double p4,
                                     double p5, double p6, double p7, double p8)
{
    const double deg = M_PI / 180.0;
    switch (m_type) {
    case ElementType::Drift:
        m_len = p1;
        m_aperture = p2 > 0 ? p2 : 1e6;
        break;
    case ElementType::Quadrupole:
        m_len = p1;
        m_gradient = p2;
        m_aperture = p3;
        m_fringe = (p4 > 0) ? p4 : 0.2;
        break;
    case ElementType::Dipole: {                 // B_Тл, R, angle, gap, e1, e2, xmin, xmax
        m_B = std::fabs(p1);
        m_R = std::fabs(p2);
        m_gap = p4;
        m_e1 = p5 * deg; m_e2 = p6 * deg;
        m_xmin = p7; m_xmax = p8;
        m_yApertureDip = m_gap / 2.0;
        BendGeom g;
        g.R0 = m_R; g.theta0 = std::fabs(p3) * deg;
        g.bend = (p1 >= 0.0) ? +1 : -1;         // B>0 — влево (к -x)
        g.e1 = m_e1; g.e2 = m_e2;
        g.xMin = p7; g.xMax = p8; g.yHalf = m_gap / 2.0;
        g.kicks = true;
        m_geom = g; m_geomSet = true;
        m_len = g.R0 * g.theta0;
        m_angle = g.theta0;
        break;
    }
    case ElementType::ElectrostaticDeflector: { // U_кВ, R, angle, gap, xmin, xmax
        m_voltage = std::fabs(p1);
        m_R = std::fabs(p2);
        m_gap = p4;
        m_xmin = p5; m_xmax = p6;
        m_yAperture = 0.15;
        BendGeom g;
        g.R0 = m_R; g.theta0 = std::fabs(p3) * deg;
        g.bend = (p1 < 0.0) ? +1 : -1;          // U<0 — влево (к -x)
        g.e1 = 0.0; g.e2 = 0.0;
        g.xMin = p5; g.xMax = p6; g.yHalf = m_yAperture;
        g.kicks = false;
        m_geom = g; m_geomSet = true;
        m_len = g.R0 * g.theta0;
        m_angle = g.theta0;
        break;
    }
    default:
        break;
    }
}

void SeparatorElement::transport(Particle& p) const
{
    if (!p.alive) return;

    switch (m_type) {
    case ElementType::Drift:
        p.x += m_len * std::tan(p.xp);
        p.y += m_len * std::tan(p.yp);
        break;

    case ElementType::Quadrupole: {
        const double fringe_factor = 0.2;
        double fringe_strength = 0.0;

        double K = std::sqrt(std::abs(p.charge * m_gradient) / (1.439 * std::sqrt(p.mass * p.energy)));
        double KL = K * m_len;
        fringe_strength = K * m_len * fringe_factor / 2.0;

        if (m_gradient > 0) {
            p.xp -= fringe_strength * p.x;
            p.yp += fringe_strength * p.y;
        } else {
            p.xp += fringe_strength * p.x;
            p.yp -= fringe_strength * p.y;
        }

        if (m_gradient > 0) {
            double x_in = p.x, xp_in = p.xp;
            p.x = x_in * std::cos(KL) + xp_in * std::sin(KL) / K;
            p.xp = -x_in * K * std::sin(KL) + xp_in * std::cos(KL);
            double y_in = p.y, yp_in = p.yp;
            p.y = y_in * std::cosh(KL) + yp_in * std::sinh(KL) / K;
            p.yp = y_in * K * std::sinh(KL) + yp_in * std::cosh(KL);
        } else {
            double y_in = p.y, yp_in = p.yp;
            p.y = y_in * std::cos(KL) + yp_in * std::sin(KL) / K;
            p.yp = -y_in * K * std::sin(KL) + yp_in * std::cos(KL);
            double x_in = p.x, xp_in = p.xp;
            p.x = x_in * std::cosh(KL) + xp_in * std::sinh(KL) / K;
            p.xp = x_in * K * std::sinh(KL) + xp_in * std::cosh(KL);
        }

        if (m_gradient > 0) {
            p.xp += fringe_strength * p.x;
            p.yp -= fringe_strength * p.y;
        } else {
            p.xp -= fringe_strength * p.x;
            p.yp += fringe_strength * p.y;
        }
        break;
    }

    case ElementType::Dipole: {
        if (!m_geomSet || p.charge == 0 || m_B <= 0.0) {          // нет геометрии/поля — дрейф
            p.x += m_len * std::tan(p.xp);
            p.y += m_len * std::tan(p.yp);
            break;
        }
        if (!bendInAperture(m_geom, p.x, p.y)) { p.alive = false; break; }   // вход
        double pm, et;
        momentumOf(p, pm, et);
        const double rho = pm / (BRHO_PER_T * std::abs(p.charge) * m_B);     // радиус ЭТОЙ частицы
        double x = p.x, xp = p.xp, y = p.y, yp = p.yp;
        if (!bendTransport(m_geom, rho, x, xp, y, yp)) { p.alive = false; break; }
        p.x = x; p.xp = xp; p.y = y; p.yp = yp;
        break;
    }

    case ElementType::DipoleEdge: {          // только для старых конфигов
        double tanPsi = std::tan(std::abs(m_angle));
        double factor = (tanPsi * m_correction) / m_R;
        if (m_angle > 0) { p.xp += factor * p.x; p.yp -= factor * p.y; }
        else             { p.xp -= factor * p.x; p.yp += factor * p.y; }
        break;
    }

    case ElementType::ElectrostaticDeflector: {
        if (!m_geomSet || p.charge == 0 || m_voltage <= 0.0) {    // выключен — дрейф
            p.x += m_len * std::tan(p.xp);
            p.y += m_len * std::tan(p.yp);
            break;
        }
        if (!bendInAperture(m_geom, p.x, p.y)) { p.alive = false; break; }
        double pm, et;
        momentumOf(p, pm, et);
        const double beta = pm / et;
        const double E0 = m_voltage * 1000.0 / m_gap;                         // В/м
        const double rho = pm * beta * 1.0e6 / (std::abs(p.charge) * E0);    // радиус ЭТОЙ частицы
        double x = p.x, xp = p.xp, y = p.y, yp = p.yp;
        if (!bendTransport(m_geom, rho, x, xp, y, yp)) { p.alive = false; break; }
        p.x = x; p.xp = xp; p.y = y; p.yp = yp;
        break;
    }
    }
}

bool SeparatorElement::apertureCheck(const Particle& p) const {
    switch (m_type) {
    case ElementType::Drift:
    case ElementType::Quadrupole:
        return (p.x * p.x + p.y * p.y) <= m_aperture * m_aperture;
    case ElementType::Dipole:
    case ElementType::ElectrostaticDeflector:
        if (m_geomSet) return bendInAperture(m_geom, p.x, p.y);
        return (std::abs(p.x) <= m_xAperture) && (std::abs(p.y) <= m_yApertureDip);
    default:
        return true;
    }
}
