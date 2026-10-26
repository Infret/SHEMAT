// fieldelement.h — элементы как «комнаты с полем» для field-движка
#pragma once
#include "fieldcore.h"
#include "integrator.h"
#include <memory>
#include <string>

namespace ft {

// Поворот рамки на 180° вокруг оси z: ex -> -ex, ey -> -ey (правая тройка сохраняется).
// Так элемент, изгибающий в сторону +x, считается как «обычный» (изгиб к -x) в зеркальной рамке.
inline Frame flipFrame(const Frame& f) {
    Frame o = f;
    o.ex = -f.ex;
    o.ey = -f.ey;
    return o;
}

class FieldElement {
public:
    virtual ~FieldElement() = default;

    virtual std::string name() const = 0;
    virtual double designLength() const = 0;
    virtual double exitZ() const = 0;

    // Поле в ГЛОБАЛЬНЫХ координатах и компонентах
    virtual FieldFn makeField(const Frame& entryFrame) const = 0;

    // Апертура: принимает координаты ОТНОСИТЕЛЬНО ДИЗАЙН-ОРБИТЫ (ориентация x — как у входной рамки)
    virtual bool apertureLocal(const Vec3& rLoc) const = 0;

    // Перевод точки из локальных координат входной рамки в координаты относительно дизайн-орбиты
    virtual Vec3 designLocal(const Vec3& rLoc) const { return rLoc; }

    virtual Frame exitFrame(const Frame& entry) const;
    virtual bool isDrift() const { return false; }

    // Элементы с наклонными гранями (диполь): движок летит прямо до входной грани,
    // делает краевой «толчок», интегрирует до выходной грани, снова «толчок»,
    // затем летит прямо до выходной плоскости.
    virtual bool  hasFaces() const { return false; }
    virtual Frame entryFace(const Frame& entry) const { return entry; }
    virtual Frame exitFace(const Frame& entry) const { return exitFrame(entry); }
    virtual void  faceKick(TrackState&, const Frame&, bool /*entrance*/) const {}
};

class FieldDrift : public FieldElement {
public:
    FieldDrift(double L, double apertureRadius = -1.0, const std::string& nm = "drift")
        : m_L(L), m_ap(apertureRadius), m_name(nm) {}
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    FieldFn makeField(const Frame&) const override {
        return [](const Vec3&) { return FieldSample(); };
    }
    bool apertureLocal(const Vec3& r) const override {
        return m_ap <= 0.0 || (r.x * r.x + r.y * r.y) <= m_ap * m_ap;
    }
    bool isDrift() const override { return true; }
private:
    double m_L, m_ap;
    std::string m_name;
};

class FieldQuadrupole : public FieldElement {
public:
    FieldQuadrupole(double G_Tpm, double L, double apertureRadius,
                    bool hardEdge, const EngeCoeffs& ec, double lambda,
                    const std::string& nm = "quad");
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    FieldFn makeField(const Frame& entry) const override;
    bool apertureLocal(const Vec3& r) const override;
private:
    double m_G, m_L, m_ap, m_lambda;
    bool   m_hard;
    EngeCoeffs m_ec;
    std::string m_name;
};

// Диполь. B0 — модуль поля, Тл. Rdesign — радиус дизайн-орбиты (геометрия магнита), angle — дизайн-угол.
// bendSign: +1 изгиб к -x, -1 к +x (для положительного заряда). e1, e2 — углы граней, рад.
// Апертура по x — [xMin, xMax] относительно дизайн-орбиты, по y — ±yHalf.
class FieldDipole : public FieldElement {
public:
    FieldDipole(double B0_T, double Rdesign, double angleRad, int bendSign,
                double e1, double e2, double xMin, double xMax, double yHalf,
                bool hardEdge, const EngeCoeffs& ec, double lambda,
                const std::string& nm = "dipole");
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    double bendAngle() const { return m_L / m_R; }
    FieldFn makeField(const Frame& entry) const override;
    bool apertureLocal(const Vec3& r) const override;
    Vec3 designLocal(const Vec3& rLoc) const override;
    Frame exitFrame(const Frame& entry) const override;
    bool  hasFaces() const override { return true; }
    Frame entryFace(const Frame& entry) const override;
    Frame exitFace(const Frame& entry) const override;
    void  faceKick(TrackState& st, const Frame& entry, bool entrance) const override;
private:
    Frame eff(const Frame& entry) const { return m_flip ? flipFrame(entry) : entry; }
    double m_B0, m_R, m_L, m_e1, m_e2, m_xMin, m_xMax, m_yAp, m_lambda;
    bool   m_flip, m_hard;
    EngeCoeffs m_ec;
    std::string m_name;
};

// Цилиндрический электростатический дефлектор. E0 — модуль поля на дизайн-орбите, В/м (U/gap).
class FieldElectrostaticDeflector : public FieldElement {
public:
    FieldElectrostaticDeflector(double E0_Vpm, double Rdesign, double angleRad, int bendSign,
                                double xHalfAperture, double yHalfAperture,
                                bool hardEdge, const EngeCoeffs& ec, double lambda,
                                const std::string& nm = "deflector");
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    double bendAngle() const { return m_L / m_R; }
    FieldFn makeField(const Frame& entry) const override;
    bool apertureLocal(const Vec3& r) const override;
    Vec3 designLocal(const Vec3& rLoc) const override;
    Frame exitFrame(const Frame& entry) const override;
private:
    Frame eff(const Frame& entry) const { return m_flip ? flipFrame(entry) : entry; }
    double m_E0, m_R, m_L, m_xAp, m_yAp, m_lambda;
    bool   m_flip, m_hard;
    EngeCoeffs m_ec;
    std::string m_name;
};

using FieldElementPtr = std::shared_ptr<FieldElement>;

} // namespace ft
