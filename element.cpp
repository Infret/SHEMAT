#include "element.h"
#include <QDebug>

Element::Element(int Z, const QString& symbol, const QString& name, double commonDensity)
    : m_Z(Z), m_symbol(symbol), m_name(name), m_commonDensity(commonDensity)
{}

int Element::getZ() const { return m_Z; }
QString Element::getSymbol() const { return m_symbol; }
QString Element::getName() const { return m_name; }
double Element::getCommonDensity() const { return m_commonDensity; }

void Element::addIsotope(const Isotope& iso) {
    m_isotopes.append(iso);
}

QList<Isotope> Element::getAllIsotopes() const {
    return m_isotopes;
}

const Isotope* Element::findIsotope(int A) const {
    if (A <= 0) return nullptr;
    for (const Isotope& iso : m_isotopes) {
        if (iso.A == A) return &iso;
    }
    return nullptr;
}

double Element::getAtomicMass() const {
    if (!m_isotopes.isEmpty()) {
        const Isotope& first = m_isotopes.first();
        if (first.mass > 0.0) return first.mass;
        if (first.A > 0) return static_cast<double>(first.A);
    }
    // fallback: если вообще ничего нет
    qWarning() << "Element::getAtomicMass(): no isotopes for" << m_symbol;
    return 0.0;
}

double Element::getAtomicMassForIsotope(int A) const {
    const Isotope* iso = findIsotope(A);
    if (iso) {
        if (iso->mass > 0.0) return iso->mass;
        if (iso->A > 0) return static_cast<double>(iso->A);
    }
    qWarning() << "Element::getAtomicMassForIsotope(): isotope not found A=" << A << "for" << m_symbol;
    return 0.0;
}
Isotope Element::getIsotopeByA(int A) const {
    for (const Isotope &iso : m_isotopes) {
        if (iso.A == A) return iso;
    }
    return Isotope();
}
