#include "separatorelement.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

SeparatorElement::SeparatorElement(ElementType type)
    : m_type(type), m_len(0), m_aperture(1e6), m_gradient(0), m_gap(0),
      m_voltage(0), m_yAperture(0.15), m_radius(0), m_xAperture(0.1),
      m_yApertureDip(0.05), m_angle(0), m_R(1), m_correction(0.05), m_fringe(0.2), m_name("")
{}
void SeparatorElement::setParameters(double param1, double param2, double param3,
                                     double param4, double param5, double param6)
{
    switch (m_type) {
    case ElementType::Drift:
        m_len = param1;
        m_aperture = param2 > 0 ? param2 : 1e6;
        qDebug() << "Drift setParameters: m_len=" << m_len << "m_aperture=" << m_aperture;
        break;
    case ElementType::Quadrupole:
        m_len = param1;
        m_gradient = param2;
        m_aperture = param3;
        m_fringe = (param4 > 0) ? param4 : 0.2;
        qDebug() << "setParameters Quad: name=" << m_name << " aperture=" << m_aperture;
        break;
    case ElementType::Dipole:
        m_len = param1;       // эффективная длина (м)
        m_gap = param2;       // зазор (м)
        m_radius = param3;
        m_xAperture = param4;
        m_yApertureDip = param5;
        break;
    case ElementType::DipoleEdge:
        m_angle = param1;     // угол наклона грани (рад)
        m_R = param2;         // радиус кривизны (м)
        m_correction = param3;
        break;
    case ElementType::ElectrostaticDeflector:
        m_len = param1;
        m_gap = param2;
        m_voltage = param3;
        m_yAperture = param4;
        break;
    }
}

//void SeparatorElement::setDipoleField(double B)
//{
//    if (m_type == ElementType::Dipole) m_B = B;
//}

void SeparatorElement::transport(Particle& p) const
{
    if (!p.alive) return;

    switch (m_type) {
    case ElementType::Drift:
        p.x += m_len * std::tan(p.xp);
        p.y += m_len * std::tan(p.yp);
        break;

    case ElementType::Quadrupole: {
        // Параметры краевых полей (доля длины линзы)
        const double fringe_factor = 0.2; // 20% - типичное значение
        double fringe_strength = 0.0;

        // K = sqrt(|q| * G / (1.439 * sqrt(M * E))) (G в кГс/м, M в а.е.м., E в МэВ)
        double K = std::sqrt(std::abs(p.charge * m_gradient) / (1.439 * std::sqrt(p.mass * p.energy)));
        double KL = K * m_len;

        // Вычисляем силу краевой линзы (для тонкой линзы фокусное расстояние F = 2/(K * fringe_len))
        // Используем приближение: Δx' = ∓ (K * fringe_len / 2) * x
        fringe_strength = K * m_len * fringe_factor / 2.0;

        // Входной край (применяется перед основной матрицей)
        if (m_gradient > 0) {
            // Для фокусирующего квадруполя: входной край дефокусирует по X и фокусирует по Y
            p.xp -= fringe_strength * p.x;
            p.yp += fringe_strength * p.y;
        } else {
            // Для дефокусирующего квадруполя: входной край делает наоборот
            p.xp += fringe_strength * p.x;
            p.yp -= fringe_strength * p.y;
        }

        // Основной транспорт (матрица идеального квадруполя)
        if (m_gradient > 0) {
            // фокусировка X
            double x_in = p.x, xp_in = p.xp;
            p.x = x_in * std::cos(KL) + xp_in * std::sin(KL) / K;
            p.xp = -x_in * K * std::sin(KL) + xp_in * std::cos(KL);
            // дефокусировка Y
            double y_in = p.y, yp_in = p.yp;
            p.y = y_in * std::cosh(KL) + yp_in * std::sinh(KL) / K;
            p.yp = y_in * K * std::sinh(KL) + yp_in * std::cosh(KL);
        } else {
            // фокусировка Y
            double y_in = p.y, yp_in = p.yp;
            p.y = y_in * std::cos(KL) + yp_in * std::sin(KL) / K;
            p.yp = -y_in * K * std::sin(KL) + yp_in * std::cos(KL);
            // дефокусировка X
            double x_in = p.x, xp_in = p.xp;
            p.x = x_in * std::cosh(KL) + xp_in * std::sinh(KL) / K;
            p.xp = x_in * K * std::sinh(KL) + xp_in * std::cosh(KL);
        }

        // Выходной край (применяется после основной матрицы, знаки противоположны входу)
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
        // Вычисляем поле (кГс) по текущим параметрам частицы
        double brho = 1.439 * std::sqrt(p.mass * p.energy) / std::abs(p.charge);
        double B = brho / m_radius;   // поле в кГс
//        qDebug()<<"BBBBB"<<B<<"Charge:"<<p.charge;
        double R = m_radius;          // радиус траектории (м)
        double theta = m_len / R;     // угол поворота (рад)

        // Матрица транспорта
        double x_in = p.x, xp_in = p.xp;
        p.x = x_in * std::cos(theta) + R * std::sin(theta) * xp_in;
        p.xp = -(1.0 / R) * std::sin(theta) * x_in + std::cos(theta) * xp_in;

        // Вертикаль – дрейф
        p.y += m_len * std::tan(p.yp);
        break;
    }

    case ElementType::DipoleEdge: {
        double tanPsi = std::tan(std::abs(m_angle));
        double factor = (tanPsi * m_correction) / m_R; // m_correction = renter или rexit
        if (m_angle > 0) { // входной край
            p.xp += factor * p.x;
            p.yp -= factor * p.y;
        } else { // выходной край
            p.xp -= factor * p.x;
            p.yp += factor * p.y;
        }
        break;
    }

    case ElementType::ElectrostaticDeflector: {
        if (m_voltage == 0.0) {
            // Дефлектор выключен – просто дрейф
            p.x += m_len * std::tan(p.xp);
            p.y += m_len * std::tan(p.yp);
            break;
        }
        double E_keV = p.energy * 1000.0;
        double U = m_voltage; // кВ
        double R = (2.0 * E_keV * m_gap) / (std::abs(p.charge) * U); // радиус в метрах
        double theta = m_len / R; // угол поворота

        double x_in = p.x, xp_in = p.xp;
        p.x = x_in * std::cos(theta) + R * std::sin(theta) * xp_in;
        p.xp = -(1.0 / R) * std::sin(theta) * x_in + std::cos(theta) * xp_in;

        p.y += m_len * std::tan(p.yp);
        break;
    }
    }
}

bool SeparatorElement::apertureCheck(const Particle& p) const {
    bool pass = true;
//    qDebug() << "Quad aperture check: x=" << p.x << "y=" << p.y << "r=" << sqrt(p.x*p.x+p.y*p.y) << "aperture=" << m_aperture;
    switch (m_type) {
    case ElementType::Drift:
    case ElementType::Quadrupole:
        pass = (p.x * p.x + p.y * p.y) <= m_aperture * m_aperture;
        if (!pass) {
//            qDebug() << "APERTURE FAIL: x=" << p.x << "y=" << p.y
//                     << "r=" << sqrt(p.x*p.x + p.y*p.y)
//                     << "aperture=" << m_aperture;
        }
        break;
    case ElementType::Dipole:
        return (std::abs(p.x) <= m_xAperture) && (std::abs(p.y) <= m_yApertureDip);
    case ElementType::ElectrostaticDeflector:
        return (std::abs(p.x) <= m_gap / 2.0) && (std::abs(p.y) <= m_yAperture);
    default:
        return true;
    }

    return pass;
}
