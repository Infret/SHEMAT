#include "periodictable.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QtMath>

PeriodicTable::PeriodicTable(QObject* parent)
    : QObject(parent)
{
}

bool PeriodicTable::loadFromTxt(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "PeriodicTable::loadFromTxt: cannot open" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    file.close();

    content.replace("\r\n", "\n");
    QStringList blocks = content.split("\n\n", Qt::SkipEmptyParts);

    for (const QString &block : blocks) {
        QStringList lines = block.split('\n', Qt::SkipEmptyParts);
        if (lines.isEmpty()) continue;

        QString headerLine = lines[0];
        QStringList headerParts = headerLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (headerParts.size() < 6) {
            qWarning() << "Invalid header line (less than 6 parts):" << headerLine;
            continue;
        }

        bool okZ, okA;
        int Z = headerParts[0].toInt(&okZ);
        int A_header = headerParts[1].toInt(&okA);
        if (!okZ || !okA) {
            qWarning() << "Failed to parse Z or A in header:" << headerLine;
            continue;
        }

        bool isNumber;
        headerParts[2].toDouble(&isNumber);
        if (isNumber) {
            qWarning() << "Expected symbol in third part, but got number:" << headerLine;
            continue;
        }

        QString symbol = headerParts[2];
        QString name = headerParts[3];

        double atomicMass, density;
        bool okMass = true, okDensity = true;
        atomicMass = headerParts[4].toDouble(&okMass);
        density = headerParts[5].toDouble(&okDensity);
        if (!okMass) atomicMass = qQNaN();
        if (!okDensity) density = qQNaN();

        Element elem(Z, symbol, name, density);

        for (int i = 1; i < lines.size(); ++i) {
            QString isoLine = lines[i];
            QStringList isoParts = isoLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (isoParts.size() < 3) {
                qWarning() << "Isotope line has too few parts:" << isoLine;
                continue;
            }

            bool okA;
            int A = isoParts[0].toInt(&okA);
            if (!okA) {
                qWarning() << "Failed to parse A in isotope line:" << isoLine;
                continue;
            }

            double mass = 0.0, isoDensity = 0.0;
            Isotope iso;
            iso.A = A;
            iso.mass = mass;

            // если в файле плотность не указана или равна 0 → ставим плотность элемента
            if (!okDensity || qFuzzyIsNull(isoDensity)) {
                iso.density = density;
            } else {
                iso.density = isoDensity;
            }

            elem.addIsotope(iso);
        }

        m_elementsByZ.insert(Z, elem);
        m_elementsBySymbol.insert(symbol, elem);
    }

    qDebug() << "PeriodicTable::loadFromTxt: loaded" << m_elementsByZ.size() << "elements.";
    return true;
}

const Element* PeriodicTable::getElementByZ(int Z) const
{
    auto it = m_elementsByZ.find(Z);
    if (it != m_elementsByZ.end()) return &it.value();
    return nullptr;
}

const Element* PeriodicTable::getElementBySymbol(const QString& symbol) const
{
    auto it = m_elementsBySymbol.find(symbol);
    if (it != m_elementsBySymbol.end()) return &it.value();
    return nullptr;
}

const Compound* PeriodicTable::getCompoundByName(const QString& name) const
{
    auto it = m_compoundsByName.find(name);
    if (it != m_compoundsByName.end()) return &it.value();
    return nullptr;
}

QList<Element> PeriodicTable::getAllElements() const
{
    QList<Element> list;
    for (auto it = m_elementsByZ.constBegin(); it != m_elementsByZ.constEnd(); ++it) {
        list.append(it.value());
    }
    return list;
}

QList<Compound> PeriodicTable::getAllCompounds() const
{
    QList<Compound> list;
    for (auto it = m_compoundsByName.constBegin(); it != m_compoundsByName.constEnd(); ++it) {
        list.append(it.value());
    }
    return list;
}
