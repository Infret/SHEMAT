#ifndef SEPARATORELEMENT_H
#define SEPARATORELEMENT_H

#include "particle.h"
#include "bendmodel.h"
#include <cmath>

enum class ElementType {
    Drift,
    Quadrupole,
    Dipole,
    DipoleEdge, // (оставляем для совместимости, в новом парсере не используется)
    ElectrostaticDeflector
};

class SeparatorElement
{
public:
    SeparatorElement(ElementType type);

    // Drift:      (L, aperture)
    // Quadrupole: (L, G_кГс/м, aperture, fringe)
    // Dipole:     (B_Тл, R, angle_град, gap, e1_град, e2_град, xmin, xmax)   B>0 — влево, B<0 — вправо
    // Deflector:  (U_кВ, R, angle_град, gap, xmin, xmax)                      U<0 — влево, U>0 — вправо
    void setParameters(double param1 = 0.0, double param2 = 0.0, double param3 = 0.0,
                       double param4 = 0.0, double param5 = 0.0, double param6 = 0.0,
                       double param7 = 0.0, double param8 = 0.0);

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
    double getYApertureDefl() const { return m_yAperture; }

private:
    ElementType m_type;
    double m_len;
    double m_aperture;
    double m_gradient;
    double m_gap;
    double m_voltage;      // кВ (модуль), дефлектор
    double m_yAperture;
    double m_B;            // Тл (модуль), диполь
    double m_R;
    double m_angle;
    double m_e1, m_e2;
    double m_xmin, m_xmax;
    double m_xAperture;
    double m_yApertureDip;
    double m_radius;
    double m_correction;
    double m_fringe;
    QString m_name;

    BendGeom m_geom;       // геометрия диполя/дефлектора
    bool m_geomSet = false;
};

#endif // SEPARATORELEMENT_H
