#ifndef SEPARATORELEMENT_H
#define SEPARATORELEMENT_H

#include "particle.h"
#include <cmath>

enum class ElementType {
    Drift,
    Quadrupole,
    Dipole,
    DipoleEdge,
    ElectrostaticDeflector
};

class SeparatorElement
{
public:
    SeparatorElement(ElementType type);
    void setParameters(double param1 = 0.0, double param2 = 0.0, double param3 = 0.0,
                       double param4 = 0.0, double param5 = 0.0, double param6 = 0.0);
    void transport(Particle& p) const;
    bool apertureCheck(const Particle& p) const;
    double getLength() const { return m_len; }
    ElementType getType() const { return m_type; }
    void setName(const QString& name) { m_name = name; }
    QString getName() const { return m_name; }


    double getAperture() const { return m_aperture; }
    double getXAperture() const { return m_xAperture; }
    double getYApertureDip() const { return m_yApertureDip; }
    double getGap() const { return m_gap; }
    double getYApertureDefl() const { return m_yAperture; } // для дефлектора


private:
    ElementType m_type;
    double m_len;          // длина (м) для Drift, Quad, Dipole, Deflector
    double m_aperture;     // апертура (м) для Drift, Quad
    double m_gradient;     // градиент (кГс/м) для Quad
    double m_gap;          // зазор (м) для Deflector, Dipole
    double m_voltage;      // напряжение (кВ) для Deflector
    double m_yAperture;    // вертикальная апертура (м) для Deflector
    // Для Dipole:
    double m_radius;       // радиус диполя (м) (вместо m_B)
    double m_xAperture;    // горизонтальная апертура (м) для Dipole
    double m_yApertureDip; // вертикальная апертура (м) для Dipole
    // Для DipoleEdge:
    double m_angle;        // угол наклона грани (рад)
    double m_R;            // радиус кривизны (м) (используется в краях)
    double m_correction;   // множитель (renter/rexit)
    // Для Quadrupole (краевые поля)
    double m_fringe;       // доля длины, занимаемая краевым полем
    // Имя элемента
    QString m_name;
};

#endif // SEPARATORELEMENT_H
