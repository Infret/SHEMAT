#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include "element.h"
#include <QObject>
#include <QMap>
#include <QFile>
#include <QTextStream>

struct CompoundComponent {
    QString elementSymbol;
    int A; // массовое число (0 для природной смеси)
    int atomCount; // количество атомов этого типа в соединении
    double massFraction; // массовая доля (опционально, для обратной совместимости)
};

struct Compound {
    QString formula;
    QString name;
    double density; // общая плотность составной мишени
    QVector<CompoundComponent> components;
};

class PeriodicTable : public QObject
{
    Q_OBJECT

public:
    PeriodicTable(QObject* parent = nullptr);
    bool loadFromTxt(const QString& filePath);

    const Element* getElementByZ(int Z) const;
    const Element* getElementBySymbol(const QString& symbol) const;
    const Compound* getCompoundByName(const QString& name) const;

    QList<Element> getAllElements() const;
    QList<Compound> getAllCompounds() const;

private:
    bool parseElement(QTextStream& in, QString& firstLine);
    bool parseCompound(QTextStream& in, QString& firstLine);

    QMap<int, Element> m_elementsByZ;
    QMap<QString, Element> m_elementsBySymbol;
    QMap<QString, Compound> m_compoundsByName;
};

#endif // PERIODICTABLE_H
