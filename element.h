#ifndef ELEMENT_H
#define ELEMENT_H

#include <QString>
#include <QList>

class Isotope {
public:
    int A = 0;
    double mass = 0.0;
    double density = 0.0;

    Isotope() {}
    Isotope(int a, double m, double d) : A(a), mass(m), density(d) {}

    bool isValid() const { return A > 0; }
};


class Element
{
public:
    Element(int Z = 0, const QString& symbol = "", const QString& name = "", double commonDensity = 0.0);

    int getZ() const;
    QString getSymbol() const;
    QString getName() const;
    double getCommonDensity() const;
    Isotope getIsotopeByA(int A) const;
    void addIsotope(const Isotope& iso);
    QList<Isotope> getAllIsotopes() const;
    const Isotope* findIsotope(int A) const;

    // Возвращает атомную массу для элемента (если есть хотя бы один изотоп с mass>0 — берёт первый),
    // иначе вернёт 0.
    double getAtomicMass() const;

    // Возвращает атомную массу (g/mol) для конкретного изотопа A; если нет — вернёт 0.
    double getAtomicMassForIsotope(int A) const;

private:
    int m_Z;
    QString m_symbol;
    QString m_name;
    double m_commonDensity;
    QList<Isotope> m_isotopes;
};

#endif // ELEMENT_H
