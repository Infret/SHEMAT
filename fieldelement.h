// fieldelement.h — элементы как «комнаты с полем» для field-движка
#pragma once
#include "fieldcore.h"
#include "integrator.h"
#include <memory>
#include <string>

namespace ft {

// Абстрактный элемент. Поле задаётся в ЛОКАЛЬНЫХ координатах входной рамки:
//   z — дизайн-ось (0 на входе), x — плоскость изгиба, y — вертикаль.
class FieldElement {
public:
    virtual ~FieldElement() = default;

    virtual std::string name() const = 0;
    virtual double designLength() const = 0;            // длина вдоль дизайн-оси, м
    virtual double exitZ() const = 0;                   // z_local выходной плоскости

    // Глобальное поле (замыкает на себе локальную модель и рамку)
    virtual FieldFn makeField(const Frame& entryFrame) const = 0;

    // Апертура в локальных координатах (проверяется каждый шаг)
    virtual bool apertureLocal(const Vec3& rLoc) const = 0;

    // Обновление референсной рамки линии после элемента (для изгибающих)
    virtual Frame exitFrame(const Frame& entry) const;

    // Дрейфы летим аналитически — это главный ускоритель
    virtual bool isDrift() const { return false; }
};

// ---------------------------------------------------------------
// Дрейф: поля нет. apertureRadius <= 0 — апертура не проверяется.
// ---------------------------------------------------------------
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

// ---------------------------------------------------------------
// Квадруполь: Bx = G*y*S(z), By = G*x*S(z), Bz = G*x*y*S'(z)  (Максвелл, ведущий порядок)
//   G — градиент, Тл/м;  S(z) = Enge(-z)*Enge(z-L) — краевые поля;
//   hardEdge — ступенька S=1 внутри [0,L] (для сопоставления с матрицами).
// ---------------------------------------------------------------
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

// ---------------------------------------------------------------
// Диполь (сектор с наклонными гранями):
//   By = B0 * D1(s1) * D2(s2) — произведение входного и выходного края Энге;
//   s1, s2 — расстояния, перпендикулярные граням (отрицательные внутри);
//   e1, e2 — углы граней (рад);  Rref — радиус референсной частицы;
//   L — длина дуги референсной траектории, м;  B0 — поле, Тл.
//   Максвелл (ведущий порядок): Bx = y*dBy/dx, Bz = y*dBy/dz.
// ---------------------------------------------------------------
class FieldDipole : public FieldElement {
public:
    FieldDipole(double B0, double Larc, double Rref,
                double e1, double e2, double p,
                double xHalfAperture, double yHalfAperture,
                bool hardEdge, const EngeCoeffs& ec, double lambda,
                const std::string& nm = "dipole");
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    double bendAngle() const { return m_L / m_R; }
    FieldFn makeField(const Frame& entry) const override;
    bool apertureLocal(const Vec3& r) const override;
    Frame exitFrame(const Frame& entry) const override;
private:
    double m_B0, m_L, m_R, m_e1, m_e2, m_xAp, m_yAp, m_lambda;
    bool   m_hard;
    EngeCoeffs m_ec;
    std::string m_name;
};

// ---------------------------------------------------------------
// Электростатический дефлектор (изгиб в плоскости x):
//   Ex = E0*S(z), Ez = x*E0*S'(z)  (Максвелл, ведущий порядок)
//   E0 = U[В]/gap — поле между пластинами.
//   Rref — дизайн-радиус референсной частицы (нужен для поворота рамки).
// ---------------------------------------------------------------
class FieldElectrostaticDeflector : public FieldElement {
public:
    FieldElectrostaticDeflector(double E0_Vpm, double L, double Rref,
                                double xHalfAperture, double yHalfAperture,
                                bool hardEdge, const EngeCoeffs& ec, double lambda,
                                const std::string& nm = "deflector");
    std::string name() const override { return m_name; }
    double designLength() const override { return m_L; }
    double exitZ() const override { return m_L; }
    double bendAngle() const { return m_L / m_R; }
    FieldFn makeField(const Frame& entry) const override;
    bool apertureLocal(const Vec3& r) const override;
    Frame exitFrame(const Frame& entry) const override;
private:
    double m_E0, m_L, m_R, m_xAp, m_yAp, m_lambda;
    bool   m_hard;
    EngeCoeffs m_ec;
    std::string m_name;
};

using FieldElementPtr = std::shared_ptr<FieldElement>;

} // namespace ft
